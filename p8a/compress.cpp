#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <bitset>
#include "node.hpp"


//pre:
//post:
//description:
void increment_dictionary(std::map<char, int> &dictionary, char letter){
    //checks if letter is not in dictionary
    if (dictionary.find(letter) == dictionary.end()){
        dictionary.insert({letter, 1});
    }
    else {
        dictionary[letter] += 1;
    }
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
    std::cout<<"In build tree"<<std::endl;
    std::pair<int, node*> left, right;
    node* new_tree;

    while (list.size() > 1){
        std::stable_sort(list.begin(), list.end(), std::less<void>{});
        std::reverse(list.begin(), list.end());
        
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

    char test;
    std::ifstream file (argv[1],std::ios::binary | std::ios::in);
    std::ofstream output (argv[2], std::ios::binary | std::ios::out);
    std::string line;
    std::vector<std::pair<int, char>> char_list;

    if(!file){
        std::cout<<"Could not open file: " << argv[1]<< std::endl;
        return 0;
    }

    while (file.read(&test, 1)){
        increment_dictionary(dictionary, test);

    }

    //now go through dictionary and put into vector
    for (auto const& x : dictionary){
        char_list.push_back({x.second, x.first});
    }
    


    //left is smaller side, right is larger side

    std::vector<std::pair<int, node*>> list = build_dictionary_for_build_tree(char_list);
    tree = build_tree(list);
    
    std::map<char, std::string> table = make_table_from_tree(tree);
    std::map<int, int> amount_of_length; // input is the length of bit prefix, out is the number with that length
 
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

    std::vector<std::pair<std::string, std::string>> before_right = vector_sorted_length(table);
    std::string what_to_write;

    for (auto const& x : before_right){        
        what_to_write += x.first;
        what_to_write += x.second;
    }
    
    while (0!=(what_to_write.length()%8)){
        what_to_write = what_to_write + "0";
    }

    //Writes the table of codes to the file
    while (what_to_write.length()){
        std::string temp = what_to_write.substr(0,8);
        what_to_write.replace(0,8,"");
        unsigned char byte =static_cast<unsigned char>(std::bitset<8>(temp).to_ulong());
        output.write(reinterpret_cast<const char *>(&byte), 1);
    }

    std::string big_code;
    file.clear();
    file.seekg(0, std::ios::beg);

    while (file.read(&test, 1)){
        big_code += table[test];
    }

    unsigned char amount_padding = 0;
    while (0!=(big_code.length()%8)){
        big_code += "0";
        ++amount_padding;
    }
    //writes a byte which says how much padding is at end of file
    output.write(reinterpret_cast<const char *>(&amount_padding), 1);

    //writes the actual compressed file
    while (big_code.length()){
        std::string temp = big_code.substr(0,8);
        big_code.replace(0,8,"");
        unsigned char byte =static_cast<unsigned char>(std::bitset<8>(temp).to_ulong());
        output.write(reinterpret_cast<const char *>(&byte), 1);
    }
    return 0;
}