#pragma once
#include <string>
#include <vector>

class Response;
class ResponseBuilder;

class Header {
public:
    explicit Header(std::string key, std::string value);
    ~Header();
    std::string to_string();
private:
     std::string m_key;
     std::string m_value; 
};

class Response {
public:
    Response();
    ~Response();
    std::vector<char> finalize();
    friend class ResponseBuilder;
    static ResponseBuilder create(int rescode, std::string response);
    std::vector<Header> get_headers() { return headers; }
private:
    std::vector<Header> headers;
    int rescode;
    std::string reserr;
    std::string file;
};

class ResponseBuilder {
private:
    Response response;
public:
    ResponseBuilder(int _rescode, std::string _response);

    operator Response() const { return std::move(response); }

    ResponseBuilder& rescode(int code);
    ResponseBuilder& reserr(std::string response);
    ResponseBuilder& add_header(Header header);
    ResponseBuilder& add_header(std::string key, std::string value);
    ResponseBuilder& file(std::string loc);
};
