#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <unordered_map>
#include <cstdio>
#include <cstring>

namespace jsoni {

int idx = 0;
// Utils
void inline Error(const char * info) {
    throw info;
}

#define IS_LETTER_OR_NUMBER(var) ( \
        ((var)>='A'&&(var)<= 'Z') \
        ||((var)>='a'&&(var)<='z') \
        ||((var)>='0'&&(var)<= '9') \
)

// Lexer
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
Token get_token(const char * source) {
    Token result;
    char two_chars_string[2] = {0, 0};
    // skip space
    while (source[idx] == ' ' || source[idx] == '\n' || source[idx] == '\t') {
        ++ idx;
    }
    // end
    if (source[idx] == '\0') {
        result.type = TokenTypeEnd;
        return result;
    }
    // signs
    switch (source[idx]) {
        case '{':
        case '}':
        case '(':
        case ')':
        case ',':
        case '[':
        case ']':
        case ':':
          result.type = TokenTypeSign;
          two_chars_string[0] = source[idx];
          result.content = two_chars_string;
          ++ idx;
          return result;
    }
    // string
    if (source[idx] == '\"' || source[idx] == '\'') {
        char quote = source[idx]; // " or '
        char ch;
        result.type = TokenTypeString;
        ++ idx;
        while (source[idx] != quote) {
            if (source[idx] == '\0' || source[idx] == '\n') Error("String token is not enough.");
            if (source[idx] == '\\') {
                ++ idx;
                switch (source[idx]) {
                    case 'n': ch = '\n'; break;
                    case 't': ch = '\t'; break;
                    case '\"': ch = '\"'; break;
                    case '\'': ch = '\''; break;
                    case '\\': ch = '\\'; break;
                    default: Error("Wrong using `\\`.");
                }
            } else {
                ch = source[idx];
            }
            result.content += ch;
            ++ idx;
        }
        ++ idx;
        return result;
    }
    // int and float
    if (source[idx] >= '0' && source[idx] <= '9') {
        result.type = TokenTypeInt;
        do {
            result.content += source[idx];
            ++ idx;
        } while (source[idx] >= '0' && source[idx] <= '9');
        if (source[idx] != '.') return result;
        result.type = TokenTypeFloat;
        result.content += '.';
        ++ idx;
        while (source[idx] >= '0' && source[idx] <= '9') {
            result.content += source[idx];
            ++ idx;
        }
        return result;
    }
    // bool
    // i know it's ugly, but it's a safe and efficient way :(
    if (source[idx] == 't') 
     if (source[idx+1] == 'r')
      if (source[idx+2] == 'u')
       if (source[idx+3] == 'e')
        if (!IS_LETTER_OR_NUMBER(source[idx+4])) {
            result.type = TokenTypeBool;
            result.content = "true";
            idx += 4;
            return result;
    }
    if (source[idx] == 'f') 
     if (source[idx+1] == 'a')
      if (source[idx+2] == 'l')
       if (source[idx+3] == 's')
        if (source[idx+4] == 'e')
         if (!IS_LETTER_OR_NUMBER(source[idx+5])) {
            result.type = TokenTypeBool;
            result.content = "false";
            idx += 5;
            return result;
    }
    Error("Something wrong!");
    return result;
}
// Tree
// TreeNode could be a leaf node or a non-leaf node.
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
TreeNode::TreeNode() {
    this->isLeaf = false;
}
TreeNode::TreeNode(Token * tok) {
    this->save_token(tok);
}
void TreeNode::save_token(Token * tok) {
    Token * new_place = (Token*)malloc(sizeof(Token));
    new_place->type = tok->type;
    new_place->content = tok->content;
    this->isLeaf = true;
    this->leaf = new_place;
}
void TreeNode::free() {
    if (this->isLeaf) {
        if (this->leaf != nullptr) {
             std::free(this->leaf);
        }
        return ;
    }
    for (std::pair<std::string, TreeNode*> child : this->dict) {
        (child.second)->free();
        delete child.second;
    }
    this->dict.clear();
    for (TreeNode * child : this->list) {
        child->free();
        delete child;
    }
    this->list.clear();
}
TreeNode * TreeNode::new_child(std::string key) {
    TreeNode * child = new TreeNode();
    this->dict.insert({key, child});
    return child;
}
void TreeNode::insert(std::string key, TreeNode* val) {
    this->dict.insert({key, val});
}
void TreeNode::add_to_list(TreeNode* node) {
    this->list.push_back(node);
}
TreeNode TreeNode::operator[](const int & i) {
    if (i >= this->list.size()) return TreeNode();
    return *(this->list[i]);
}
int TreeNode::size() {
    return this->list.size();
}
TreeNode TreeNode::operator[](const std::string & i) {
    auto pr = this->dict.find(i);
    if (pr == this->dict.end()) return TreeNode();
    return *(pr->second);
}
bool TreeNode::contains(const std::string & key) {
    if (this->list.size() == 0) return this->dict.find(key) != this->dict.end();
    for (TreeNode* child : this->list) {
        if (child->isLeaf) {
            if (child->leaf->content == key) return true;
        }
    }
    return false;
}
Token TreeNode::get() {
    if (!this->isLeaf) return Token();
    return *leaf;
}
// Parser
/*
 * parse -> anything
 * dict -> ({) .... }
 * list
 */

TreeNode * parse_dict(const char * source);
TreeNode * parse_list(const char * source);
TreeNode * parse(const char * source, Token * first_tok = nullptr);

TreeNode * parse_dict(const char * source) {
    TreeNode * result = new TreeNode();
    std::string key_name;
    Token tok = get_token(source);
    for (;;) {
        if (tok.content == "}") {
            break;
        }
        if (tok.type == TokenTypeEnd) {
            Error("Should be some content!");
        }
        if (tok.type != TokenTypeString) {
            Error("Should be a string.");
        }
        key_name = tok.content;
        tok = get_token(source);
        if (tok.content != ":") {
            Error("Shoud be : here.");
        }
        result->insert(key_name, parse(source));

        tok = get_token(source);
        if (tok.content == ",") tok = get_token(source);
    }
    return result;
}
TreeNode * parse_list(const char * source) {
    TreeNode * result = new TreeNode();
    Token tok = get_token(source);
    for (;;) {
        if (tok.content == "]") {
            break;
        }
        if (tok.type == TokenTypeEnd) {
            Error("Should be some content!");
        }

        result->add_to_list(parse(source, &tok));

        tok = get_token(source);
        if (tok.content == ",") tok = get_token(source);
    }
    return result;
}
TreeNode * parse(const char * source, Token * first_tok) {
    Token tok;
    if (first_tok == nullptr) tok = get_token(source);
    else tok = *first_tok;

    if (tok.type == TokenTypeEnd) {
        Error("No data.");
    }
    if (tok.content == "{") {
        return parse_dict(source);
    }
    if (tok.content == "[") {
        return parse_list(source);
    }
    if (tok.type == TokenTypeInt || 
        tok.type == TokenTypeFloat ||
        tok.type == TokenTypeString ||
        tok.type == TokenTypeBool) {
        return new TreeNode(&tok);
    }
    Error("Something wrong");
    return nullptr; // never run here
}

TreeNode * parse_from_file(const char * filename) {
    // C grammar here
    int file_size;
    char * json_data;
    TreeNode * result;
    std::FILE * fp = fopen(filename, "r");
    if (fp == NULL) Error("Cannot open file");
    std::fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    std::fseek(fp, 0, SEEK_SET);
    json_data = (char*)std::malloc(file_size+1);
    fread(json_data, sizeof(char), file_size, fp);
    json_data[file_size] = '\0';
    result = parse(json_data);
    std::free(json_data);
    std::fclose(fp);
    return result;
}

} // for namespace

