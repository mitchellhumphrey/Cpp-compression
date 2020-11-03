#include "proto_file.hpp"
#include <vector>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>


int main(int argc, char **argv){

    std::ifstream input(argv[1], std::ios::binary);
    input.seekg(3);
    
    char read_in;
    input.read(&read_in, 1);
    uint8_t amount_of_files = static_cast<uint8_t>(read_in);

    uint16_t file_name_length = 0;
    uint64_t file_length = 0;
    char* file_name;
    std::vector<proto_file*> file_structure;
    for(uint8_t i = 0; i < amount_of_files; ++i){
        input.read(reinterpret_cast<char *>(&file_name_length), 2);
        input.read(reinterpret_cast<char *>(&file_length), 4);
        
        file_name = new char[file_name_length]{};
        input.read(file_name, file_name_length);
        proto_file * temp = new proto_file();
        temp->file_name = std::string(file_name, file_name_length);
        temp->file_name_length = file_name_length;
        temp->file_length = file_length;
        file_structure.push_back(temp);
    }

    std::string folder_name = std::string(argv[1]).substr(0,std::string(argv[1]).size() - 4);

    struct stat info;
    if (stat(folder_name.c_str(), &info) != 0) {
        system(("mkdir " + folder_name).c_str());
    }
    
    
    


    
    char * write_array;
    for (auto &x : file_structure){
        std::string full_file_path = "./"+folder_name+"/"+x->file_name;
        if (stat(full_file_path.c_str(), &info) == 0){
            std::cout<<"File with name "<<x->file_name<<" exists in folder "<<folder_name<<". Skipping dearchiving"<<std::endl;
        }
        else {
            std::ofstream temp_output ("./"+folder_name+"/"+x->file_name, std::ios::binary);
            write_array = new char[x->file_length];
            input.read(write_array, x->file_length);
            temp_output.write(write_array, x->file_length);
            temp_output.close();
        }
        

    }


}