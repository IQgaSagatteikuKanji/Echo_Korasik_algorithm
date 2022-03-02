#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <stack>
#include <queue>
const static short ENGLISH_ALPHABET_SIZE = 26;
const static char ENGLISH_FIRST_LETTER = 'a';
const static char OUT_OF_RANGE = -1;


class Echo_Korasik{
public:
    bool add_word(const std::string &);
    void remove_word(const std::string &);
    void clear_all_words();
    bool contains(const std::string &);
    void print_all_words(std::ostream & );


    size_t search_in_string(const std::string &);

    Echo_Korasik();
    Echo_Korasik(const Echo_Korasik&) = delete;
    Echo_Korasik& operator=(const Echo_Korasik &) = delete;
    ~Echo_Korasik(){};
private:

    struct TrieNode{
        std::shared_ptr<TrieNode> alphabet[ENGLISH_ALPHABET_SIZE] = {};
        std::weak_ptr<TrieNode> suffix;
        bool end_of_string = false;
        TrieNode() = default;
    };

    std::shared_ptr<TrieNode> root;

    int get_index(char );
    void set_all_suffixes();
    std::weak_ptr<TrieNode> find_suffix_ptr_for_word(const std::string &);
    void handle_end_of_string(std::shared_ptr<TrieNode>);


};

int Echo_Korasik::get_index(char ch){
    if(ch < ENGLISH_FIRST_LETTER || ch>(ENGLISH_FIRST_LETTER+ENGLISH_ALPHABET_SIZE)) return OUT_OF_RANGE;
    return ch - ENGLISH_FIRST_LETTER;
}

std::weak_ptr<Echo_Korasik::TrieNode> Echo_Korasik::find_suffix_ptr_for_word(const std::string &word){
    if(word.empty()) return root;

    std::string str = word.substr(1);
    bool suffix_found = false;
    std::shared_ptr<TrieNode> ptr;
    while(!str.empty() && !suffix_found){
        ptr = root;
        size_t index = 0;

        bool case_not_found = false;
        for(int i =0; i<str.size() && !case_not_found; i++){
            index = get_index(str[i]);
            
            if(index != OUT_OF_RANGE){
                if(ptr->alphabet[index] == nullptr) case_not_found =true;
                ptr = ptr->alphabet[index];
            }
            else ptr = root;
            
        }        

        if(!case_not_found) suffix_found = !case_not_found;
        str = str.substr(1);
    }

    std::weak_ptr<TrieNode> result = root;
    if(suffix_found) result = ptr;
    return result;
}

void Echo_Korasik::set_all_suffixes(){
    std::stack<std::shared_ptr<TrieNode>> nodes;
    std::stack<std::string> strs;
    std::shared_ptr<TrieNode> ptr;
    std::string string;
    nodes.push(root);
    strs.push("");
    while(!nodes.empty()){
        ptr = nodes.top();
        string = strs.top();
        nodes.pop();
        strs.pop();

        for(size_t i = 0; i< ENGLISH_ALPHABET_SIZE; i++){
            if(ptr->alphabet[i] != nullptr){
                nodes.push(ptr->alphabet[i]);
                strs.push(string + (char)(ENGLISH_FIRST_LETTER + i));
            
            }
        }
        ptr->suffix = find_suffix_ptr_for_word(string);
    }
}

void Echo_Korasik::print_all_words(std::ostream & out){
    std::stack<std::shared_ptr<TrieNode>> nodes;
    std::stack<std::string> strs;
    std::shared_ptr<TrieNode> ptr;
    std::string string;

    nodes.push(root);
    strs.push("");

    while(!nodes.empty()){
        ptr = nodes.top();
        string = strs.top();
        nodes.pop();
        strs.pop();

        for(size_t i = 0; i< ENGLISH_ALPHABET_SIZE; i++){
            if(ptr->alphabet[i] != nullptr){
                nodes.push(ptr->alphabet[i]);
                strs.push(string + (char)(ENGLISH_FIRST_LETTER + i));
            }
        }
        if(ptr->end_of_string == true) out<< string<< ";\n";
    }
}

size_t Echo_Korasik::search_in_string(const std::string &string){
    size_t count = 0;
    set_all_suffixes();
    size_t index = 0;
    std::shared_ptr<TrieNode> ptr = root;

    for(size_t i = 0; i < string.size(); i++){
        index = get_index(string[i]);
        //char current_char = string[i];
        if(index!= OUT_OF_RANGE)
            if(ptr->alphabet[index] != nullptr) 
                ptr = ptr->alphabet[index];
            else {
                bool loop = true;
                while(loop){
                    ptr = ptr->suffix.lock();
                    if(ptr->end_of_string == true) {
                        count++;
                        handle_end_of_string(ptr);
                    }
                    if(ptr->alphabet[index] != nullptr){
                        loop = false;
                        ptr = ptr->alphabet[index];
                    }
                    if(ptr == root) loop = false;
                }
            
            }

        if(ptr->end_of_string == true) 
            count++;
    if(index == OUT_OF_RANGE) ptr = root;
    }

    return count;
}

void Echo_Korasik::handle_end_of_string(std::shared_ptr<TrieNode>){};

bool Echo_Korasik::contains(const std::string &word){
    std::shared_ptr<TrieNode> ptr = root;
    size_t index = 0;

    for(int i =0; i<word.size(); i++){
        index = get_index(word[i]);
        if(index!=OUT_OF_RANGE)
            if(ptr->alphabet[index] == nullptr) return false;
        else return false;

        ptr = ptr->alphabet[index];
    }
    return ptr->end_of_string;
}

Echo_Korasik::Echo_Korasik(){
    root = std::make_shared<TrieNode>();
    root->suffix = root;
};

bool Echo_Korasik::add_word(const std::string& word){
    std::shared_ptr<TrieNode> ptr = root;
    size_t index = 0;
    for(int i=0; i<word.size();i++){
        index = get_index(word[i]);
        if(index != OUT_OF_RANGE)
        if(ptr->alphabet[index] == nullptr){
            ptr->alphabet[index] = std::make_shared<TrieNode>();
        }
        if(index == OUT_OF_RANGE) return false;
         ptr = ptr->alphabet[index];
    }
    ptr->end_of_string = true;
    return true;
};

void Echo_Korasik::clear_all_words(){
    root = std::make_shared<TrieNode>();
    root->suffix = root;
};

void Echo_Korasik::remove_word(const std::string & word){
    std::stack<std::shared_ptr<TrieNode> *> stack;
    std::shared_ptr<TrieNode> ptr = root;
    size_t index = 0;
    index = get_index(word[0]);
    if(index != OUT_OF_RANGE)
        if(ptr->alphabet[index] == nullptr) return;
        ptr = ptr->alphabet[index];

    for(int i=1; i<word.size();i++){
        index = get_index(word[i]);

        if(index == OUT_OF_RANGE) return;
        if(ptr->alphabet[index] == nullptr) return;


        bool has_other_childs = false;
        for(int j = 0;(j < ENGLISH_ALPHABET_SIZE) && !has_other_childs; j++){
            has_other_childs += (ptr->alphabet[j] != nullptr) && (j != index);
        }
        if(has_other_childs) stack.empty(); else stack.push(&ptr);

        ptr = ptr->alphabet[index];
    }

    bool has_childs = false;
    for(int j = 0;(j < ENGLISH_ALPHABET_SIZE) && !has_childs; j++){
        has_childs += (ptr->alphabet[j] != nullptr);
    }
    if(has_childs) stack.empty();

    ptr->end_of_string = false;

    while(!stack.empty()){
        std::shared_ptr<TrieNode> * ptr = stack.top();
        ptr = nullptr;
        stack.pop();
    }
}

