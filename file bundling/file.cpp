#include <iostream>
#include <fstream>
#include <string>
#include "file.hpp"
using std::string;

file::file(std::string input_filename){
    this->file_name = input_filename;
    this->file_name_length = input_filename.length();
    
    this->file_stream = std::ifstream(input_filename,std::ios::binary);
    if(!this->file_stream){
        throw "File named " + this->file_name + " does not exist. Aborting";
    }
    this->file_stream.seekg(0, std::ios::end);
    this->file_length = static_cast<uint64_t>(file_stream.tellg());
    this->file_stream.seekg(0,std::ios::beg);
    this->file_stream.clear();
}