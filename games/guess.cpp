#include <iostream>
#include <random>



using namespace std;

void game_guess(){
    cout << "Welcome to the guessing game!" << endl;
    cout << "In this game you have to guess between 0 and 100!" << "!" << endl;
    int guess;
    double test;
    int rnum = random() % 100;
    while(1){
        try{
            cin >> guess;
            //guess == INT32_MAX
            if(guess > 100 || guess < 0){
                cout << "No way!" << endl;
                continue;
            }
        }catch(exception i){
            cout << "Invalid number! so exiting!" << endl;
            break;
        }

        if(guess == rnum){
            cout << "You win!" << endl;
            break;
        }else if(guess > rnum){
            cout << "Your guess is larger than the actual number!" << endl;
        }else if(guess < rnum){
            cout << "Your guess is smaller than the actual number!" << endl;
        }else{
            return;
        }
        guess = 0;
    }
    cout << "Bye!" << endl;
}
