#include <cctype>
#include <termios.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <float.h>
#include <iostream>
#include <fstream>
using namespace std;

// Ani
// function is used to check if dob is valid
bool isValidDOB(int month, int day, int year){
    if(year > 2021 || year < 1903){
        cout << "Year is not valid. Enter year between 1903 and 2021 only." << endl;
        return false;
    }
    if(day < 1 || day > 31){
        cout << "Day is not valid. Enter year between 1 and 31 only." << endl;
        return false;
    }
    if(month < 1 || month > 12){
        cout << "Month is not valid. Enter year between 1 and 31 only." << endl;
        return false;
    }

    if(month == 2){
        if( ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0) ){
            if(day <= 29){
                return true;
            }else{
                cout << "This day does not exists in February of " << year << "." << endl;
                return false;
            }
        }else{
            if(day <= 28){
                return true;
            }else{
                cout << "This day does not exists in February of " << year << "." << endl;
                return false;
            }
        }
    }

    if(month == 4 || month == 6 || month == 9 || month == 11){
        if(day <= 30){
            return true;
        }else{
            cout << "This day does not exists in month " << month << "." << endl;
            return false;
        }
    }
    return true;
}

// Radek
bool isMoneyFormat(const string& str)
{
    int len = str.size();
    bool dot = false;
    bool hasDigit = false;
    int count = 0;
    if (len == 0) return false;
    int i = 0;
    if (str[0] == '$') i++;
    for (i=i; i<len; i++){
        if ((str[i] == '.')){
            if (dot) return false;
            dot = true;
            continue;
        } 
        if (dot){
            if (!isdigit(str[i])) return false;
            if (count == 2) return false;
            hasDigit = true;
            count++;
        }else{
            if (!isdigit(str[i])) return false;
            hasDigit = true;
        }
    }
    return hasDigit;
}

// Radek
float moneyStringToNumber(const string& str)
{
    string money;
    int len = str.size();
    bool hasDigit = false;
    bool dot = false;
    int count = 0;
    if (len == 0) throw "Input is not in money format!";
    int i = 0;
    if (str[0] == '$') i++;
    for (i=i; i<len; i++){
        if ((str[i] == '.')){
            if (dot) throw "Input is not in money format!";
            dot = true;
            money += str[i];
            continue;
        } 
        if (dot){
            if (!isdigit(str[i])) throw "Input is not in money format!";
            if (count == 2) throw "Input is not in money format!";
            hasDigit = true;
            money += str[i];
            count++;
        }else{
            if (!isdigit(str[i])) throw "Input is not in money format!";
            money += str[i];
            hasDigit = true;
        }
    }
    if (hasDigit){
        try{
            float answer = stod(money);
            return answer;
        }catch (exception &ex) {
            throw "Money Input is too large!";
        }
    } 
    else throw "Input is not in money format!";
}

// Radek
string getTime(long time){
    if (time < 0) return "";
    tm* currTime = localtime(&time);
    ostringstream  timestamp;
    timestamp << hex << setw(2) << setfill('0') << to_string(currTime->tm_mon+1) << "/";
    timestamp << hex << setw(2) << setfill('0') << to_string(currTime->tm_mday) << "/";
    timestamp << hex << setw(4) << setfill('0') << to_string(currTime->tm_year+1900) << " ";
    timestamp << hex << setw(2) << setfill('0') << to_string(currTime->tm_hour) << ":";
    timestamp << hex << setw(2) << setfill('0') << to_string(currTime->tm_min) << ":";
    timestamp << hex << setw(2) << setfill('0') << to_string(currTime->tm_sec);
    return timestamp.str();
}

// Radek
float floatAdd(float n1, float n2)
{
    double d = n1 + n2;
    if (d > FLT_MAX) throw "Max Size Reached";
    else return d;
}

// Radek
bool isNumber(const string& str)
{
    for (char const &c : str) {
        if (isdigit(c) == 0) return false;
    }
    return true;
}

// Radek
bool inputNameValid(const string& str)
{
    for (char const &c : str) {
        if (isdigit(c) == 0) return false;
    }
    return true;
}

// Radek
void showInput(bool showEcho)
{
    termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if (showEcho) tty.c_lflag |= ECHO;
    else tty.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

// Radek
string hashString(string str)
{
    str += "LIV6HKH3VKRD5"; // salt
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}

// Encryption method that takes in the name of the file and encrypts the contents
// Way to protect user info, in real life application, the encryption scheme would be more intense
int encrypt(string fileName)
{
    char ch;
    fstream fps, fpt;
    // Open file using fstream
    fps.open(fileName, fstream::in);
    // throw error and return from encryption method if file cannot be opened
    if(!fps)
    {
        cout<<"\nError Occurred, Opening the Source File (to Read)!";
        return 0;
    }
    // Open temporary file to hold encrypted bits.
    fpt.open("tmp.txt", fstream::out);
    // throw error if temporary file cannot be opened.
    if(!fpt)
    {
        cout<<"\nError Occurred, Opening/Creating the tmp File!";
        return 0;
    }
    //noskipws is used to check for white space before text input. 
    //only want to encrypt the char inputs
    while(fps>>noskipws>>ch)
    {
        ch = ch+100;
        fpt<<ch;
    }
    //close both files after writing the encrypted bits into the temp file
    fps.close();
    fpt.close();
    //open file again
    fps.open(fileName, fstream::out);
    // Throw error, if file cannot be opened. 
    if(!fps)
    {
        cout<<"\nError Occurred, Opening the Source File (to write)!";
        return 0;
    }
    // open temp file that hold the encrypted bits
    fpt.open("tmp.txt", fstream::in);
    if(!fpt)
    {
        cout<<"\nError Occurred, Opening the tmp File!";
        return 0;
    }
    //write the encrypted contents of the temp file into the original file
    //then close both files
    while(fpt>>noskipws>>ch)
        fps<<ch;
    fps.close();
    fpt.close();
    return 0;
}

int decrypt(string fileName)
{
    char ch;
    fstream fps, fpt;
    //open the file to be decrypted
    fps.open(fileName, fstream::out);
    // throw error and exit if file cannot be opened.
    if(!fps)
    {
        cout<<"\nError Occurred while Opening the Source File!";
        return 0;
    }
    //open the temporary file to write decrypted bits into
    fpt.open("tmp.txt", fstream::in);
    // throw error if file cannot be opened
    if(!fpt)
    {
        cout<<"\nError Occurred while Opening/Creating tmp File!";
        return 0;
    }
    // use noskips to get through white space and then decrement the char values
    // write each char into original file
    while(fpt>>noskipws>>ch)
    {
        ch = ch-100;
        fps<<ch;
    }

    //close both files
    fps.close();
    fpt.close();
    return 0;
}



