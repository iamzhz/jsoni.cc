#pragma once

#include <string>
#include <iostream>                                
#include <vector>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <unordered_map>

namespace jsoni {

enum TokenType {
    TokenTypeEnd,
    TokenTypeInt, // nn
    TokenTypeFloat, // nn.n
    TokenTypeString, // "xx" 'xx'
    TokenTypeBool, // True False
    TokenTypeSign, // { } ( ) , : [ ]
};
struct Token {
    TokenType type;
    std::string content;
};

class TreeNode {
    public:
    bool isLeaf;
    Token * leaf; // for leaf node
    std::unordered_map<std::string, TreeNode*> dict; // for non-leaf node (when is dict {})
    std::vector<TreeNode*> list;  // for non-leaf node (when is list [])

    // for leaf node
    TreeNode(Token * tok);
    void save_token(Token * tok);
    // for non-leaf node
    TreeNode();
    TreeNode * new_child(std::string key);
    void insert(std::string key, TreeNode* val);
    void add_to_list(TreeNode* node);
    // both
    void free();

    // for jsoni user
    TreeNode operator[](const int & i); // for list
    int size(); // for list
    TreeNode operator[](const std::string & i); // for dict
    bool contains(const std::string & key); // for dict
    Token get(); // for Token
};

TreeNode * parse(const char * source, Token * first_tok = nullptr);
TreeNode * parse_from_file(const char * filename);

}

