#pragma once
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Server {
public:
    explicit Server(int port=80);
    ~Server();
    void session(tcp::socket sock);
    void start();
private:
    int m_port;
    boost::asio::io_context io_context;
};
