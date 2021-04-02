/*

*/
#include <iostream>
#include <cstdio>
#include <stdio.h>
#include <chrono>
#include <ctime> 
#include <fstream>
#include <string.h>
#include <string>
using namespace std;

//temp variables hard coded for testing
string fname = "Dan", lname="Pobidel", address = "400 road st";
int dob[3]= {07,10,1999};
int pin = 0000;
long verificationCode=6754238089205;
int transactions[512]={20,10,5,5,-10};
string tranDates[512]={"01/01/2021","01/02/2021","01/03/2021","01/04/2021","01/05/2021"};


//Dan
void createOutputFile(){
    FILE *f;
    time_t t = time(0);   // get time now
    tm* currTime = std::localtime(&t);
    string timestamp = to_string((currTime->tm_mon + 1)) + "." + to_string(currTime->tm_mday) + "."+ to_string((currTime->tm_year + 1900))+"." 
                      + to_string(currTime->tm_hour-4) + "." + to_string(currTime->tm_min) + "." + to_string(currTime->tm_sec) ;

    string filename = lname + "_PiggyCard_" + timestamp;

    f=fopen(filename.c_str(), "w");

    fclose(f);
    

}

int main(){
    createOutputFile();

    return 0;
}



