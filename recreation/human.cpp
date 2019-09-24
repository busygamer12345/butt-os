#ifndef COUNSELLOR_LOADED

#include <iostream>
using namespace std;
bool yes_no(bool takeasyes = true){
    string yesno;
    cout << "(Yes or No)";
    getline(cin,yesno);

    if(yesno == "yes" || yesno == "yeah" || yesno == "YES" || yesno == "yep" || yesno == "Y" || yesno == "y"){
        return true;
    }else if(yesno == "no" || yesno == "NO" || yesno == "N" || yesno == "n"){
        return false;
    }else{
        if(takeasyes){
            cout << "I am taking that as a yes!" << endl;
            return true;
        }
        return false;
    }
}

void say_something();

void behavior_no_binary(){ // locks computer till yes
    cout << "Do you agree with me";
    while(!!!yes_no(false)){
        cout << "I am not going to leave you till you agree with me" << endl;
    }
    cout << "Good, I am glad that you agree with me." << endl;
    return;
}

void behaviour_seniment(){ // make user feel sentimental
    cout << "That happens to me too" << endl;
}

void session_work(){ // selection: work
    cout << "So you have a problem with your work?" << endl;
    cout << "Do you think your work is tedious?";
    if(yes_no()){
        behaviour_seniment();
        cout << "Work is only tedious when you think about it!" << endl;
        cout << "Think about something else" << endl;
        behavior_no_binary();
        return;
    }
    cout << "Is your boss too harsh?" << endl;
    if(yes_no()){
        behaviour_seniment();
        cout << "Learn to cope up with it! Let it go! Don't care about him!" << endl;
        cout << "Live in peace by not thinking about him!" << endl;
        behavior_no_binary();
        return;
    }
    cout << "You don't like your work? yes=no like" << endl;
    if(yes_no()){
        //!!! behaviour_seniment();counsellor: I love my work!!!
        cout << "Learn to love it. Your job is the God's gift." << endl;
        cout << "He must have a happy outcome for you! It is your DESTINY to be in this job!" << endl;
        behavior_no_binary();
        return;
    }
}

void counsellor_program_init(){
    // TODO: Make different choices

    cout << "Hi! I am John! Your counsellor!" << endl; // TODO: Add different names for our dear counsellor!
    string problem;
    cout << "Tell me what is your problem! (work)" << endl;
    getline(cin,problem);
    if(problem == "work"){
        session_work();
    }

    else{
        cout << "I have no clue what you are talking about!" << endl;
    }
    cout << "Bye!!" << endl;
}

#define COUNSELLOR_LOADED true
#endif
