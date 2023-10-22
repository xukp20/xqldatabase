#ifndef STRING_COMPARE_H
#define STRING_COMPARE_H

#include <string>
#include <vector>
using namespace std;

class StringCompare {
public:
    bool static like(const std::string& str, const std::string& pattern) {
        if (pattern.empty()) return str.empty();
        if (pattern[0] == '%') {
            if (pattern.size() == 1) return true;
            for (int i = 0; i < str.size(); i++) {
                if (like(str.substr(i), pattern.substr(1))) return true;
            }
            return false;
        } else {
            if (str.empty()) return false;
            if (pattern[0] == str[0] || pattern[0] == '_') {
                return like(str.substr(1), pattern.substr(1));
            }
            if (pattern[0] == '[') {        // match char list
                return matchCharList(str, pattern);
            } else {
                return false;
            }
        }
    }

private:
    bool static matchCharList(const std::string& str, const std::string& pattern) {
        if (pattern.size() > 1 && pattern[1] == '^' || pattern[1] == '!') {     // pattern after [, and it's ^ or !
            vector<char> no_char_list;
            int last = 0;
            for (int i = 2; i < pattern.size(); i++) {
                if (pattern[i] == '-') {        // omit char list
                    i = addOmitList(pattern, no_char_list, i);
                    i--;        // to remove i++
                } else if (pattern[i] == ']') {
                    last = i;
                    break;
                } else {
                    no_char_list.push_back(pattern[i]);
                }
            }

            if (last == 0)              // no ] found, treat  [ as normal char
                return str[0] == pattern[0] && like(str.substr(1), pattern.substr(1));
            else {
                for (int i = 0; i < no_char_list.size(); i++) {
                    if (str[0] == no_char_list[i]) return false;
                }
                return like(str.substr(1), pattern.substr(last + 1));
            }
        } else {
            vector<char> char_list;
            int last = 0;
            for (int i = 1; i < pattern.size(); i++) {
                if (pattern[i] == '-') {        // omit char list
                    i = addOmitList(pattern, char_list, i);
                    i--;        // to remove i++
                } else if (pattern[i] == ']') {
                    last = i;
                    break;
                } else {
                    char_list.push_back(pattern[i]);
                }
            }

            if (last == 0)              // no ] found, treat  [ as normal char
                return str[0] == pattern[0] && like(str.substr(1), pattern.substr(1));
            else {
                for (int i = 0; i < char_list.size(); i++) {
                    if (str[0] == char_list[i]) return like(str.substr(1), pattern.substr(last + 1));
                }
                return false;
            }
        }
    }


    // tool
    // match a-b in pattern's [], return pattern after b (or - if no b), add a-b except a into char_list
    // input: pattern = '..-bc...', char_list = [...,a], index = index of '-'
    // output: index of c, char_list = [...,a,...,b];
    int static addOmitList(std::string old_pattern, vector<char>& charlist, int index){
        if(index == old_pattern.size() - 1 || old_pattern[index - 1] == '[' || old_pattern[index + 1] == ']'){     // '-' is the last char or '[-' '-]' , raed '-'
            charlist.push_back(old_pattern[index]);     // add '-'
            return index + 1;
        } else {
            for (char i = old_pattern[index - 1]; i <= old_pattern[index + 1]; i++) {
                charlist.push_back(i);
            }
            return index + 2;
        }
    }
};



#endif