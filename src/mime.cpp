#include <mime.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <boost/algorithm/string.hpp>

MimeDetector::MimeDetector(std::string path) {
    m_path = path;
}

MimeDetector::~MimeDetector() {

}

std::string MimeDetector::detect() {
    std::string extension = "";
    extension = strrchr(m_path.c_str(), '.');
    boost::erase_all(extension, ".");

    if(extension.size() < 0){
        printf("Invalid extension encountered\n");
        return "text/plain";
    }

    std::map<std::string, std::string>::iterator mime = m_extension_types.find(extension);
    if(mime != m_extension_types.end())
        return mime->second;

    return "text/plain";
}
