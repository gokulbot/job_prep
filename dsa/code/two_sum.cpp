#include <iostream>
#include <unordered_map>
#include <vector>

std::pair<int, int> twoSumIndices(std::vector<int>& nums, int target) {
    std::unordered_map<int, int> m;
    std::pair<int, int> result;

    for (int i = 0; i < nums.size(); i++) {
        if (m.find(nums[i]) == m.end()) {
            m[target - nums[i]] = i;
        } else {
            result.first = i;
            result.second = m[nums[i]];
            return result;
        }
    }
    return {-1, -1};
}

int main() {
    std::vector<int> nums = {2, 7, 11, 15};
    int target = 18;
    std::pair<int, int> result = twoSumIndices(nums, target);

    std::cout << "Indices: " << result.first << " " << result.second << "\n";
    return 0;
}