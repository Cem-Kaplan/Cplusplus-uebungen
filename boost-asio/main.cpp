#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

int main() {
    boost::asio::io_context io;

    tcp::resolver resolver(io);
    tcp::socket socket(io);

    auto endpoints = resolver.resolve("example.com", "80");

    boost::asio::connect(socket, endpoints);

    std::cout << "Verbunden\n";
}