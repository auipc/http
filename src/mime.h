#pragma once
#include <string>
#include <map>

class MimeDetector {
public:
    static std::string detect(std::string path);
private:
    static std::map<std::string, std::string> s_extension_types;
};
