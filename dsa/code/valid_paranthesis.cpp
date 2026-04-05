#include<iostream>
#include<stack>
#include<string>

int main(){

    std::stack<char> s;
    std::string paranthesis = "([)]";

    for( char c : paranthesis){
        if(c == '(' || c == '[' || c== '{'){
            s.push(c);
        }
        else{
            if(s.empty()){
                std::cout<<false;
                return 0;
            }   
            char c1 = s.top();
            if(c1 == '('  && c !=')'){
                std::cout<<false;
                return 0;
            }
            else if(c1 == '{'  && c !='}'){
                std::cout<<false;
                return 0;
            }
            else if(c1 == '['  && c !=']'){
                std::cout<< false;
                return 0;
            }
            else{
                s.pop();
            }
        }
    }
    std::cout<<s.empty();
    return 0;
}
