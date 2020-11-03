#ifndef FILE_HPP
#define FILE_HPP

#include <fstream>
#include <string>

struct file{

    public:
        std::string file_name;
        std::ifstream file_stream;
        uint64_t file_length;
        uint16_t file_name_length;
        file(std::string input_filename);



};


#endif