#include <iostream>
class Mapmanger{

    private:
        Mapmanger() =default;


    public:

        static Mapmanger& getInstance();
        Mapmanger& operator=(const Mapmanger&) = delete;
        Mapmanger(const Mapmanger&) = delete;
};

 Mapmanger& Mapmanger::getInstance(){
    static Mapmanger instance;

    return instance;

}

  int main() {                                                                                                                                                                                                     
      Mapmanger& m1 = Mapmanger::getInstance();                                                                                                                                                                    
      Mapmanger& m2 = Mapmanger::getInstance();                                                                                                                                                                    
      std::cout << (&m1 == &m2) << "\n";   // prints 1 — same instance
  }                                                                                                                                                                                                                
    