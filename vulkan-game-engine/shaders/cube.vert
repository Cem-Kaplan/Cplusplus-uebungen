#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    float ax = radians(28.0);
    float ay = radians(-35.0);

    float cx = cos(ax);
    float sx = sin(ax);
    float cy = cos(ay);
    float sy = sin(ay);

    mat4 rotX = mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0,  cx,  sx, 0.0,
        0.0, -sx,  cx, 0.0,
        0.0, 0.0, 0.0, 1.0
    );

    mat4 rotY = mat4(
         cy, 0.0, -sy, 0.0,
        0.0, 1.0, 0.0, 0.0,
         sy, 0.0,  cy, 0.0,
        0.0, 0.0, 0.0, 1.0
    );

    mat4 translate = mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, -2.5, 1.0
    );

    float fov = radians(45.0);
    float f = 1.0 / tan(fov * 0.5);
    float aspect = 16.0 / 9.0;
    float zNear = 0.1;
    float zFar = 10.0;

    mat4 proj = mat4(
        f / aspect, 0.0, 0.0, 0.0,
        0.0, -f, 0.0, 0.0,
        0.0, 0.0, zFar / (zNear - zFar), -1.0,
        0.0, 0.0, (zFar * zNear) / (zNear - zFar), 0.0
    );

    gl_Position = proj * translate * rotY * rotX * vec4(inPos, 1.0);
    fragColor = inColor;
}
