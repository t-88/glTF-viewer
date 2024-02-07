#pragma once
#include <string>



#define ERROR_DY(msg,...) \
    _Pragma("GCC diagnostic push")\
        _Pragma("GCC diagnostic ignored \"-Wformat-extra-args\"")\
    printf("[ERROR] " msg"\n",__VA_ARGS__); \
    _Pragma("GCC diagnostic pop")

#define ERROR(msg) ERROR_DY(msg,NULL) 



std::string load_file_src(const char* file_path);
