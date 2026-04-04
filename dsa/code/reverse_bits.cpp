#include <iostream>

int main() {
    int n = 43261596;
    int result = 0;

    for (int i = 1; i <= 32; i++) {
        std::cout << "result before : " << result << "\n";

        result = (result << 1) |
                 (n & 1);  // left shift by one add the n last digit there

        std::cout << "result after : " << result << "\n";
        n >>= 1;  // right shift by 1 divide by 2 getting newer lst digits
    }

    return result;
}