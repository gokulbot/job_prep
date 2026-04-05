#include<iostream>
#include<string>
#include <cctype>

int main(){

    std::string s = "race a car";
    int start =0;
    int end = s.size()-1;

    while(start<=end){
        
        while(start <= end && !isalnum(s[start])){
            start++;
        }

        while(start <= end && !isalnum(s[end])){
            end--;
        }

        if(tolower(s[start]) != tolower(s[end])){
            std::cout<<s[start]<<" "<<s[end]<<"\n";
            std::cout<<false;
            return 0;
        }
        else{
            start++;
            end--;
        }

    }
    std::cout<<true;
    return 0;
}