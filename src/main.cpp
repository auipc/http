#include <server.h>
#include <response.h>
#include <iostream>
#include <glog/logging.h>
#include <gflags/gflags.h>

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    google::ParseCommandLineFlags(&argc, &argv, true);

    Server server(8080);
    server.start();
    //Header* header = new Header("Connection", "keep-alive");
    Response response = Response::create(200, "OK")
                        .add_header("Connection", "keep-alive")
                        .file("index.html");

    std::vector<char> data = response.finalize();
    std::string s(data.begin(), data.end());
    std::cout << s << "\r\n";
}
