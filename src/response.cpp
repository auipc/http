#include <response.h>
#include <sstream>
#include <iostream>
#include <http.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/wait.h>

Header::Header(std::string key, std::string value)
: m_key(key), m_value(value) 
{

}

Header::~Header() {

}

std::string Header::to_string() {
    std::ostringstream ss;
    ss << m_key << ": " << m_value;
    return ss.str(); 
}

Response::Response() {

}

Response::~Response() {

}

// TODO worker that clears every 5m or so
// and cache invalidated files n stuff
std::map<std::string, LoadFileResult> cache;

std::vector<char> Response::finalize() {
    LoadFileResult lfile;
    if (cache.find(file) == cache.end()) {
        lfile = http::load_file(file); 
        cache.insert(std::pair<std::string, LoadFileResult>(file, lfile));
    } else {
        lfile = cache.find(file)->second;
        // Nuke files caches older than 5m
        if (((int)time(NULL) - lfile.time) >= 300) {
            lfile = http::load_file(file); 
            cache[file] = lfile;
        }
    }

    std::stringstream ss;
    ss << "HTTP/1.1 " << rescode << " " << reserr << "\r\n";

    for(Header header : headers)
        ss << header.to_string() << "\r\n";

    ss << "\r\n";
    std::string headers = ss.str();
    std::vector<char> response(headers.begin(), headers.end());
    response.insert(response.end(), lfile.content.begin(), lfile.content.end());

    return response;
}

ResponseBuilder Response::create(int rescode, std::string response) {
   return ResponseBuilder(rescode, response); 
}

ResponseBuilder::ResponseBuilder(int _rescode, std::string _response) {
     rescode(_rescode);
     reserr(_response);
}

ResponseBuilder& ResponseBuilder::rescode(int code) {
    response.rescode = code;
    return *this;
}

ResponseBuilder& ResponseBuilder::reserr(std::string res) {
    response.reserr = res;
    return *this;
}

ResponseBuilder& ResponseBuilder::add_header(Header header) {
    response.headers.push_back(header);
    return *this;
}

ResponseBuilder& ResponseBuilder::add_header(std::string key, std::string value) {
    response.headers.push_back(Header(key, value));
    return *this;
}

ResponseBuilder& ResponseBuilder::file(std::string loc) {
    response.file = loc;
    return *this;
}
