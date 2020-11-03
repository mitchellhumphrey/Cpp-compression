#ifndef PROTO_FILE_HPP
#define PROTO_FILE_HPP

#include <fstream>
#include <string>

struct proto_file{

    public:
        std::string file_name;
        uint64_t file_length;
        uint16_t file_name_length;
        std::ofstream output_file;
        proto_file(){};



};


#endif