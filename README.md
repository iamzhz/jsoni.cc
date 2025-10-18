# jsoni – a pocket-sized JSON library

## What it is
Single-file JSON parser that turns text into a tiny tree you can walk with `[]` and `.get()`.   
(Another wheel)

## Quick start
```cpp
#include "jsoni.h"

int main() {
    // 2. parse
    jsoni::TreeNode * root = jsoni::parse(
        R"({
        "name": "iamzhz",
        "age": 16,
        "hobbies": {
            "code": ["C", "C++", "Python"],
            "and...": "learning"
        }
        })"
    );

    // 3. read
    std::cout << (*root)["name"].get().content << std::endl; // iamzhz
    std::cout << (*root)["age"].get().content << std::endl;  // 16
    std::cout << (*root)["hobbies"]["code"][1].get().content << std::endl;  // C++
    std::cout << (*root)["hobbies"]["and..."].get().content << std::endl;  // learning
    std::cout << (*root)["hobbies"].contains("code") << std::endl; // 1
    std::cout << (*root)["hobbies"]["code"].contains("Java") << std::endl; // 0
    std::cout << (*root)["hobbies"]["code"].size() << std::endl; // 3

    // 6. clean-up
    root->free();
    delete root;
}
```

## Build
*TIP:* `C++11` is enough.  
```bash
g++ your_code.cc jsoni.cc -std=c++11 -o program
```
## Limits
ASCII only, no `\uXXXX` unescape, numbers stored as strings → convert with `std::stoi/stod` when needed.

## License
MIT
