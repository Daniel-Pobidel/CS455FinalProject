#include <termios.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

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
string hashString(const string str)
{
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