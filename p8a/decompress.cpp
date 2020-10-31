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

    char read_char;
    if(!input.read(&read_char, 1)){
        std::cout<<"Input file " << argv[1]<< " is empty"<<std::endl;
        return 0;
    }

    //length of longest code stored in tree
    int biggest_size = static_cast<int>(read_char);
    //std::cout<<"THE LONGEST ENCODING IS "<<biggest_size<<"BITS"<<std::endl;
    std::vector<int> amount_for_each_size;
    for (auto i = 0; i <= biggest_size; i++){
        input.read(&read_char, 1);
        amount_for_each_size.push_back(static_cast<int>(read_char));
    }
    
    int bit_count_for_table = 0;
    int index = 0;
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
            std::string temp = decode_table.substr(0,8);
            decode_table.replace(0,8,"");
            table[decode_table.substr(0,i)] = static_cast<char>(std::bitset<8>(temp).to_ulong());
            decode_table.replace(0,i,"");
            //std::cout<<"h"<<std::endl;
            amount_for_each_size[i] -= 1;
        }
    }    
    

    /*for (auto &x : table){
        std::cout<<x.first<<" "<<x.second<<std::endl;
    }8*/

    node* tree = build_tree(table);
    node* tree_pointer = tree;
    print_tree(tree, "");

    input.read(&read_char, 1);
    int padding = static_cast<int>(read_char);

    std::string string_of_bits;

    while (input.read(&read_char, 1)){
        string_of_bits += std::bitset<8>(read_char).to_string();
    }

    index = 0;
    while(string_of_bits.length() >static_cast<unsigned long>(index + padding)){
        tree_pointer = tree;
        while (true){
            if (tree_pointer->left == nullptr || tree_pointer->right == nullptr){
                output.write(&(tree_pointer->character), 1);
                //std::cout<<"wrote the char: "<<tree_pointer->character<<std::endl;
                //std::cin.ignore(1024,'\n');
                //index++;
                break;
            }
            else{
                if (string_of_bits[index]=='0'){
                    tree_pointer = tree_pointer->left;
                    index++;
                }
                else{
                    tree_pointer = tree_pointer->right;
                    index++;
                }
            }
        }
        
    }

    return 0; 
}