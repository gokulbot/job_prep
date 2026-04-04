#include <deque>
#include <iostream>
#include <vector>

std::vector<int> slidingWindowMax(std::vector<int>& nums, int window) {
    std::deque<int> max_window;
    std::vector<int> max_array;

    for (int i = 0; i < nums.size(); i++) {
        while (!max_window.empty() && nums[max_window.back()] < nums[i]) {
            max_window.pop_back();
        }

        max_window.emplace_back(i);

        if (max_window.front() <= (i - window)) {
            max_window.pop_front();
        }

        if (i >= window - 1) {
            max_array.emplace_back(nums[max_window.front()]);
        }
    }

    return max_array;
}

int main() {
    std::vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
    int window = 3;
    std::vector<int> max_array = slidingWindowMax(nums, window);

    for (int x : max_array) {
        std::cout << x << " ";
    }

    return 0;
}