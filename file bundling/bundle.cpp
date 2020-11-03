#include "file.hpp"
#include <vector>
#include <iostream>


int main(int argc, char **argv){
    std::vector<file> file_vector;
    if (argc < 3){
        std::cout<<"Have to give at least one file to compress!"<<std::endl;
        return -1;
    }
    try{
        for (int i = 2; i < argc; ++i){
            std::string temp(argv[i]);
            file_vector.push_back(file(temp));
        }
    }
    
    catch (std::string e){
        std::cout<<e<<std::endl;
        return -1;
    }
    for (auto &x : file_vector){
        if(!x.file_stream){
            std::cout<<"file "<<x.file_name<<" does not exist"<<std::endl;
        }
    }

    std::string outputname = std::string(argv[1]);
    outputname += ".arp";

    std::ofstream output(outputname, std::ios::binary);


    char magic_number[3] = {'a','r','p'};
    output.write(magic_number, sizeof(magic_number));

    uint16_t amount_of_files = file_vector.size();
    output.write(reinterpret_cast<const char *>(&amount_of_files), 1);

    for (auto &x : file_vector){
        output.write(reinterpret_cast<const char *>(&x.file_name_length), 2);
        output.write(reinterpret_cast<const char *>(&x.file_length), 4);
        output.write(x.file_name.c_str(), x.file_name_length);
    }

    char holder[8096];
    for (auto &x : file_vector){
        while (x.file_stream.read(holder, sizeof(holder))|| x.file_stream.gcount() > 0){
            int count = x.file_stream.gcount();
            output.write(holder, count);
        }
    }

    return 0;
}