

class Test{

    public:

        Test() = default;

        Test(const Test& other){

            if(other.x_!=nullptr){

                x_ = new int;
                *x_ = *(other.x_);
            }
            else{
                x_=nullptr;
            }
        }

        Test& operator=(const Test& other){

            if(this == &other) return *this;

            if(other.x_!=nullptr){
                delete x_;
                x_ = new int;
                *x_ = *(other.x_);
            }
            else{
                delete x_;
                x_ = nullptr;
            }

            return *this;
        }

        ~Test(){
            delete x_;
        }



    private:
        int* x_ = nullptr;
};