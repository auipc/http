#pragma once
#include <boost/asio.hpp>
#include <string>
#include <map>
#include <vector>

using boost::asio::ip::tcp;

enum Method {
    GET = 0,
    POST = 1,
    // Mostly unused for the modern web
    PUT = 2,
    UPDATE = 3,
    UNKNOWN = 4,
};

typedef struct _HTTPRequest {
    // TODO make this an enum
    Method method;
    std::string page;
    std::string httpver;
    std::string host;
    std::map<std::string, std::string> extra_headers;
} HTTPRequest;

typedef struct _LoadFileResult {
    std::vector<char> content;
    std::string content_type;
    bool exists;
    int time;
} LoadFileResult;

namespace http {
    // Make this into a lookup table instead...
    inline Method string_to_method(std::string method) {
        if (method == "GET")
            return Method::GET;
        else if (method == "POST")
            return Method::POST;
        else if (method == "PUT")
            return Method::PUT;
        else if (method == "UPDATE")
            return Method::UPDATE;
        else
            return Method::UNKNOWN;
    }

    HTTPRequest parse_request(std::string request);
    LoadFileResult load_file(std::string file);
    std::string setup_headers(int rescode, std::string message, std::string content_type);
    std::vector<char> handle_request(std::string data, boost::asio::io_context& io_context);//tcp::socket sock, std::string data);
};
