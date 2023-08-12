#include <iostream>
#include <server.h>
#include <http.h>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <sys/socket.h>

using boost::asio::ip::tcp;

Server::Server(int port) : m_port(port)
{

}

// Add shutdown code
Server::~Server() {}

void Server::session(tcp::socket sock) {
    try {
        const int timeout = 200;
        ::setsockopt(sock.native_handle(), SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof timeout);
        for (;;) {
            boost::asio::streambuf buffer;
            boost::system::error_code error;
            
            if(error == boost::asio::error::eof)
                break;
            else if (error)
                throw boost::system::system_error(error);
            
            size_t length = boost::asio::read_until(sock, buffer, "\r\n\r\n", error);
            if(length <= 0)
                break;

            std::string s( (std::istreambuf_iterator<char>(&buffer)), std::istreambuf_iterator<char>() );

            std::cout << "Received " << length << " bytes of data" << std::endl;
            // Send response
            std::vector<char> response = http::handle_request(s, std::ref(io_context));
            length = boost::asio::write(sock, boost::asio::buffer(response.data(), response.size()));

            if(length > 0) {
                // wait a bit before killing thread
                sock.close();
                //break;
            }
        }
    } catch (std::exception& e) {
        std::cerr << "Exception in thread: " << e.what() << "\n";
    }
}

void Server::start() {
    tcp::acceptor a(io_context, tcp::endpoint(tcp::v4(), m_port));
    for (;;)
    {
        std::thread t(&Server::session, this, a.accept());
        t.detach();
    }
}
