#include <algorithm>
#include <iostream>
#include <vector>
std::vector<std::vector<int>> threeSum(std::vector<int>& nums) {
    std::vector<std::vector<int>> result;
    std::sort(nums.begin(), nums.end());
    for (int i = 0; i < nums.size(); i++) {
        if (i > 0) {
            if (nums[i] == nums[i - 1]) {
                continue;
            }
        }
        int left = i + 1;
        int right = nums.size() - 1;

        while (left < right) {
            if (nums[left] + nums[right] == -nums[i]) {
                result.push_back({nums[left], nums[right], nums[i]});
                left++;
                right--;
                while (left < right && nums[left] == nums[left - 1]) left++;
                while (left < right && nums[right] == nums[right + 1]) right--;
            } else if (nums[left] + nums[right] > -nums[i]) {
                right--;
            } else {
                left++;
            }
        }
    }
    return result;
}

int main() {
    std::vector<int> nums = {-1, 0, 1, 2, -1, -4};
    std::vector<std::vector<int>> result = threeSum(nums);

    for (auto e : result) {
        for (int i = 0; i < e.size(); i++) {
            std::cout << e[i] << " ";
        }
        std::cout << "\n";
    }
}