#include <iostream>
#include "upload.h"

int main() {
    //std::cout << "Hello, World!" << std::endl;

    char * str[3] = {"png","jpg"};

    std::vector<std::string> vec;

    for (int i = 0; i < 2; ++i) {

        vec.push_back(str[i]);

    }


    std::string filepath = "/Users/gujun/Downloads/2k/";

    std::string url = "http://192.168.1.173/api/personnels";

    int groupId = 40;

    dealBus(filepath,vec,url,groupId);

    return 0;
}