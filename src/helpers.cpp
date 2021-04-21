#include <cctype>
#include <termios.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <float.h>
#include <iostream>
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
double moneyStringToDouble(const string& str)
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
    if (hasDigit) return (stod(money));
    else throw "Input is not in money format!";
}

// Radek
double doubleAdd(double n1, double n2)
{
    long double ld = n1 + n2;
    if (ld > DBL_MAX) throw "Max Size Reached";
    else return ld;
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