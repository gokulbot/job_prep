#include <deque>
#include <iostream>
#include <string>
#include <unordered_map>

int longestSubStringWihtoutRepeating(std::string source_str) {
    int max_len = 0;

    int left = 0;

    std::unordered_map<char, int> last_seen;

    for (int right = 0; right < source_str.size(); right++) {
        if (last_seen.find(source_str[right]) != last_seen.end() &&
            last_seen[source_str[right]] >= left) {
            left = last_seen[source_str[right]] + 1;
        }

        last_seen[source_str[right]] = right;
        max_len = std::max(max_len, right - left + 1);
    }

    return max_len;
}

int main() {
    std::string source = "pwwkew";
    int result = longestSubStringWihtoutRepeating(source);

    std::cout << result;
    return 0;
}
