#include<iostream>
#include<memory>


void fixed_allocate(){

    int* x = new int;

    delete x; 
}
void leaky_allocate(){

    int* x = new int;
}

int main(){
   
    
}