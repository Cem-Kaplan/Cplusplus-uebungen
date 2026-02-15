#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main() {
    asio::io_context io;

    asio::steady_timer timer(io, std::chrono::seconds(1));
    timer.async_wait([](const std::error_code&) {
        std::cout << "Timer done";
    });
    io.run();

}