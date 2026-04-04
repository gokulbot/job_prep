

#include <iostream>
#include <mutex>
class RAIIMutex {
   private:
    std::mutex& mtx_;

   public:
    RAIIMutex(std::mutex& mtx) : mtx_(mtx) { mtx_.lock(); }

    ~RAIIMutex() { mtx_.unlock(); }
    RAIIMutex(const RAIIMutex&) = delete;
    RAIIMutex& operator=(const RAIIMutex&) = delete;
};