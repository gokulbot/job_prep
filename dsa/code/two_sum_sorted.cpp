#include <iostream>
#include <unordered_map>
#include <vector>

std::pair<int, int> twoSumIndices(std::vector<int>& nums, int target) {
    int start = 0;
    int end = nums.size() - 1;

    while (start < end) {
        if ((nums[start] + nums[end]) == target) {
            return {start + 1, end + 1};
        }

        else if ((nums[start] + nums[end]) > target) {
            end--;
        } else {
            start++;
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