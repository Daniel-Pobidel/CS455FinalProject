#include <iostream>
#include <cstdio>
#include <stdio.h>
#include <chrono>
#include <ctime> 
#include <fstream>
#include <string.h>
#include <string>
#include <unistd.h>
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
// Radek
void usage(){
    cout << "Usage: PiggyVBank [-d <argument> -nh] (Only one flag is accepted)" << endl;
    cout << "-d <argument> Enter debit card number of the text file" << endl;
    cout << "-n            User is directed to create a new file" << endl;
    cout << "-h            Display Help" << endl;
}

// Radek
void start(int argc, char **argv){
    int dflag = 0; // Debit Card
    int nflag = 0; // New
    int hflag = 0; // Help
    int flags = 0; // Checks if user uses multiple flags
    char *dvalue = NULL; // Debit Card value
    int index;
    int c;

    opterr = 0;
    while ((c = getopt (argc, argv, "d:nh")) != -1) // ":" Means that flag requires argument
        switch (c){
            case 'd':
                dvalue = optarg;
                flags += (1-dflag); // Doesn't count multiple times
                dflag = 1;
                break;
            case 'n':
                flags += (1-nflag); // Doesn't count multiple times
                nflag = 1;
                break;
            case 'h':
                flags += (1-hflag); // Doesn't count multiple times
                hflag = 1;
                break;
            case '?':
                if (optopt == 'd') // Checks if character is printable
                    fprintf (stderr, "Option '-%c' requires an argument.\n", optopt);
                else if (isprint (optopt)) // Checks if character is printable
                    fprintf (stderr, "Unknown argument for '-%c'.\n", optopt);
                else
                    fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
                exit(1);
            default:
                fprintf (stderr, "Arguments not provided.");
                exit(1);
        }
        // Print arguments out for testing purpose. [can be deleted]
        printf ("dflag = %d, dvalue = %s, nflag = %d, hflag = %d, flags = %d\n",
          dflag, dvalue, nflag, hflag, flags);

        if (flags > 1){
            fprintf (stderr, "Too many flags! Choose only 1 option. Here is the help menu:\n");
            usage();
            exit(1);
        }else if (dflag){
            // Do Debit Card Stuff here
        }else if (nflag){
            // Create new account here $$$$ ANI 
            fprintf (stdout, "Creating an account now.\n");
        }else if (hflag){
            usage();
            // Display help menu
        }else{
            fprintf (stderr, "No arguments provided. Here is the help menu:\n");
            usage();
            exit(1);
        }
        // Prints the rest of the arguments [can be deleted]
        // for (index = optind; index < argc; index++)
        //     printf ("Non-option argument %s\n", argv[index]);
}



int main(int argc, char **argv){
    
    start(argc, argv);
    // createOutputFile();

    return 0;
}



