#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <functional>
#include <bitset>
#include "node.hpp"


node* build_tree(std::map<std::string, char> table){
    node* output = new node();
    node* current = output;
    for (auto const &x : table){
        current = output;
        for( char c : x.first){
            if (c =='0'){
                if (current->left == nullptr){
                    current->left = new node();
                }
                current = current->left;
            }
            else{
                if (current->right == nullptr){
                    current->right = new node();
                }
                current = current->right;
            }
            current->character = x.second;
        } 
        
    }
    return output;
}

void print_tree(node* tree, std::string prefix){
    if(tree->left == nullptr && tree->right == nullptr){
        //std::cout<<prefix<<" is the character :"<<tree->character<<std::endl;
    }
    else if(tree->left == nullptr || tree->right == nullptr){
        std::cout<<"there was an error"<<"the code is "<<prefix<<std::endl;
        //std::cout;
    }
    else{
        print_tree(tree->left, prefix + "0");
        print_tree(tree->right, prefix +"1");
    }
}



int main(int argc, char **argv){

    if (argc != 3){
        std::cout<<"Not the correct amount of parameters! \n parameter 1 is input compressed file,"
        " parameter 2 is the name of the output file."
        "\nIt will overwrite a file with the name of parameter 2."<<std::endl;
        return 0;
    }

    std::ifstream input (argv[1],std::ios::binary | std::ios::in);
    std::ofstream output (argv[2], std::ios::binary | std::ios::out);


    if(!input){
        std::cout<<"Could not open file: " << argv[1]<< std::endl;
        return 0;
    }
    input.seekg(0, std::ios::end);
    unsigned long long file_length = input.tellg();
    input.seekg(0);

    char read_char;
    if(!input.read(&read_char, 1)){
        std::cout<<"Input file " << argv[1]<< " is empty"<<std::endl;
        return 0;
    }

    //length of longest code stored in tree
    int biggest_size = static_cast<unsigned char>(read_char);
    //std::cout<<"THE LONGEST ENCODING IS "<<biggest_size<<"BITS"<<std::endl;
    std::vector<unsigned int> amount_for_each_size;
    for (auto i = 0; i <= biggest_size; i++){
        input.read(&read_char, 1);
        amount_for_each_size.push_back(static_cast<unsigned char>(read_char));
    }
    
    int bit_count_for_table = 0;
    unsigned long long index = 0;
    for (auto &x : amount_for_each_size){
        bit_count_for_table += (index * x)+(8 * x);
        index++;
    }

    //std::cout<<bit_count_for_table<<std::endl;

    while (bit_count_for_table % 8) bit_count_for_table++;

    //std::cout<<bit_count_for_table<<std::endl;


    //decode table is a string of 1s and 0s that is the 8 bit representation of character, then the encoding
    std::string decode_table;
    while (bit_count_for_table){
        input.read(&read_char, 1);
        std::bitset<8> temp_byte = read_char;
        decode_table += temp_byte.to_string();
        bit_count_for_table -= 8;
    }

    //char is the decoded character, std::string is the encoded bits that represent the char
    std::map<std::string, char> table;
    index = 0;
    
    for(long unsigned int i = 0; i < amount_for_each_size.size(); i++){
        while(amount_for_each_size[i]){
            std::string temp = decode_table.substr(index,8);
            index += 8;
            table[decode_table.substr(index,i)] = static_cast<char>(std::bitset<8>(temp).to_ulong());
            index += i;
            //std::cout<<"h"<<std::endl;
            amount_for_each_size[i] -= 1;
        }
    }    
    


    node* tree = build_tree(table);
    node* tree_pointer = tree;
    print_tree(tree, "");

    input.read(&read_char, 1);
    int padding = static_cast<int>(read_char);

    std::string string_of_bits;

    char array[131072];
    char write_array[131072];
    index = 0;
    int counter = 0;
    tree_pointer = tree;
    std::cout<<"padding is "<<padding<<std::endl;
    while (input.read(array, sizeof(array)) || input.gcount() > 0){
        int count = input.gcount();
        
        if (count != sizeof(array) || input.tellg() == file_length || input.tellg() == std::char_traits<char>::eof()){
            for(int i = 0; i < (count - 1); ++i){
                for (int j = 7; j >= 0; --j){
                    if(array[i]&(1 << j)){
                        tree_pointer = tree_pointer->right;
                    }
                    else {
                        tree_pointer = tree_pointer->left;
                    }
                    if (tree_pointer->left == nullptr || tree_pointer->right == nullptr){
                        write_array[index] = tree_pointer->character;
                        ++index;
                        tree_pointer = tree;
                    }
                    if (index >= sizeof(write_array)) {
                        output.write(write_array, sizeof(write_array));
                        index = 0;
                        counter++;
                        if (counter % 1024 == 0)std::cout<<"wrote 128 MMMMiiiiiiiB"<<std::endl;
                    }
                }
            }

            
            for (int j = 7; j >= padding; --j){
                if(array[count-1]&(1 << j)){
                    tree_pointer = tree_pointer->right;
                }
                else {
                    tree_pointer = tree_pointer->left;
                }
                if (tree_pointer->left == nullptr || tree_pointer->right == nullptr){
                    write_array[index] = tree_pointer->character;
                    ++index;
                    tree_pointer = tree;
                }
                if (index >= sizeof(write_array)) {
                    output.write(write_array, sizeof(write_array));
                    index = 0;
                    counter++;
                    if (counter % 1024 == 0)std::cout<<"wrote 128 MMMMiiiiiiiB"<<std::endl;
                }
            }

        }

        else{
            for(int i = 0; i < count; ++i){
                for (int j = 7; j >= 0; --j){
                    
                    if(array[i]&(1 << j)){
                        tree_pointer = tree_pointer->right;
                    }
                    else {
                        tree_pointer = tree_pointer->left;
                    }

                    if (tree_pointer->left == nullptr || tree_pointer->right == nullptr){
                        write_array[index] = tree_pointer->character;
                        ++index;
                        tree_pointer = tree;
                    }


                    if (index >= sizeof(write_array)) {
                        output.write(write_array, sizeof(write_array));
                        index = 0;
                        counter++;
                        if (counter % 1024 == 0)std::cout<<"wrote 128 MMMMiiiiiiiB"<<std::endl;
                    }
                }
            }
        }




        
    }
    output.write(write_array, index);
    return 0; 
}