#include <iostream>
using namespace std;

void lock(){
    string passwd;
    string attempt;
    cout << "enter password: ";
    getline(cin,passwd);
    system("clear");
    while(1){
        cout << "Enter password to unlock: ";
        getline(cin,attempt);
        if(attempt == passwd){
            break;
        }
        cout << "Incorrect password" << endl;
    }
}
