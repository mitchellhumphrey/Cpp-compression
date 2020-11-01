#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <bitset>
#include <sstream>
#include "node.hpp"

bool debug = false;

//pre:
//post:
//description:
void increment_dictionary(std::map<char, int> &dictionary, char letter){
    //checks if letter is not in dictionary
    dictionary[letter] += 1;
}


//pre:
//post:
//description:
std::vector<std::pair<int, node*>> build_dictionary_for_build_tree(std::vector<std::pair<int, char>> list){
    std::vector<std::pair<int, node*>> output;

    for(auto const& x : list){
        output.push_back(std::make_pair(x.first, new node(x.second)));
    }

    return output;
}

//pre:
//post:
//description:
node* build_tree(std::vector<std::pair<int, node*>> list){
    std::pair<int, node*> left, right;
    node* new_tree;

    while (list.size() > 1){
        std::stable_sort(list.begin(), list.end(), std::greater<void>{});
        
        //always gonna have more than 1 item in list so no need for if statement to check if vector is empty
        left = list.back();
        list.pop_back();
        right = list.back();
        list.pop_back();
        new_tree = new node(0, left.second, right.second);

        list.push_back(std::make_pair(left.first+right.first, new_tree));   
    }
    return list.back().second;
}


std::map<char, std::string> make_table_from_tree(node* tree, std::string prefix=""){
    std::map<char, std::string> output;
    if (tree->left){
        output.merge(make_table_from_tree(tree->left,prefix+"0"));
    } 
    if (tree->right){
        output.merge(make_table_from_tree(tree->right,prefix+"1"));
    }
    if (!tree->right && !tree->left){
        output[tree->character] = prefix;
    }
    return output;
}

//pre: 
//post:
//description: takes in map made from make_table_from_tree, 
//returns a vector sorted by length of the code (1st index string), 
//and 0th index is the 8bit string of the character value
std::vector<std::pair<std::string, std::string>> vector_sorted_length(std::map <char, std::string> input){
    std::vector<std::pair<std::string, std::string>> output;
    for (auto &x : input){
        output.push_back(std::make_pair(std::bitset<8>(x.first).to_string(), x.second));
    }
    std::sort(output.begin(), output.end(), [] 
        (std::pair<std::string, std::string>& first, std::pair<std::string, std::string>&second){
            return first.second.size() < second.second.size();
        });
    return output;


}

void print_tree(node* tree, std::string prefix){
    if(tree->left == nullptr && tree->right == nullptr){
        std::cout<<prefix<<" is the byte :"<<std::bitset<8>(tree->character).to_string()<<" the char "<<tree->character<<std::endl;
    }
    else if(tree->left == nullptr || tree->right == nullptr){
        std::cout<<"there was an error"<<"the code is "<<prefix<<std::endl;

    }
    else{
        print_tree(tree->left, prefix + "0");
        print_tree(tree->right, prefix +"1");
    }
}

int main(int argc, char **argv){

    if (argc != 3){
        std::cout<<"Not the correct amount of parameters! \n parameter 1 is input text file,"
        " parameter 2 is the name of the output file."
        "\nIt will overwrite a file with the name of parameter 2."<<std::endl;
        return 0;
    }


    std::map<char, int> dictionary;
    struct node* tree = nullptr;

    std::ifstream file (argv[1],std::ios::binary | std::ios::in);
    std::ofstream output (argv[2], std::ios::binary | std::ios::out);
    std::string line;
    std::vector<std::pair<int, char>> char_list;

    if(!file){
        std::cout<<"Could not open file: " << argv[1]<< std::endl;
        return 0;
    }

    if (debug) std::cout<<"about to parse input"<<std::endl;
    char array[1024];
    while (file.read(array, 1024)|| file.gcount() > 0){
        int count = file.gcount();
        for( int i = 0; i < count; ++i){
            increment_dictionary(dictionary, array[i]);
        }
    }
    if (debug) std::cout<<"finished parsing input"<<std::endl;

    //now go through dictionary and put into vector
    for (auto const& x : dictionary){
        char_list.push_back({x.second, x.first});
    }

    if (debug) std::cout<<"finished making vector"<<std::endl;
    


    //left is smaller side, right is larger side

    std::vector<std::pair<int, node*>> list = build_dictionary_for_build_tree(char_list);
    tree = build_tree(list);
    
    if (debug) std::cout<<"finished building tree"<<std::endl;

    std::map<char, std::string> table = make_table_from_tree(tree);
    std::map<int, int> amount_of_length; // input is the length of bit prefix, out is the number with that length
 
    if (debug) std::cout<<"finished making table from tree"<<std::endl;

    for (auto const& x : table){
        amount_of_length[x.second.size()] += 1;        
    }

    unsigned char biggest = 0;
    for (auto const& x : amount_of_length){
        if (x.first > biggest) biggest = x.first;
    }

    //writes the first byte of file, which is length of biggest code
    output.write(reinterpret_cast<const char *>(&biggest), 1);

    //writes the amount of codes from length 0 to length biggest inclusive to file, 1 byte at a time
    for (int i = 0; i <= biggest; ++i){
        unsigned char convert_to_one_byte = amount_of_length[i];
        output.write(reinterpret_cast<const char *>(&convert_to_one_byte), 1);
    }

    if (debug) std::cout<<"finished writing bytes to file"<<std::endl;

    std::vector<std::pair<std::string, std::string>> before_right = vector_sorted_length(table);
    std::string what_to_write;

    for (auto const& x : before_right){        
        what_to_write += x.first;
        what_to_write += x.second;
    }
    
    while (0!=(what_to_write.length()%8)){
        what_to_write = what_to_write + "0";
    }


    unsigned long long index = 0;
    //Writes the table of codes to the file
    while (what_to_write.length() > index){
        std::string temp = what_to_write.substr(index, 8);
        //what_to_write.replace(0, 8, "");
        unsigned char byte =static_cast<unsigned char>(std::bitset<8>(temp).to_ulong());
        output.write(reinterpret_cast<const char *>(&byte), 1);
        index += 8;
    }

    if (debug) std::cout<<"finished writing table to file"<<std::endl;

    std::string big_code;


    file.clear();
    big_code.reserve(file.tellg() * 4);

    
    file.seekg(0, std::ios::beg);

    while (file.read(array, 1024) || file.gcount() > 0){
        int count = file.gcount();
        for (int i = 0; i < count; ++i){
            big_code += table[array[i]];
        }
        
    }
    if (debug) std::cout<<"finished appending to big_code"<<std::endl;

    unsigned char amount_padding = 0;
    while (big_code.length() % 8 != 0){
        big_code += "0";
        ++amount_padding;
    }
    //writes a byte which says how much padding is at end of file
    output.write(reinterpret_cast<const char *>(&amount_padding), 1);

    //writes the actual compressed file
    index = 0;
    while (big_code.length() > index){
        std::string temp = big_code.substr(index,8);
        
        unsigned char byte =static_cast<unsigned char>(std::bitset<8>(temp).to_ulong());
        output.write(reinterpret_cast<const char *>(&byte), 1);
        
        if (debug) if (!(index % 16000000)) std::cout<<"count is "<<index<<std::endl;
        
        index += 8;

    }
    return 0;
}