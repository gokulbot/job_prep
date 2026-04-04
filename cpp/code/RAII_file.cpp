
#include <cstdio>
#include <iostream>
#include <memory>
class RAIIfile {
   private:
    FILE* file_;

   public:
    RAIIfile(const std::string& path) {
        file_ = fopen(path.c_str(), "r");
        if (!file_) throw std::runtime_error("Failed to open file: " + path);
    }

    ~RAIIfile() {
        if (file_ != nullptr) {
            fclose(file_);
        }
    }
    RAIIfile(const RAIIfile&) = delete;
    RAIIfile& operator=(const RAIIfile&) = delete;
};