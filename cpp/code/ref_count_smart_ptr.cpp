#include <iostream>
template <typename T>

class shr_ptr {
   public:
    int* ref_count = nullptr;
    T* t_;

   private:
    shr_ptr(T val) {
        t_ = new T(val);
        ref_count = new int(1);
    }

    shr_ptr(const shr_ptr& other) {
        ref_count = other.ref_count;
        (*ref_count)++;
        t_ = other.t_;
    }

    shr_ptr& operator=(const shr_ptr& other) {
        if (&other == this) {
            return *this;
        }

        (*ref_count)--;

        if ((*ref_count) == 0) {
            delete t_;
            delete ref_count;
        }

        ref_count = other.ref_count;
        (*ref_count)++;

        t_ = other.t_;
        return *this;
    }

    T& operator*() {
        if (t_ != nullptr) {
            return *t_;
        }
    }

    T* operator->() {
        if (t_ != nullptr) {
            return t_;
        }
    }

    ~shr_ptr() {
        (*ref_count)--;
        if ((*ref_count) == 0) {
            delete t_;
            delete ref_count;
        }
    }
};
int main() {}