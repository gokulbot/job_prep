#include <chrono>
#include <iostream>
class RAIITimer {
   private:
    std::chrono::duration<double> duration_;
    std::chrono::high_resolution_clock::time_point start_;
    std::chrono::high_resolution_clock::time_point end_;

   public:
    RAIITimer() { start_ = std::chrono::high_resolution_clock::now(); }

    ~RAIITimer() {
        end_ = std::chrono::high_resolution_clock::now();
        duration_ = end_ - start_;
        std::cout << "Elapsed: " << duration_.count() << " s\n";
    }
    RAIITimer(const RAIITimer&) = delete;
    RAIITimer& operator=(const RAIITimer&) = delete;
};