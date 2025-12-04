#include <zmqpp/zmqpp.hpp>

#include <format>
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
    const std::string endpoint = "tcp://localhost:5555";

    // Initialize the 0MQ context.
    zmqpp::context context;

    // Generate a push socket.
    const zmqpp::socket_type type = zmqpp::socket_type::req;
    zmqpp::socket socket(context, type);

    // Open the connection.
    std::cout << "Connecting to hello world server…" << std::endl;
    socket.connect(endpoint);
    for (int request_nbr = 0; request_nbr < 10; ++request_nbr)
    {
        // Send a message.
        std::cout << "Sending Hello " << request_nbr << "…" << std::endl;
        zmqpp::message message;
        message << std::format("Hello {}", request_nbr);
        socket.send(message);

        // Receive the response.
        std::string buffer;
        socket.receive(buffer);
        std::cout << "Received World " << request_nbr << std::endl;
  }
}
