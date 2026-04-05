#include <iostream>
#include <chrono>

int main() {
    auto start_stack = std::chrono::high_resolution_clock::now();
    int arr_stack[1000000];
    auto end_stack = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration_stack =  end_stack - start_stack ;

    std::cout << "Time taken stack: " << duration_stack.count() << " seconds\n";

    auto start_heap = std::chrono::high_resolution_clock::now();
    int* arr_heap = new int[1000000];
    auto end_heap = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration_heap = end_heap - start_heap;

    std::cout << "Time taken heap: " << duration_heap.count() << " seconds\n";

    delete[] arr_heap;


    return 0;
}