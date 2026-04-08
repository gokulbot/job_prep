#include <iostream>
#include <vector>

bool search2d(std::vector<std::vector<int>>& nums, int target) {
    int row_size = nums.size();
    int col_size = nums[0].size();

    // first col binary search

    int start_row = 0;
    int end_row = row_size - 1;

    while (start_row <= end_row) {
        int mid = start_row + (end_row - start_row) / 2;

        if (nums[mid][0] == target) {
            return true;
        } else if (nums[mid][0] < target) {
            start_row = mid + 1;
        } else {
            end_row = mid - 1;
        }
    }
    // row binary search

    if (end_row < 0) return false;  // target smaller than smallest element

    int start_col = 0;
    int end_col = col_size - 1;

    while (start_col <= end_col) {
        int mid = start_col + (end_col - start_col) / 2;

        if (nums[end_row][mid] == target) {
            return true;
        } else if (nums[end_row][mid] < target) {
            start_col = mid + 1;
        } else {
            end_col = mid - 1;
        }
    }

    return false;
}

int main() {
    int target = 13;
    std::vector<std::vector<int>> m = {
        {1, 3, 5, 7}, {10, 11, 16, 20}, {23, 30, 34, 60}};

    std::cout << search2d(m, target);
    return 0;
}