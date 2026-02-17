#include <GLFW/glfw3.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <vector>

struct Vec3 {
    float x;
    float y;
    float z;
};

struct Box {
    Vec3 min;
    Vec3 max;
};

static constexpr int WINDOW_WIDTH = 1280;
static constexpr int WINDOW_HEIGHT = 720;

static constexpr float PLAYER_HALF_W = 0.35f;
static constexpr float PLAYER_HEIGHT = 1.8f;
static constexpr float MOVE_ACCEL = 22.0f;
static constexpr float MAX_MOVE_SPEED = 7.5f;
static constexpr float JUMP_SPEED = 9.5f;
static constexpr float GRAVITY = 26.0f;
static constexpr float GROUND_FRICTION = 22.0f;

static float clampf(float v, float lo, float hi) {
    return std::max(lo, std::min(v, hi));
}

static Vec3 add(const Vec3& a, const Vec3& b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

static Vec3 sub(const Vec3& a, const Vec3& b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

static Vec3 scale(const Vec3& v, float s) {
    return {v.x * s, v.y * s, v.z * s};
}

static float dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static Vec3 cross(const Vec3& a, const Vec3& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
    };
}

static float length(const Vec3& v) {
    return std::sqrt(dot(v, v));
}

static Vec3 normalize(const Vec3& v) {
    float len = length(v);
    if (len < 1e-6f) {
        return {0.0f, 0.0f, 0.0f};
    }
    return scale(v, 1.0f / len);
}

static bool intersects(const Box& a, const Box& b) {
    return a.min.x < b.max.x && a.max.x > b.min.x &&
           a.min.y < b.max.y && a.max.y > b.min.y &&
           a.min.z < b.max.z && a.max.z > b.min.z;
}

static Box playerBoxFromPos(const Vec3& pos) {
    return {
        {pos.x - PLAYER_HALF_W, pos.y, pos.z - PLAYER_HALF_W},
        {pos.x + PLAYER_HALF_W, pos.y + PLAYER_HEIGHT, pos.z + PLAYER_HALF_W},
    };
}

static void setPerspective(float fovYDeg, float aspect, float zNear, float zFar) {
    float fovRad = fovYDeg * 3.14159265358979323846f / 180.0f;
    float top = std::tan(fovRad * 0.5f) * zNear;
    float right = top * aspect;
    glFrustum(-right, right, -top, top, zNear, zFar);
}

static void applyLookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
    Vec3 f = normalize(sub(center, eye));
    Vec3 s = normalize(cross(f, up));
    Vec3 u = cross(s, f);

    float m[16] = {
         s.x,  u.x, -f.x, 0.0f,
         s.y,  u.y, -f.y, 0.0f,
         s.z,  u.z, -f.z, 0.0f,
        -dot(s, eye), -dot(u, eye), dot(f, eye), 1.0f,
    };

    glMultMatrixf(m);
}

static void drawBox(const Box& b, const Vec3& baseColor) {
    float r = baseColor.x;
    float g = baseColor.y;
    float bl = baseColor.z;

    auto faceColor = [&](float shade) {
        glColor3f(clampf(r * shade, 0.0f, 1.0f),
                  clampf(g * shade, 0.0f, 1.0f),
                  clampf(bl * shade, 0.0f, 1.0f));
    };

    glBegin(GL_QUADS);

    faceColor(1.00f);
    glVertex3f(b.min.x, b.min.y, b.max.z);
    glVertex3f(b.max.x, b.min.y, b.max.z);
    glVertex3f(b.max.x, b.max.y, b.max.z);
    glVertex3f(b.min.x, b.max.y, b.max.z);

    faceColor(0.78f);
    glVertex3f(b.max.x, b.min.y, b.min.z);
    glVertex3f(b.min.x, b.min.y, b.min.z);
    glVertex3f(b.min.x, b.max.y, b.min.z);
    glVertex3f(b.max.x, b.max.y, b.min.z);

    faceColor(0.92f);
    glVertex3f(b.min.x, b.min.y, b.min.z);
    glVertex3f(b.min.x, b.min.y, b.max.z);
    glVertex3f(b.min.x, b.max.y, b.max.z);
    glVertex3f(b.min.x, b.max.y, b.min.z);

    faceColor(0.85f);
    glVertex3f(b.max.x, b.min.y, b.max.z);
    glVertex3f(b.max.x, b.min.y, b.min.z);
    glVertex3f(b.max.x, b.max.y, b.min.z);
    glVertex3f(b.max.x, b.max.y, b.max.z);

    faceColor(1.12f);
    glVertex3f(b.min.x, b.max.y, b.max.z);
    glVertex3f(b.max.x, b.max.y, b.max.z);
    glVertex3f(b.max.x, b.max.y, b.min.z);
    glVertex3f(b.min.x, b.max.y, b.min.z);

    faceColor(0.68f);
    glVertex3f(b.min.x, b.min.y, b.min.z);
    glVertex3f(b.max.x, b.min.y, b.min.z);
    glVertex3f(b.max.x, b.min.y, b.max.z);
    glVertex3f(b.min.x, b.min.y, b.max.z);

    glEnd();
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Jump and Run 3D", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    std::vector<Box> solids = {
        {{-8.0f, -1.0f, -5.0f}, {62.0f, 0.0f, 5.0f}},
        {{ 4.0f,  1.2f, -1.5f}, { 9.0f, 1.8f, 1.5f}},
        {{12.0f,  2.5f, -1.8f}, {17.0f, 3.1f, 1.8f}},
        {{20.0f,  4.0f, -1.8f}, {24.5f, 4.6f, 1.8f}},
        {{28.0f,  3.0f, -1.6f}, {33.0f, 3.6f, 1.6f}},
        {{36.0f,  5.0f, -1.6f}, {41.0f, 5.6f, 1.6f}},
        {{45.0f,  2.2f, -1.8f}, {50.0f, 2.8f, 1.8f}},
    };

    Box goal = {{56.0f, 0.0f, -0.8f}, {56.8f, 4.5f, 0.8f}};

    Vec3 playerPos{0.0f, 0.2f, 0.0f};
    Vec3 playerVel{0.0f, 0.0f, 0.0f};
    bool onGround = false;
    bool jumpWasDown = false;
    bool won = false;

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        float dt = clampf(static_cast<float>(now - lastTime), 0.0f, 0.033f);
        lastTime = now;

        bool wDown = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
        bool sDown = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;
        bool aDown = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
        bool dDown = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
        bool jumpDown = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            playerPos = {0.0f, 0.2f, 0.0f};
            playerVel = {0.0f, 0.0f, 0.0f};
            onGround = false;
            won = false;
        }

        if (!won) {
            Vec3 input{0.0f, 0.0f, 0.0f};
            if (wDown) {
                input.x += 1.0f;
            }
            if (sDown) {
                input.x -= 1.0f;
            }
            if (aDown) {
                input.z += 1.0f;
            }
            if (dDown) {
                input.z -= 1.0f;
            }

            Vec3 moveDir = normalize(input);
            if (length(moveDir) > 0.0f) {
                playerVel.x += moveDir.x * MOVE_ACCEL * dt;
                playerVel.z += moveDir.z * MOVE_ACCEL * dt;
            } else if (onGround) {
                float speedXZ = std::sqrt(playerVel.x * playerVel.x + playerVel.z * playerVel.z);
                if (speedXZ > 0.0f) {
                    float newSpeed = std::max(0.0f, speedXZ - GROUND_FRICTION * dt);
                    float scaleFactor = (speedXZ > 0.0f) ? (newSpeed / speedXZ) : 0.0f;
                    playerVel.x *= scaleFactor;
                    playerVel.z *= scaleFactor;
                }
            }

            float horizontalSpeed = std::sqrt(playerVel.x * playerVel.x + playerVel.z * playerVel.z);
            if (horizontalSpeed > MAX_MOVE_SPEED) {
                float k = MAX_MOVE_SPEED / horizontalSpeed;
                playerVel.x *= k;
                playerVel.z *= k;
            }

            if (!jumpWasDown && jumpDown && onGround) {
                playerVel.y = JUMP_SPEED;
                onGround = false;
            }
            jumpWasDown = jumpDown;

            playerVel.y -= GRAVITY * dt;

            playerPos.x += playerVel.x * dt;
            Box bx = playerBoxFromPos(playerPos);
            for (const Box& solid : solids) {
                if (!intersects(bx, solid)) {
                    continue;
                }
                if (playerVel.x > 0.0f) {
                    playerPos.x = solid.min.x - PLAYER_HALF_W;
                } else if (playerVel.x < 0.0f) {
                    playerPos.x = solid.max.x + PLAYER_HALF_W;
                }
                playerVel.x = 0.0f;
                bx = playerBoxFromPos(playerPos);
            }

            playerPos.z += playerVel.z * dt;
            Box bz = playerBoxFromPos(playerPos);
            for (const Box& solid : solids) {
                if (!intersects(bz, solid)) {
                    continue;
                }
                if (playerVel.z > 0.0f) {
                    playerPos.z = solid.min.z - PLAYER_HALF_W;
                } else if (playerVel.z < 0.0f) {
                    playerPos.z = solid.max.z + PLAYER_HALF_W;
                }
                playerVel.z = 0.0f;
                bz = playerBoxFromPos(playerPos);
            }

            playerPos.y += playerVel.y * dt;
            onGround = false;
            Box by = playerBoxFromPos(playerPos);
            for (const Box& solid : solids) {
                if (!intersects(by, solid)) {
                    continue;
                }
                if (playerVel.y > 0.0f) {
                    playerPos.y = solid.min.y - PLAYER_HEIGHT;
                } else if (playerVel.y < 0.0f) {
                    playerPos.y = solid.max.y;
                    onGround = true;
                }
                playerVel.y = 0.0f;
                by = playerBoxFromPos(playerPos);
            }

            if (playerPos.y < -8.0f) {
                playerPos = {0.0f, 0.2f, 0.0f};
                playerVel = {0.0f, 0.0f, 0.0f};
                onGround = false;
            }

            if (intersects(playerBoxFromPos(playerPos), goal)) {
                won = true;
                playerVel = {0.0f, 0.0f, 0.0f};
            }
        }

        int fbWidth = WINDOW_WIDTH;
        int fbHeight = WINDOW_HEIGHT;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        float aspect = (fbHeight > 0) ? static_cast<float>(fbWidth) / static_cast<float>(fbHeight) : 16.0f / 9.0f;

        glViewport(0, 0, fbWidth, fbHeight);
        if (won) {
            glClearColor(0.08f, 0.18f, 0.10f, 1.0f);
        } else {
            glClearColor(0.11f, 0.14f, 0.21f, 1.0f);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        setPerspective(65.0f, aspect, 0.1f, 300.0f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        Vec3 target = {playerPos.x + 1.4f, playerPos.y + 0.9f, playerPos.z};
        Vec3 eye = {playerPos.x - 6.0f, playerPos.y + 4.6f, playerPos.z + 9.0f};
        applyLookAt(eye, target, {0.0f, 1.0f, 0.0f});

        drawBox({{-80.0f, -1.2f, -60.0f}, {130.0f, -1.0f, 60.0f}}, {0.18f, 0.20f, 0.23f});

        for (const Box& solid : solids) {
            drawBox(solid, {0.46f, 0.36f, 0.24f});
        }

        drawBox(goal, {0.95f, 0.82f, 0.20f});

        Vec3 playerColor = won ? Vec3{0.30f, 0.92f, 0.34f} : Vec3{0.24f, 0.72f, 0.95f};
        drawBox(playerBoxFromPos(playerPos), playerColor);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
