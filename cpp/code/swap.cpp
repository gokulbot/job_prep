#include <iostream>
class Buffer {
   private:
    int* data_ = nullptr;
    int size_;

   public:
    // default constructor
    // why is initializer list better
    Buffer(int size) : size_(size) {
        std::cout << "default constructor called" << '\n';
        data_ = new int[size_];
    }

    // copy constructor
    Buffer(const Buffer& other) {
        std::cout << "copy constructor called" << '\n';

        size_ = other.size_;
        data_ = new int[other.size_];

        for (int i = 0; i < other.size_; i++) {
            data_[i] = other.data_[i];
        }
    }

    // move constructor
    Buffer(Buffer&& other) {
        std::cout << "move constructor called" << '\n';
        size_ = 0;

        data_ = other.data_;
        size_ = other.size_;

        other.data_ = nullptr;
        other.size_ = 0;
    }

    // copy assingment
    const Buffer& operator=(const Buffer& other) {
        if (this == &other) {
            return *this;
        }

        std::cout << "copy assignment constructor called" << '\n';

        delete[] data_;
        size_ = 0;

        size_ = other.size_;
        data_ = new int[other.size_];

        for (int i = 0; i < other.size_; i++) {
            data_[i] = other.data_[i];
        }

        return *this;
    }

    const Buffer& operator=(Buffer&& other) {
        if (this == &other) {
            return *this;
        }

        std::cout << "move assignment constructor called" << '\n';

        delete[] data_;
        size_ = 0;

        data_ = other.data_;
        size_ = other.size_;

        other.size_ = 0;
        other.data_ = nullptr;  // how does this even know how much to delete

        return *this;
    }

    ~Buffer() { delete[] data_; }
};

void swap(Buffer& a, Buffer& b) {
    Buffer temp = std::move(a);
    a = std::move(b);
    b = std::move(temp);
}

int main() {
    Buffer a(5);
    Buffer b(10);
    swap(a, b);
}