#include<iostream>


template<typename T>

std::unique_ptr<T> make_unique(T value){

    return std::unique_ptr<T>(new T(value));
}

int main(){


}