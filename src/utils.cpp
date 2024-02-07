#include "utils.hpp"

std::string load_file_src(const char* file_path) {
    FILE* f = fopen(file_path,"r");
    if(!f) {
        ERROR_DY("failed to open file %s",file_path);
        fclose(f);
    }
    int state;    
    
    state = fseek(f,0,SEEK_END);
    if(state == -1) {
        ERROR_DY("failed to seek the end of the file %s",file_path);
        fclose(f);
        exit(-1);
    }

    long file_size =  ftell(f);
    fseek(f,0,SEEK_SET);

    char* src = (char*)malloc(file_size + 1);
    state = fread(src,1,file_size,f);
    if(state != file_size) {
        ERROR_DY("failed to read the whole file %s",file_path);
        fclose(f);
        exit(-1);
    }
    src[file_size] = '\0';

    std::string file_src(src);

    free(src);
    fclose(f);

    return file_src;
}
