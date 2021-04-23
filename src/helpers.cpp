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

// Description: function is used to check if date of birth user input is valid
// Inputs: month, day and year as integers
// Outputs: true if valid, false if invalid (ie. someone claims to be born in 1300)
bool isValidDOB(int month, int day, int year){
    if(year > 2020 || year < 1903){
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

// Description: checks if input is in money format (ie: 2 , 2.00, $2, $2.00 all valid inputs)
// Inputs: money input as a string
// Outputs: true if fits valid formatting, false otherwise
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

// Description: converts money from string to float
// Inputs: money input as a string
// Outputs: money as a float value
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

// Description: converts timestamp from long type to formatted timestamp string
// Inputs: time as a long
// Outputs: nicely formatted time as a string in UTC
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

// Description: checks for overflow when adding two float values
// Inputs:  2 float values to be added together
// Outputs: sum of values as a float
float floatAdd(float n1, float n2)
{
    double d = n1 + n2;
    if (d > FLT_MAX) throw "Max Size Reached";
    else return d;
}

// Description: checks if string is only numbers
// Inputs: string to be validated
// Outputs: bool true if only digits, false if contains non-digit character
bool isNumber(const string& str)
{
    for (char const &c : str) {
        if (isdigit(c) == 0) return false;
    }
    return true;
}

// Description: turns terminal echo on or off
// Inputs: true or false for whether echo should be enabled
void showInput(bool showEcho)
{
    termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if (showEcho) tty.c_lflag |= ECHO;
    else tty.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

// Description: hashes string with SHA256 algorithm
// Inputs: string to be hashed
// Outputs: hashed string
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

// Description: Encryption method that takes in string and encrypts the contents using caesar algorithm
// Inputs: string to be encrypted
// Outputs: encrypted string
string encrypt1(const string& str)
{
    string newStr;
    for (char const &c : str) {
        if (c == '\n' || c == '\r') newStr+=c;
        else newStr += c+100;
    
    }
    return newStr;
}

// Description: Decryption method that takes in encrypted string and decrypted the contents to original string using caesar algorithm
// Inputs: encrypted string to be decrypted
// Outputs: decrypted string
string decrypt1(const string& str)
{
    string newStr;
    for (char const &c : str) {
        if (c == '\n' || c == '\r') newStr+=c;
        else newStr += c-100;
    }
    return newStr;
}



