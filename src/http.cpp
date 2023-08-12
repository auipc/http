#include <http.h>
#include <iostream>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <mime.h>
#include <response.h>
#include <glog/logging.h>
#include <gflags/gflags.h>
#include <time.h> 
#include <limits.h>
#include <stdlib.h>
#define DEFAULT_FILE "./index.html"
namespace bfs = boost::filesystem;
namespace bp = boost::process;

std::string UriDecode(const std::string& value)
{
    std::string result;
    result.reserve(value.size());
    
    for (std::size_t i = 0; i < value.size(); ++i)
    {
        auto ch = value[i];
        
        if (ch == '%' && (i + 2) < value.size())
        {
            auto hex = value.substr(i + 1, 2);
            auto dec = static_cast<char>(std::strtol(hex.c_str(), nullptr, 16));
            result.push_back(dec);
            i += 2;
        }
        else if (ch == '+')
        {
            result.push_back(' ');
        }
        else
        {
            result.push_back(ch);
        }
    }
    
    return result;
}

HTTPRequest http::parse_request(std::string data) {
    HTTPRequest httpreq;
    std::map<std::string, std::string> m;
    std::string header;
    std::string method;
    std::string::size_type index;
    std::istringstream resp(data);

    // Get first line
    std::getline(resp, method, ' ');
    httpreq.method = string_to_method(method);
    std::getline(resp, httpreq.page, ' ');
    httpreq.page = UriDecode(httpreq.page);
    // TODO HTTP 2.0 !
    std::string version;
    std::getline(resp, version, ' ');
    version = version.substr(5, 8);
    //printf("LOL: %s\n", version.c_str());

    // Get headers
    while (std::getline(resp, header) && header != "\r") {
      index = header.find(':', 0);
      if(index != std::string::npos) {
        m.insert(std::make_pair(
          boost::algorithm::trim_copy(header.substr(0, index)),
          boost::algorithm::trim_copy(header.substr(index + 1))
        ));
      }
    }

    std::map<std::string, std::string>::iterator host = m.find("Host");
    if(host != m.end())
        httpreq.host = host->second;

    httpreq.extra_headers = m;

    //printf("Method: %s %i\r\n", method.c_str(), httpreq.method);
    //printf("Page: %s\r\n", httpreq.page.c_str());
    //printf("Host: %s\r\n", httpreq.host.c_str());

    return httpreq;
}

LoadFileResult http::load_file(std::string file) {
    LoadFileResult result;
    result.time = (int)time(NULL);
    result.exists = boost::filesystem::exists(file) && boost::filesystem::is_regular_file(file);

    if(result.exists) {
        std::ifstream _file(file, std::ios::binary);
        std::ostringstream ss;
        ss << _file.rdbuf();
        const std::string& s = ss.str();
        std::vector<char> vec(s.begin(), s.end());
        result.content = vec;
    }

    return result;
}

std::vector<char> http::handle_request(std::string data, boost::asio::io_context& io_context) {
    try {
        boost::asio::io_service ios;

        //std::cout << data << std::endl;
        HTTPRequest request = parse_request(data);
        std::string fullpath = "." + request.page;
        if (fullpath.find("..") != std::string::npos) {
            fullpath.replace(fullpath.begin(), fullpath.end(), "..", "");
        }

        if(request.page == "/") fullpath = DEFAULT_FILE;

        std::map<std::string, std::string>::iterator useragent = request.extra_headers.find("User-Agent");

        if(useragent != request.extra_headers.end())
            LOG(INFO) << "Got request on page " << fullpath << " with User-Agent " << request.extra_headers.find("User-Agent")->second;
        else
            LOG(INFO) << "Got request on page " << fullpath;

        ResponseBuilder responsebuild = Response::create(200, "OK")
                            .add_header("Server", "puppets")
                            .add_header("Content-Type", MimeDetector(fullpath).detect())
                            .add_header("Connection", "keep-alive")
                            .file(fullpath);

        if(!boost::filesystem::exists(fullpath) || !boost::filesystem::is_regular_file(fullpath)) {
            responsebuild.rescode(404).reserr("File Not Found");
        }

        Response response = responsebuild;
        std::vector<char> res = response.finalize();
        return res;
    } catch (const std::exception& e) {
        Response response = Response::create(500, "Internal Server Error");
        std::vector<char> res = response.finalize();
        return res;
    }
}
