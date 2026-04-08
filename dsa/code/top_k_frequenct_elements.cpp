#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>

struct Compare {
    bool operator()(const std::pair<int, int>& a,
                    const std::pair<int, int>& b) {
        return a.second > b.second;
    }
};
std::vector<int> topKfrequent(std::vector<int>& nums, int k) {
    std::unordered_map<int, int> freq;
    std::vector<int> result;
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
                        Compare>
        pq;

    std::vector<std::vector<int>> bucket(nums.size() + 1);

    for (int e : nums) {
        freq[e]++;
    }

    // for (auto it = freq.begin(); it != freq.end(); it++) {
    //     pq.emplace(std::make_pair(it->first, it->second));

    //     if (pq.size() > k) {
    //         pq.pop();
    //     }
    // }

    // while (!pq.empty()) {
    //     int top = pq.top().first;
    //     result.emplace_back(top);
    //     pq.pop();
    // }
    // return result;

    for (auto& [num, count] : freq) {
        bucket[count].emplace_back(num);
    }

    for (int i = nums.size(); i >= 0; i--) {
        for (int j = 0; j < bucket[i].size(); j++) {
            result.push_back(bucket[i][j]);
        }

        if (result.size() == k) {
            break;
        }
    }
    return result;
}
int main() {
    std::vector<int> nums = {1, 1, 1, 2, 2, 3};
    int k = 2;
    std::vector<int> result = topKfrequent(nums, k);

    for (int e : result) {
        std::cout << e << " ";
    }

    return 0;
}