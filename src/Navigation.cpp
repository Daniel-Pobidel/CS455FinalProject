#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <iomanip>
#include <bits/stdc++.h>
#include <sstream>
#include <stdio.h>
#include <chrono>
#include <ctime> 
#include <fstream>
#include <string.h>
#include <vector>
#include <string>
using namespace std;

#include "./helpers.cpp"

void process_debit();

//temp variables hard coded for testing, these will be populated by user input
string fname = "Dan", lname="Pobidel", address = "400 Road st, New britain, CT, 06051", account;
int dob[3]= {07,10,1999};
int pin = 0000;
double balance = 0.00;
bool isLocked = false;
string unlockTimestamp;
string hashedPin="1";
string dataValidationCode, tempDVC;
string entireFile, hashedFile;
vector <double> transactions;
vector <string> tranDates;
string accFileName;


//Dan
//temp func, could be used for verifying balance == transaction total, may not be used
int calcBalance(){
    balance = 0;
    for(double tran :transactions){
        balance += tran;
    }
    return balance;
}


//Dan
//Loads data from given input file into global variables
bool loadInputFile(const char *filename){

    FILE *f;
    transactions.clear();
    tranDates.clear();
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

        //read data validation code
        if(currLine == 6 ){
            getline(ss, temp, ':');
            getline(ss, temp, ' ');
            getline(ss,dataValidationCode);
        }

        if(currLine > 9 && readTran == false){
            //read transactions
            getline(ss, temp, '$'); //sign (+/-)

            if(temp.compare("+") == 0){
                getline(ss, temp, ' '); //amount (dollars)
                sscanf(temp.c_str(), "%lf", &tempDouble);
                transactions.push_back(tempDouble);
                //read timestamps
                getline(ss, temp, '[');
                getline(ss, temp, ']');
                tranDates.push_back(temp); 
            }
            else if(temp.compare("-") == 0){
                getline(ss, temp, ' '); //amount (dollars)
                sscanf(temp.c_str(), "%lf", &tempDouble);
                tempDouble *= -1; 
                transactions.push_back(tempDouble);
                //read timestamps
                getline(ss, temp, '[');
                getline(ss, temp, ']');
                tranDates.push_back(temp); 
            }  

            //read balance
            if(temp.compare("Balance: ") == 0) {
                getline(ss, temp, '$');
                getline(ss, temp);
                sscanf(temp.c_str(), "%lf", &balance);

                readTran = true;
            };
        }

        if(readTran && hashedPin.compare("1")==0){
            getline(ss, temp, '~');
            //iterate through each '~' character until hashed pin is read
            while(temp == "" && hashedPin.compare("1")==0){
                getline(ss, temp, '~');
                if(temp.compare("") != 0){
                    hashedPin = temp;
                }
            }
        }

        //read lock timetamp if exists after pin has been read
        if(hashedPin.compare("1")!=0){
            isLocked = true;
            getline(ss, temp, '[');
            // while(temp == ""){
            //     getline(ss, temp, '[');
            if(temp.compare("!!! ACCOUNT LOCKED !!! ACCOUNT WILL UNLOCK AT ")==0){
                    getline(ss, unlockTimestamp, ']');
                }
            //}
        }

        
        currLine++;

      } while(c != EOF); 
      fclose(f); 

        //below prints input from file
        // cout << "fname: " <<fname <<endl;
        // cout <<"lname: " <<lname <<endl;
        // cout <<"dob: " << dob[0] << "/" << dob[1] << "/" << dob[2] << endl;
        // cout << "address: " << address << endl;
        // cout << "account #: "<< account << endl;
        // cout << "DVC: " << dataValidationCode << endl;
        // cout << "balance" << fixed << setprecision(2) << balance << endl;
        // for(int i =0; i<transactions.size(); i++)
        //  cout << "transaction" << fixed << setprecision(2) << transactions.at(i) << endl;
        // for(int i =0; i<tranDates.size(); i++)
        //  cout << "transaction date: " << tranDates.at(i) << endl;
        // cout << "verification code: " << hashedPin << endl;
        // cout << "unlock timestamp " << unlockTimestamp << endl;
      cout << "\nSuccessfully loaded your piggyCard file...\n" << endl;

      //TODO - validate file has not been modified
      return true;    
    }
    else{
        throw "Failed to open, specified file not found.";
    }


    
}

//Dan
void createOutputFile(const char *filename){
    ostringstream temp;
    temp<<setprecision(3) << balance;
    string header = "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Bank of Piggy~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    string accountInfo = fname + " " + lname + "\n" + to_string(dob[0])+ "/" + to_string(dob[1])+ "/" + to_string(dob[2]) + "\n" + address + "Account #: " + account + 
                        "\nData Validation code: " + dataValidationCode + "\n\n";
    string transHeader = "Transactions \n~~~~~~~~~~~~~\n\n";
    string transactionsHistory;
    string footer = "~~~~~~\nBalance: $" + temp.str() + "\n\n~~~~~~~~~~~~~~~~~" + hashedPin + "~~~~~~~~~~~~~~~~~";

    if(isLocked){
        footer += "\n\n!!! ACCOUNT LOCKED !!! ACCOUNT WILL UNLOCK AT [" + unlockTimestamp +"]";
    }
    
    for(int i=0; i<transactions.size();i++){
        if(transactions.at(i) >= 0){
            transactionsHistory += "+$";
            transactionsHistory += (to_string(transactions.at(i)));
        }
        else{
            double temp = transactions.at(i)*-1;
            transactionsHistory += "-$";
            transactionsHistory += to_string(temp);            
        }
        transactionsHistory += "                                                                     [";
        transactionsHistory += ( tranDates[i] + "]\n\n");
    }
    
    string fileTemplate = header + accountInfo + transHeader + transactionsHistory + footer;

    FILE *f;
    f=fopen(filename, "w");
    fputs(fileTemplate.c_str(), f);
    fclose(f);
}

//Dan
//This function is used for the creation of a new account file
//Name of file generated by users last name + "PiggyCard" + timestamp of creation
//calls function to populate new file with template
void generateFileName(){

    FILE *f;
    time_t t = time(0);   // get time now
    tm* currTime = std::localtime(&t);
    string timestamp = to_string((currTime->tm_mon + 1)) + "." + to_string(currTime->tm_mday) + "."+ to_string((currTime->tm_year + 1900))+"." 
                      + to_string(currTime->tm_hour-4) + "." + to_string(currTime->tm_min) + "." + to_string(currTime->tm_sec) ;

    string filename = lname + "_PiggyCard_" + timestamp + ".txt";
    account = timestamp;

    createOutputFile((filename).c_str());

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
// User input functions:
void takeFName(){
    cout << "Please enter in your first name: ";
    cin >> fname;
    cout << endl; 
}
void takeLName(){
    cout << "Please enter in your last name: ";
    cin >> lname;
    cout << endl; 
}
void takeAddress(){
    cout << "Please enter in your address:";
    cin.ignore();        
    getline(cin, address);
    address += "\n";
    cout << endl; 
}
int takePin(){
    int tempPin;
    cout << "Enter your pin:" << endl;
    cin >> tempPin;
    if(cin.fail()){
        cout << "Please enter numbers only." << endl;
        return 0;
    }
    if(tempPin < 1 || tempPin > 9999){
        cout << "Pin is invalid" << endl;
        return 0;
    }
    pin = tempPin;
    cout << endl; 
    return 1;
}

// Ani
// Function to create new account
// using local varaibles for now, only validaty check is to see if DOB.pin is in correct format. 
void newAcc(){
    // varaibles to hold the validity of each process
    int valid = 1;

    // call standalone functions to get first and last name
    takeFName();
    takeLName();
    
    // Ask for user's dob. 
    cout << "Please enter your birthday (mm-dd-yyyy). Enter one after another seperated by /: ";
    cin >> dob[0];
    // Makes sure the dob is entered in using correct format.
    if(cin.get() == '/'){
        cin >> dob[1];
        if(cin.get() == '/'){
            cin >> dob[2];
        }else{
            cout << "Please use the / to sperate the day and year and only use numbers. ";
            valid = 0;
        }
    }else{
        cout << "Please use the / to seperate the month and day and only use numbers.";
        valid = 0;
    }
    
    // Call function that will check if the dob is valid
    if(isValidDOB(dob[0], dob[1], dob[2]) == false){
        cout << "Date of birth is invalid" << endl;
        valid = 0;
    }

    // If user input so far is valid, then as for user address.
    if(valid == 1){
        takeAddress();
    }

    // If user input so far is valid, then ask for Pin number.
    if(valid == 1){
        if(takePin() == 0){
            valid = 0;
        }
    }

    // If all user input is valid, show user their info and call createOutPut file to create an account 
    if(valid == 1){
        cout << endl;
        cout << "Your info is " << fname << " " << lname << " from " << address << " born on " << dob[0] << "/" << dob[1]  << "/" << dob[2] << "\n" <<endl;   
        generateFileName();   
    }
    
}

// Radek
void usage(){
    cout << "Usage: PiggyVBank [-d <argument> -nh] (Only one flag is accepted)" << endl;
    cout << "-d <argument> Enter debit card number of the text file" << endl;
    cout << "-n            User is directed to create a new file" << endl;
    cout << "-h            Display Help" << endl;
}

// Ani
// Display the user's info excluding transaction
void showAccount(){
    cout << "Showing account information for: " << account << endl;
    cout << "Name: " << lname << "," << fname << endl;
    cout << "Date of Birth: " << dob[0] << "/" << dob[1] << "/" << dob[2] << endl;   
    cout << "Address: " << address << endl;
    cout << "Account Balance: " << balance << endl;
}

//Dan
void withdraw(const char *filename){
    double withdrawlAmount;
    cout << "Current balance: $" <<  fixed << setprecision(2) << balance << endl;
    cout << "How much would you like to withdrawl? ";
    if (cin >> withdrawlAmount) {
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
        time_t t = time(0);   // get time now
        tm* currTime = std::localtime(&t);
        string timestamp = to_string((currTime->tm_mon + 1)) + "/" + to_string(currTime->tm_mday) + "/"+ to_string((currTime->tm_year + 1900))+" " 
                      + to_string(currTime->tm_hour-4) + ":" + to_string(currTime->tm_min);
        tranDates.push_back(timestamp);

        balance -= withdrawlAmount;

        cout << "You withdrew $" <<  fixed << setprecision(2) << withdrawlAmount  << endl;

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
    double depositAmount;
    cout << "Current balance: $" <<  fixed << setprecision(2) << balance << endl;
    cout << "How much would you like to deposit? ";
    if (cin >> depositAmount) {
      // valid number
      if(depositAmount > 1000){
        cout << "You may only deposit up to $1000 at a time\n" << endl;
        deposit(filename);
      }
      else if(depositAmount < 0){
        cout << "You can't deposit negative money!\n" << endl;
        deposit(filename);
      }
      else if(depositAmount == 0){
          cout << "You deposited nothing!\n" << endl;
      }
      else{
        time_t t = time(0);   // get time now
        tm* currTime = std::localtime(&t);
        string timestamp = to_string((currTime->tm_mon + 1)) + "/" + to_string(currTime->tm_mday) + "/"+ to_string((currTime->tm_year + 1900))+" " 
                      + to_string(currTime->tm_hour-4) + ":" + to_string(currTime->tm_min);
        tranDates.push_back(timestamp);

        balance += depositAmount;

        cout << "You deposit $" <<  fixed << setprecision(2) << depositAmount << endl;

        transactions.push_back(depositAmount);
        
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

// condensed function that will rotate through the types of user info that can be changed
void askForNewInfo(int info){
    // string answer for user input
    string answer = "";
    string typeOfInfo;
    if(info == 1){
        typeOfInfo = "first name";
    }else if(info == 2){
        typeOfInfo = "last name";
    }else if(info == 3){
        typeOfInfo = "address";
    }else if(info == 4){
        typeOfInfo = "PIN";
    }

    //while loop to keep going if 'y' or 'n' is not used
    while(tolower(answer[0]) != 'y' || tolower(answer[0]) != 'n'){
        // ask for user's first name
        cout << "Would you like to change the " << typeOfInfo << " (y/n): ";
        cin >> answer;
        // check to see if the user input is 1 character
        // then check to see if the user input is either a 'y' 'Y' 'n' 'N'
        // if user chooses yes, then call the appropirate user input functions.
        if (answer.size() != 1){
            cout << "Please enter only 1 letter!" << endl;
        }else if(answer[0] == 'y' || answer[0] == 'Y'){
            if(info == 1){
                takeFName();
                break;
            }
            if(info == 2){
                takeLName();
                break;
            }
            if(info == 3){
                takeAddress();
                break;
            }
            if(info == 4){
                int x = takePin();
                break;
            }
                       
        }else if(answer[0] == 'n' || answer[0] == 'n')
            break;
    }

}


//Ani
// Change user settings using standalone functions
void changeSettings(){
    // ask for first name
    askForNewInfo(1);
    // ask for last name
    askForNewInfo(2);
    // ask for the address
    askForNewInfo(3);
    // ask for the PIN
    askForNewInfo(4);
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
            createOutputFile(accFileName.c_str());
            cout << "\nLogging off..." << endl;
            exit(0);
        }else cout << "Command not found!" << endl;
    }

}

/* Dan */
string calculateDataValidationCode(const char * debit){
    dataValidationCode = "1";
    entireFile = "";
    createOutputFile(debit); //recreate file with constant for vlaidation code
    sleep(2);

    FILE *f;
    int currLine = 1;
    int size = 1024, pos;
    int c;
    char *buffer = (char *)malloc(size);
    
    f = fopen(debit,"r");
    if(f) {
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
      entireFile += buffer;
        
      currLine++;
      } while(c != EOF); 
    fclose(f);
    }
    return hashString(entireFile);
}


/* Dan */
bool isDebitLegit(const char * debit){
    cout << "Validating file integrity..." <<endl;
    tempDVC = dataValidationCode;
    string currentHash = calculateDataValidationCode(debit);
    dataValidationCode = tempDVC;
    createOutputFile(debit);

    if(dataValidationCode.compare(currentHash) == 0){
        cout << "Data validation successful\n" << endl;
        return true;
    }else{
        cout << "\nFailure to verify file integrity, the Debit Card looks broken and cannot be used." << endl;
        cout << "If you have made modifications to your debit card file, undo them and try again." << endl;
        return false;
    }
}

/* TODO check is account is not locked */
bool isUnlocked(const char * debit){
    if (unlockTimestamp.empty()) return true;
    time_t currentTime = time(0);
    long lockedTime = stol(unlockTimestamp);
    if (currentTime > lockedTime)
    {
        isLocked = false;
        return true;
    }else{
        isLocked = true;
        return false;
    }
}

// Radek
void start_debit(const char * debit){
    if (loadInputFile(debit)){
        if (isDebitLegit(debit)){
            if (isUnlocked(debit)){
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
                    string hashedPinInput = hashString(pinEnter);
                    if (hashedPinInput.compare(hashedPin) == 0){ 
                        cout << "Pin sucessfully Entered!\n" << endl;
                        success = true;
                        process_debit();
                    }else{
                        tries--;
                        cout << "Pin does not match! Please try again. " << tries << " remaining before locked down." << endl;
                        if (!tries){
                            isLocked = true;
                            unlockTimestamp = to_string(time(0) + 15);
                            string newDatValidationCode = calculateDataValidationCode(debit);
                            dataValidationCode = newDatValidationCode;
                            createOutputFile(debit);
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
            fprintf (stderr, "Please contact Piggy Bank Customer support.\n\n");
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
    char * dvalue = NULL; // Debit Card value
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
            string folder = "src/DebitCards/";
            string debit  = folder + dvalue;
            start_debit(debit.c_str());
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
}
