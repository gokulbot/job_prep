#include <algorithm>
#include <iostream>
#include <vector>
int threeSum(std::vector<int>& nums, int target) {
    int min_sum = INT32_MAX;
    int closest_sum = 0;
    std::sort(nums.begin(), nums.end());
    for (int i = 0; i < nums.size(); i++) {
        int left = i + 1;
        int right = nums.size() - 1;

        while (left < right) {
            int sum = nums[left] + nums[right] + nums[i];

            if (abs(sum - target) < min_sum) {
                min_sum = abs(sum - target);
                closest_sum = sum;
                left++;

            } else if ((nums[left] + nums[right] + nums[i]) > target) {
                right--;
            } else {
                left++;
            }
        }
    }
    return closest_sum;
}

int main() {
    std::vector<int> nums = {-1, 2, 1, -4};
    int target = 1;
    int result = threeSum(nums, target);

    std::cout << result << "\n";
}