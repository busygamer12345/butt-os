#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <experimental/filesystem>
#include <fstream>
#include <dirent.h>
#include <ncurses.h>
#include <signal.h>

#include "modules/mainmod.h"

#include "games/guess.cpp"
#include "recreation/human.cpp"
#include "utility/lock.cpp"
#include "utility/eject.cpp"
#define DEBUG 0
//#define SANDBOX_PATH "/home/user/butt/sandbox"
#define PREV_DIR ".."


namespace fs = std::experimental::filesystem;
using namespace std;

//WINDOW* win = initscr();??/
const string BOOTL_VERSION= "0.04";
const string OS_VERSION = "0.25";
const string AUTOEXEC_PATH = "autoexec-batch";

string SANDBOX_PATH = "/home/user/butt/sandbox";
const vector<string> progs = {"echo","help",
"expr","read","ed","mkdir","rmdir","whatis","rm","cwd","cd","guessgame","counsellor","progs","lock","eject","lstree","jump","batch"};

string _path = "/";
string command;
string cmd;
string c;

void batch_processor(string path,bool warn);

bool safe_mode = false;
vector<string> argo;
    int current_instruction;
    bool should_increment;


vector<string> split(const string& str, const string& delim)
{
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

void println(string str){
    cout << str << endl;
};

void exit(){
    _Exit(0);
}

void error(string e){
    cout << "\x1B[91m" << e << "\033[0m" << endl;
}

string pathify(string orig){
    string toc = orig;
    toc = toc.erase(0,SANDBOX_PATH.size()+_path.size());
    return toc;
}

string slashify(string to_test){
    string fixed = to_test;
    if(fixed[fixed.size()-1] != '/'){
        fixed+="/";
    }
    return fixed;
}


bool directory_exists(const char *dname){
   DIR *di=opendir(dname); //open the directory
   bool dodo;
   if(di) dodo =  true; //can open=>return true
   else dodo =  false;//otherwise return false
   closedir(di);
   return dodo;
}

string fix_path(string path){
    string fixed_path = path;

    if(fixed_path.find(".") != string::npos|| fixed_path.find("//") != string::npos|| fixed_path.find("\\") != string::npos|| fixed_path.find("~") != string::npos){
        error("Forbidden path");
        return "forbidden/path/found";
    }
    if(fixed_path.begin().base() == (char*) '/'){
        fixed_path.erase(0,1);
    }


    return fixed_path;
}

void interrupt(int _){
    println("Shutting Down");
    _Exit(999);
}

void sandbox_watchdog(){
    if(safe_mode) {return;};
    if(SANDBOX_PATH == ""){return;}
    if(!directory_exists(SANDBOX_PATH.data())){
        error("----------------------------");
        error("|UNRECOVERABLE EXCEPTION!  |");
        error("|SANDBOX DESTROYED!        |");
        error("|CODE: 0x0 SYSFILE MISSING!|");
        error("|ALL DATA WILL BE RESET!   |");
        error("----------------------------");
        char ___[] = "mkdir -p ";
        string _ = SANDBOX_PATH;
        system(("mkdir -p "+_).data());
        exit();
    }
}

void change_dir(string dir){
    if(safe_mode){ error("Filesystem not available in safe mode"); return;}
    string good_dir = _path+fix_path(dir);
    if(!directory_exists((SANDBOX_PATH+good_dir).data())){
        error("Cannot find directory");
        return;
    }
    _path = slashify(good_dir);
}

void go_back(){
    if(safe_mode) {error("Filesystem not available in safe mode"); return;}
    vector<string> diffpaths = split(_path,"/");
    if(diffpaths.size() == 0){
        _path = "/";
        return;
    }

    diffpaths.pop_back();
    string repath = "/";
    for(string str : diffpaths){
        repath+=(str+"/");
    }
    _path = repath;
}

bool assert_args(string command,int no_exp,int got){
    if(no_exp != got){
        error("Invalid no.of arguments for command "+command);
        //goto loop;
        return true;
    }
    return false;
}

void read_file(string fpath){
    if(safe_mode){ error("Filesystem not available in safe mode"); return;}
    string fullpath;
    fullpath = SANDBOX_PATH+_path+fix_path(fpath);
    string line;
    ifstream myfile (fullpath);
    if (myfile.is_open())
    {
        println("[begin file]");
        while ( getline (myfile,line) )
        {
            cout << line << '\n';
        }
        myfile.close();
        println("[end file]");
    }

  else {error("File does not exist");}
}

void list_dir(){
    if(safe_mode) {error("Filesystem not available in safe mode"); return;}
    vector<string> paths;
    string lpath = SANDBOX_PATH+_path;
    if(!directory_exists(lpath.data())){
        string ans;
        while(1){
            cout << "\x1B[91m" << "Error in reading '" <<_path <<"'[(R)etry, (A)bort, (I)gnore]" << "\033[0m";
            getline(cin,ans);
            if(ans == "I"){
                return;
            }else if(ans == "R"){
                list_dir();
                return;
            }else if (ans == "A"){
                _path = "/";
                return;
            }
        }
    }
    for (const auto & entry : fs::directory_iterator(lpath)){
        paths.push_back((string) entry.path());
    }
    string display_path = _path;
    if(display_path.at(display_path.size()-1) == '/' && display_path.size() != 1){
        display_path.erase(display_path.size()-1,1);
    }
    println("Content of "+display_path+":");
    int i;
    for(i = 0; i < paths.size(); i++){
        if(directory_exists(paths[i].data())){
            paths[i].append("/");
        }
        cout << pathify(paths[i]) << "  ";
    }
    cout << endl;
}

void list_dir_tree(){
    if(safe_mode) {error("Filesystem not available in safe mode"); return;}
    vector<string> paths;
    string tmpo;
    string lpath = SANDBOX_PATH+_path;
    system(("tree -C "+lpath+" >tmp").data());
    ifstream file;
    file.open("tmp", ios::out);
    int i = 0;
    while(getline(file,tmpo)){
        if(!(i==0)){
            println(tmpo);
        }else{
            //println(pathify(tmpo));
        }
        i++;
    }
    cout << endl;
}

void get_cwd(){
    if(safe_mode) error("Filesystem not available in safe mode"); return;
    println("cwd: "+_path);
}


void make_dir(string name){
    if(safe_mode){ error("Filesystem not available in safe mode"); return;}
    string ___ = "mkdir ";
    int res = system((___+SANDBOX_PATH+fix_path(_path)+name+" 2> /dev/null").data());
    if(res != 0){
        error("Directory creation failed");
    }
}

void remove(string name){
    if(safe_mode) {error("Filesystem not available in safe mode"); return;}
    string ___ = "rm -rf ";
    int res = system((___+SANDBOX_PATH+fix_path(_path)+name+" 2> /dev/null").data());
    if(res != 0){
        error("Deletion failed");
    }
}

void move_file(string src, string dst){
    if(safe_mode) {error("Filesystem not available in safe mode"); return;}
    string fullsrc = SANDBOX_PATH+_path+fix_path(src);
    string fulldst = SANDBOX_PATH+_path+fix_path(dst);

    if(!!system(("mv "+fullsrc+" "+fulldst).data())){
        error("File move failed");
    }
}

void copy_file(string src, string dst){
    if(safe_mode) {error("Filesystem not available in safe mode"); return;}
    string fullsrc = SANDBOX_PATH+_path+fix_path(src);
    string fulldst = SANDBOX_PATH+_path+fix_path(dst);

    if(!!system(("cp -r "+fullsrc+" "+fulldst).data())){
        error("File copy failed");
    }
}




void whatis(string path){
    if(safe_mode) {error("Filesystem not available in safe mode"); return;}
    string fullpath;
    fullpath = SANDBOX_PATH+_path+fix_path(path);
    if(directory_exists(fullpath.data())){
        println("Directory");
    }else{
        ifstream file;
        file.open(fullpath, ios::out);
        if(file.is_open()){
            println("File");
            file.close();
        }else{
            println("Non-Existant");
        }
    }
}

void jump(string a1){
    int lnum;
    try{
        lnum = stoi(a1);
    }catch(exception e){
        error("Error: Specified argument is not a number");
        return;
    }

    current_instruction = lnum;
    should_increment = false;
}



void expr_multi(string arg1,string op,string arg2){
    int result;
    int num1;
    int num2;
    try{
        num1 = (double) stoi(arg1);
        num2 = (double) stoi(arg2);
    }catch(exception r){
        error("Arguments to expr should be a valid integer");
        return;
    }

    if(op == "+"){
        result = num1+num2;
    }
     else   if(op == "-"){
        result = num1-num2;
    }
     else   if(op == "*"){
        result = num1*num2;
    }
     else   if(op == "/"){
        if(safe_mode){ error("Division not available in safe mode"); return;}
        if(num2 == 0){
            error("Division by zero forbidden");
            return;
        }
        result = num1/num2;
    }
     else   if(op == "%"){
        result = num1%num2;
    }
     else   if(op == "^"){
         if(safe_mode) error("Bitwise operators are not available in safe mode"); return;
         result = num1^num2;
     }else{
         error("Invalid operator");
         return;
     }
     string h;
     h = to_string(result);
     println(h);
}

void edit_prog(string filepath){
    if(safe_mode) {error("Filesystem not available in safe mode"); return;}
    string path = SANDBOX_PATH+_path+fix_path(filepath);
    fstream file;
    file.open(path, ios::out | ios::in );
    string command_or_string;
    while(1){
        cout << "ed>";
        //cin >> command_or_string;
        getline(cin,command_or_string);
        //cin.ignore();

        if(command_or_string == ":!DOC"){
            read_file(filepath);
        }else if(command_or_string == ":!DELALL"){
            file.close();
            file.open(path, ios::trunc | ios::in);
            file << "";
            file.close();
            file.open(path, ios::in | ios::app);
        }else if(command_or_string == ":!QQQ"){
            file.close();
            break;
        }else if(command_or_string == ":!DELUP"){
            vector<string> lines;
            string line;
            string reass="";
            file.close();
            file.open(path, ios::out);
            while ( getline (file,line) )
            {
                lines.push_back(line);
            }
            if(lines.size() == 0){
                error("Reached all the way up");
                file.close();
                file.open(path, ios::in | ios::app);
                continue;
            }
            //println("Erased 1 line");
            lines.pop_back();
            int counter;
            for(counter = 0; counter < lines.size(); counter++){
                reass+=("\n"+lines[counter]);
            }
            reass.erase(0,1);
            file.close();
            file.open(path, ios::trunc | ios::in);
            file << reass;
            file.close();
            file.open(path, ios::in | ios::app);
            continue;
        }else{
            file.close();
            file.open(path,ios::in | ios::app);
            file << command_or_string << endl;
            file.close();
            file.open(path,ios::in | ios::app);
        }
    }
}


void subparser(string cmdll){
    argo = split(cmdll," ");
    if(argo.size() == 0){
        return;
    }
    //cmd = argo.front;
    if(argo[0] == "echo"){
        if(assert_args("echo",2,argo.size())){
            return;
        }
        println(argo[1]);
    }else if(argo[0] == "exit"){
        exit();
    }else if(argo[0]=="help"){
        println("echo: prints something, exit: quit");
    }else if(argo[0] == "expr"){
        if(assert_args("expr",4,argo.size())){
            return;
        }
        expr_multi(argo[1],argo[2],argo[3]);
    }else if(argo[0] == "ls"){
        list_dir();
    }else if(argo[0] == "cwd"){
        get_cwd();
    }else if(argo[0] == "mkdir"){
        if(assert_args("mkdir",2,argo.size())){
            return;
        }
        make_dir(argo[1]);
    }
    else if(argo[0] == "read"){
        if(assert_args("read",2,argo.size())){
            return;
        }
        read_file(argo[1]);
    }else if(argo[0] == "guessgame"){
        game_guess();
    }else if(argo[0] == "ed"){
        if(assert_args("ed",2,argo.size())){
            return;
        }
        edit_prog(argo[1]);
    }else if(argo[0] == "counsellor"){
        counsellor_program_init();
    }else if(argo[0] == "cd.."){
        go_back();
    }else if(argo[0] == "cd"){
        if(assert_args("cd",2,argo.size())){
            return;
        }
        if(argo[1] == ".."){
            go_back();
            return;
        }
        change_dir(argo[1]);
    }else if(argo[0] == "rm" || argo[0] == "rmdir"){
        if(assert_args("rm",2,argo.size())){
            return;
        }
        remove(argo[1]);
    }else if(argo[0] == "whatis"){
        if(assert_args("whatis",2,argo.size())){
            return;
        }
        whatis(argo[1]);
    }else if(argo[0] == "progs"){
        for(string prog : progs){
            cout << prog << "  ";
        }
        cout << endl;
    }else if(argo[0] == "lock"){
        lock();
    }else if(argo[0] == "eject"){
        eject();
    }else if(argo[0] == "lstree"){
        list_dir_tree();
    }else if(argo[0] == "mv"){
        if(assert_args("mv",3,argo.size())){
            return;
        }
        move_file(argo[1],argo[2]);
    }else if(argo[0] == "cp"){
        if(assert_args("cp",3,argo.size())){
            return;
        }
        copy_file(argo[1],argo[2]);
    }else if(argo[0] == "jump"){
        if(assert_args("jump",2,argo.size())){
            return;
        }
        jump(argo[1]);
    }else if(argo[0] == "batch"){
        if(assert_args("batch",2,argo.size())){
            return;
        }
        batch_processor(argo[1],true);
    }
    else{
    
        error("Command not found");
    }

    sandbox_watchdog();
}

void parser(string cstr){
    command = cstr;
    vector<string> sepcommands;
    sepcommands = split(command,":");
    if(sepcommands.size() == 0){
        return;
    }

    for(current_instruction = 1; current_instruction - 1 < sepcommands.size();){
        subparser(sepcommands[current_instruction - 1]);
        if(should_increment){
            current_instruction++;
        }else{
            should_increment = true;
        }
    }
}

void batch_processor(string path,bool warn=true){
        if(safe_mode){
            error("Batch processing cannot be done in safe mode");
            return;
        }
        ifstream file;
        file.open(SANDBOX_PATH+_path+fix_path(path), ios::out);
        if(!file.is_open()){
            if(warn){
                error("Cannot find file");
            }
            return;
        }
        string line;
        string fullcmd;
        while ( getline (file,line) ){
            fullcmd+=(line+":");
        }
        parser(fullcmd);
        file.close();
        //command = cstr;


}


void read_and_parse(){
    string prompt_path = _path;
    if(!(prompt_path.size() == 1)){
        prompt_path.erase(prompt_path.size()-1,1);
    }
    if(safe_mode){prompt_path="";}
    cout << prompt_path+"> ";
    getline (cin, c);
    if(c == ""){
        return;
    }
    //.//if(DEBUG)
    //cout << c.back() << endl;

    //cout << endl;
    parser(c);
}

void welcome(){
    system("clear");
    println("Booting ButtOS");
    println("");
    if(!safe_mode){
        batch_processor(AUTOEXEC_PATH,false);
    }
}

void scan(){
    println("ButtOS Problem Scanner v1.0 (C) Butt Group. All rights reserved");
    int problem_count = 0;
    vector<string> problems;
    if(! directory_exists(SANDBOX_PATH.data())){
        problem_count++;
        problems.push_back("Sandbox dir cannot be found (severity:fatal)");
    }else{
        vector<string> paths;
        for (const auto & entry : fs::recursive_directory_iterator(SANDBOX_PATH)){
            string a = pathify(entry.path());
            if(a.find(" ") != string::npos||a.find(".") != string::npos||a.find("~") != string::npos){
                problem_count++;
                problems.push_back("A path has a forbidden character (severity:medium)");
            }
            if(a.find("[A") != string::npos || a.find("[B") != string::npos|| a.find("[C") != string::npos|| a.find("[D") != string::npos){
                problem_count++;
                problems.push_back("Annoying character found in a path. Might move your cursor up and down while ls. (severity:low)");
            }
        }
    }

    if(! directory_exists(_path.data())){
        problem_count++;
        problems.push_back("Current directory is deleted (severity:high)");
    }


    if(!!problem_count){
        error("Oops! you have "+to_string(problem_count)+" problems!");
        for(string problem : problems){
            error(problem);
        }
    }else{
        println("No problems found");
    }
}

void bootloader(){
    println("ButtOS Bootloader");
        println("0 -> CONTINUE BOOTING");
        println("1 -> BOOT IN SAFE MODE");
        println("2 -> EFI PROMPT");
        println("3 -> DISABLE SANDBOX AND BOOT");
        println("4 -> CHOOSE ANOTHER BOOT DEVICE/OS");
        println("5 -> RUN PROBLEM SCANNER AND EXIT");
        println("");
        println("");
        println("");
        println("OS Version:"+OS_VERSION);
        println("Bootloader Version:"+BOOTL_VERSION);
        println("[A[A[A[A[A[C");
        int ans;
        string ttt;
        getline(cin,ttt);
        try{
            ans = stoi(ttt);
        }catch(exception e){
            return; // Implicit error handling
        }
        if(ans == 0){
            ;;;
        }else if(ans == 1){
            safe_mode = true;
        }else if(ans ==2){
            string cmdl;
            while(1){
                cout << "efi>";
                getline(cin,cmdl);
                system(cmdl.data());
            }
        }else if(ans == 3){
            error("DISABLING SANDOX");
            SANDBOX_PATH = "";
        }else if(ans == 4){
            string pth;
            cout <<"Enter path:";
            cin >> pth;
            system(pth.data());
            exit();
        }else if(ans == 5){
            scan();
            exit();
        }
        else{
            ;;;
        }
}

void setup(){
    signal(SIGINT,interrupt);
    signal(SIGTERM,interrupt);
}

int main(int argc,char * argv[]){
    setup();
    system("clear");
    if(argc != 1){
        bootloader();
    }
    welcome();
    while(1){
        //goto loop;
        //loop:
        read_and_parse();
    }
    return 0;
}