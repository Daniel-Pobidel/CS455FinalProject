
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <iomanip>
#include <bits/stdc++.h>
#include <stdio.h>
#include <chrono>
#include <ctime> 
#include <fstream>
#include <string.h>
#include <vector>
#include <string>
using namespace std;

#include "helpers.cpp"

void process_debit();

//temp variables hard coded for testing, these will be populated by user input
string fname = "Dan", lname="Pobidel", address = "400 Road st, New britain, CT, 06051", account;
int dob[3]= {07,10,1999};
int pin = 0000;
double balance = 0.00;
long verificationCode=100000000000000000;
vector <double> transactions;
vector <string> tranDates;
string accFileName;

int calcBalance(){
    balance = 0;
    for(int tran :transactions){
        balance += tran;
    }
    return balance;
}

//Dan
//Loads data from given input file into global variables
bool loadInputFile(const char *filename){
    FILE *f;
    bool readTran = false;
    int currLine = 1;
    int size = 1024, pos;
    int c;
    char *buffer = (char *)malloc(size);

    if(!filename || strlen(filename)<10) throw "File name too short.";
    
    f = fopen(filename,"r");

    if(f) {
      string temp;
      double tempDouble;
      do { // read all lines in file
        pos = 0;
        do{ // read one line
          c = fgetc(f);
          if(c != EOF) buffer[pos++] = (char)c;
          if(pos >= size - 1) { // increase buffer length - leave room for 0
            size *=2;
            buffer = (char*)realloc(buffer, size);
          }
        }while(c != EOF && c != '\n');
        buffer[pos] = 0;
        // line is now in buffer
        istringstream ss(buffer);

        //read first and last name
        if(currLine == 2){
            ss >> fname;
            ss >> lname;
        }

        //read date of birth
        if(currLine == 3){
            char delim;
            ss >> dob[0] >> delim;
            ss >> dob[1] >> delim;
            ss >> dob[2] >> delim;
        }

        //read address
        if(currLine == 4){
            address = buffer;
        }

        //read Account #
        if(currLine == 5 ){
            getline(ss, temp, ':');
            getline(ss, temp, ' ');
            getline(ss,account);

        }

        if(currLine > 8){
            //read transactions
            getline(ss, temp, '$'); //sign (+/-)

            if(temp.compare("+") == 0){
                getline(ss, temp, ' '); //amount (dollars)
                sscanf(temp.c_str(), "%lf", &tempDouble);
                transactions.push_back(tempDouble);
            }
            else if(temp.compare("-") == 0){
                getline(ss, temp, ' '); //amount (dollars)
                sscanf(temp.c_str(), "%lf", &tempDouble);
                tempDouble *= -1; 
                transactions.push_back(tempDouble);
            }

            //read timestamps   



            //read balance
            if(temp.compare("Balance: ") == 0) {
                getline(ss, temp, '$');
                getline(ss, temp);
                sscanf(temp.c_str(), "%lf", &balance);
            };

            //TO-DO read and check verification code
        }
        currLine++;

      } while(c != EOF); 
      fclose(f); 

        //below prints input from file
        // cout <<fname <<endl;
        // cout<<lname <<endl;
        // cout << dob[0] << "/" << dob[1] << "/" << dob[2] << endl;
        // cout << address << endl;
        // cout << account << endl;
        //cout << fixed << setprecision(2) << balance << endl;
        //for(int i =0; i<transactions.size(); i++)
         //cout << fixed << setprecision(2) << transactions.at(i) << endl;
      cout << "\nSuccessfully loaded your piggyCard file...\n" << endl;
      return true;    
    }
    else{
        throw "Failed to open, specified file not found.";
    }


    
}

//Dan
void loadOutputFileTemplate(const char *filename){
    string header = "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Bank of Piggy~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    string accountInfo = fname + " " + lname + "\n" + to_string(dob[0])+ "/" + to_string(dob[1])+ "/" + to_string(dob[2]) + "\n" + address + "\n" + "Account #: " + account + "\n\n";
    string transHeader = "Transactions \n~~~~~~~~~~~~~\n";
    string footer = "\n\n\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" + to_string(verificationCode) + "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    string fileTemplate = header + accountInfo + transHeader + footer;

    FILE *f;
    f=fopen(filename, "w");
    fputs(fileTemplate.c_str(), f);
    fclose(f);
}

//Dan
//This function is used for the creation of a new account file
//Name of file generated by users last name + "PiggyCard" + timestamp of creation
//calls function to populate new file with template
void createOutputFile(){
    FILE *f;
    time_t t = time(0);   // get time now
    tm* currTime = std::localtime(&t);
    string timestamp = to_string((currTime->tm_mon + 1)) + "." + to_string(currTime->tm_mday) + "."+ to_string((currTime->tm_year + 1900))+"." 
                      + to_string(currTime->tm_hour-4) + "." + to_string(currTime->tm_min) + "." + to_string(currTime->tm_sec) ;

    string filename = lname + "_PiggyCard_" + timestamp + ".txt";
    account = timestamp;

    f=fopen(filename.c_str(), "w");
    fclose(f);

    loadOutputFileTemplate((filename).c_str());

    cout << "Sucessfully created new account!" << endl;
    cout << "Your online piggy bank card is: " << filename << endl;
}

// Ani
// function is used to check if dob is valid
bool isValidDOB(int month, int day, int year){
    if(year > 9999 || year < 1903)
        return false;
    if(day < 1 || day > 31)
        return false;
    if(month < 1 || month > 12)
        return false;

    if(month == 2){
        if( ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0) ){
            if(day <= 29){
                return true;
            }else{
                return false;
            }
        }else{
            if(day <= 28){
                return true;
            }else{
                return false;
            }
        }
    }

    if(month == 4 || month == 6 || month == 9 || month == 11){
        if(day <= 30){
            return true;
        }else{
            return false;
        }
    }

    return true;
}


// Ani
// Function to create new account
// using local varaibles for now, only validaty check is to see if DOB.pin is in correct format. 
void newAcc(){
    int valid = 1;
    cout << "Please enter in your first name: ";
    cin >> fname;
    cout << "Please enter in your last name: ";
    cin >> lname;
    cout << "Please enter your birthday (mm-dd-yyyy). Enter one after another seperated by /: ";
    cin >> dob[0];
    if(cin.get() == '/'){
        cin >> dob[1];
        if(cin.get() == '/'){
            cin >> dob[2];
        }else{
            cout << "Please use the / to sperate the day and year. ";
        }
    }else{
        cout << "Please use the / to seperate the month and day.";
    }
        

    if(isValidDOB(dob[0], dob[1], dob[2]) == false){
        cout << "Date of birth is invalid" << endl;
        valid = 0;
    }
    if(valid == 1){
        cout << "Please enter in your address:";
        cin.ignore();        
        getline(cin, address);
    }
    if(valid == 1){
        cout << "Enter your pin:" << endl;
        cin >> pin;
    }
    if(pin < 1 || pin > 9999){
        cout << "Pin is invalid" << endl;
        valid = 0;
    }
    if(valid == 1){
        cout << endl;
        cout << "Your info is " << fname << " " << lname << " from " << address << " born on " << dob[0] << "/" << dob[1]  << "/" << dob[2] << "\n" <<endl;   
        createOutputFile();     
    }
    
}

// Radek
void usage(){
    cout << "Usage: PiggyVBank [-d <argument> -nh] (Only one flag is accepted)" << endl;
    cout << "-d <argument> Enter debit card number of the text file" << endl;
    cout << "-n            User is directed to create a new file" << endl;
    cout << "-h            Display Help" << endl;
}
/* TODO */
void showAccount(){cout << "showAccount" << endl;}

//Dan
void withdraw(const char *filename){
    double withdrawlAmount;
    cout << "Current balance: $" <<  fixed << setprecision(2) << balance << endl;
    cout << "How much would you like to withdrawl? ";
    double x;
    if (cin >> x) {
      // valid number
      if(withdrawlAmount > balance){
        cout << "You dont have that much money!\n" << endl;
        withdraw(filename);
      }
      else if(withdrawlAmount < 0){
        cout << "You can't withdrawl negative money!\n" << endl;
        withdraw(filename);
      }
      else{
        cin.clear();
        cin >> withdrawlAmount;

        time_t t = time(0);   // get time now
        tm* currTime = std::localtime(&t);
        string timestamp = to_string((currTime->tm_mon + 1)) + "/" + to_string(currTime->tm_mday) + "/"+ to_string((currTime->tm_year + 1900))+" " 
                      + to_string(currTime->tm_hour-4) + ":" + to_string(currTime->tm_min);
        tranDates.push_back(timestamp);

        balance -= withdrawlAmount;

        cout << "You withdrew $" <<  fixed << setprecision(2) << withdrawlAmount << endl;

        withdrawlAmount *= -1;
        transactions.push_back(withdrawlAmount);
        
        cout << "New balance: $" << fixed << setprecision(2) << balance << endl;
        cout << "Returning to main menu...\n" <<endl;
        process_debit();
      }
    } else {
      // not a valid number
      cout << "Invalid Input! Please input a numerical value." << endl;
      cin.clear();
      withdraw(filename);
      
    }
}

/* TODO */
void deposit(const char *filename ){

}

/* TODO */
void changeSettings(){

}

// Radek
void process_debit(){
    /* TODO Show basic info */
    while(1){
        system("echo Type 's' to show account information");
        system("echo Type 'w' to withdraw");
        system("echo Type 'd' to deposit");
        system("echo Type 'a' to change account\\'s information");
        system("echo Type 'q' to quit");
        string input;
        cin >> input;
        if (input.size() != 1) cout << "Please enter only 1 letter!" << endl;
        else if (input[0] == 's' || input[0] == 'S')
            showAccount();
        else if (input[0] == 'w' || input[0] == 'W')
            withdraw(accFileName.c_str());
        else if (input[0] == 'd' || input[0] == 'D')
            deposit(accFileName.c_str());
        else if (input[0] == 'a' || input[0] == 'A')
            changeSettings();
        else if (input[0] == 'q' || input[0] == 'Q'){
            /* TODO Gracefully exit the program */
            cout << "Logging off" << endl;
            exit(0);
        }else cout << "Command not found!" << endl;
    }

}

/* TODO checks if debit file is legit (seperate method maybe) */
bool isDebitLegit(const char * debit){
    return true;
}

/* TODO check is account is not locked */
bool isUnlocked(const char * debit){
    return true;
}

// Radek
void start_debit(char * debit){
    if (loadInputFile(debit)){
        if (isDebitLegit(debit)){
            if (isUnlocked(debit)){
                cout << "Debit Card Read Successfully" << endl;
                int tries = 3;
                string pinEnter;
                bool success = false;
                while (tries > 0 && !success ){
                    cout << "Enter Debit Card's 4 digitPin [0-9 values only]:";
                    showInput(false);
                    cin >> pinEnter;
                    showInput(true);
                    cout << endl;
                    while(!isNumber(pinEnter) || pinEnter.length() != 4)
                    {
                        cout << "Incorrect Input. Please Enter Debit Card's 4 digitPin [0-9 values only]:" << endl;
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        showInput(false);
                        cin >> pinEnter;
                        showInput(true);
                        cout << endl;
                    }
                    string hashedPin = hashString(pinEnter);
                    if (hashedPin.compare(hashString("1234")) == 0){ /* TODO switch 'hashString("1234")' to actual hash value */
                        cout << "Pin sucessfully Entered!" << endl;
                        success = true;
                        process_debit();
                    }else{
                        tries--;
                        cout << "Pin does not match! Please try again. " << tries << " remaining before locked down." << endl;
                        if (!tries){
                            /* TODO add timestamp to file to lock the acccount. (Like 20 secs or so) */
                            fprintf (stderr, "You entered wrong pin 3 times! Your account has been locked for x time!\n");
                            exit(1);                            
                        }
                    }
                }
            }else{
                fprintf (stderr, "Your account is locked. You cannot open it until x time.\n");
                exit(1);
            }
        }else{
            fprintf (stderr, "The Debit Card looks broken. Cannot be used.\n");
            exit(1);
        }
    }else{
        fprintf (stderr, "Card not found! Here is the help menu:\n");
        usage();
        exit(1);
    }
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
                accFileName = dvalue;
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
            start_debit(dvalue);
        }else if (nflag){ 
            fprintf (stdout, "Creating an account now.\n");
            newAcc();
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

    // process_debit();
    start(argc, argv);
    //loadInputFile("Bezos_PiggyCard_4.16.2021.15.53.0.txt");

    return 0;
}



