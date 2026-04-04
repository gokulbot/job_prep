#include <iostream>
#include <string>
#include <unordered_map>

class Solution {
   public:
    bool isAnagram(std::string s, std::string t) {
        std::unordered_map<char, int> m;
        if (s.size() != t.size()) return false;

        for (int i = 0; i < s.size(); i++) {
            m[s[i]] += 1;
        }

        for (int i = 0; i < t.size(); i++) {
            if (m.find(t[i]) == m.end()) {
                return false;
            } else if (m[t[i]] <= 0) {
                return false;
            } else {
                m[t[i]]--;
            }
        }
        return true;
    }
};