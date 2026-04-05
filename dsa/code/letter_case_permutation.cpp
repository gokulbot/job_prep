#include<iostream>
#include<string>
#include<cctype>
#include<vector>

using namespace std;

void permutation(int i, string& s, vector<string>& result, string temp){

    if(temp.size() == s.size()){
        result.emplace_back(temp);
        return;
    }

    if(isalpha(s[i])){
        permutation(i+1,s,result,temp + (char)toupper(s[i]));
        permutation(i+1,s,result,temp + (char)tolower(s[i]));
    }
    else{
        permutation(i+1,s,result,temp + s[i]);
    }
}

vector<string> letterCasePermutation(string s) {

    vector<string> result;
    std::string tmp = "";
    permutation(0,s,result,tmp );

    return result;
        
}



int main(){

    string s = "a1b2";

    vector<string> result = letterCasePermutation( s);

    for( std::string e : result){
        std::cout<<e<<'\n';
    }
}