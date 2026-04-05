
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>

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
            std::cout<<"file closed"<<"\n";
        }
    }
    RAIIfile(const RAIIfile&) = delete;
    RAIIfile& operator=(const RAIIfile&) = delete;
};

void dowork(){
    RAIIfile f("test.txt");
    throw std::runtime_error("Life is backwards!!");

}

int main(){
    try{
        dowork();
    } catch (const std::exception& e ){
        std::cout<<"Caught: "<<e.what()<<"\n";
    }
}