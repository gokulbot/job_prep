#include <iostream>
#include <vector>

bool jumpGame(std::vector<int> nums) {
    int max = 0;

    for (int i = 0; i < nums.size(); i++) {
        if (i > max) {
            return false;
        } else {
            max = std::max(max, i + nums[i]);
        }
    }
    return true;
}

int main() {
    std::vector<int> nums = {3, 2, 1, 0, 4};

    std::cout << jumpGame(nums) << "\n";
}