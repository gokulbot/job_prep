#include<memory>

class Test{

    public:

        Test() = default;

        Test(const Test& other){

            if(other.x_!=nullptr){
                x_ = std::make_unique<int>(*other.x_);
            }
            else{
                x_=nullptr;
            }
        }

        Test& operator=(const Test& other){

            if(this == &other) return *this;

            if(other.x_!=nullptr){
                x_ = std::make_unique<int>(*other.x_);
            }
            else{

                x_ = nullptr;
            }

            return *this;
        }

        ~Test()=default;


    private:
        std::unique_ptr<int> x_;
};