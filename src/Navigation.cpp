#include <algorithm>
#include <cctype>
#include <cfloat>
#include <cstddef>
#include <cstdlib>
#include <exception>
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
#include <unistd.h>
#include <vector>
#include <string>
using namespace std;

#include "./helpers.cpp"

const string DEBIT_FOLDER = "src/DebitCards/";

void process_debit(const char * debit);

//these will be populated by users input / reading file
string fname, lname, address, account, dataValidationCode, tempDVC, entireFile, hashedFile, unlockTimestamp,accFileName,hashedPin="1";
int dob[3];
int pin;
float balance;
bool isLocked = false;
vector <float> transactions;
vector <string> tranDates;

//Primary author: Dan
//Description: used for verifying balance by adding together every transaction
//Output: returns sum of all transactions
//Vulnerability Addressed: #3 Float Overflows
//resolution - checking amount of space available before adding to preventing float overflow
float calcBalance(){
    float tBalance = 0.0;

    for(float tran :transactions){
        float spaceAvailable = DBL_MAX - tBalance;
        if(tran > spaceAvailable){
            throw "Error calculating balance, transaction total excedes storage limits.";
        }
        else{
            tBalance += tran;
        }
    }
    return tBalance;
}


//Primary author: Dan
//Description:Loads data from given input file into global variables
//Parameter: filename to be opened/read
//Output: boolean true if successful
//Vulnerabilities addressed:
//#5 (catching exceptions): Throw exception if file does not follow expected format
//#7 (Failure to Handle Errors Correctly): Exiting gracefully upon error
bool loadInputFile(const char *filename){
    FILE *f;
    transactions.clear();
    tranDates.clear();
    bool readTran = false;
    int currLine = 1;
    int size = 1024, pos;
    int c;
    char *buffer = (char *)malloc(size);
    if(f!=NULL){
        cout << "already open";
    }

    if(strlen(filename)<(10+DEBIT_FOLDER.size())) throw "Error! Debit File name too short.";
    try{
        f = fopen(filename,"r");
        if(f) {
            string temp;
            float tempFloat;
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
                char char_array[pos];
 
                strcpy(char_array, decrypt1(buffer).c_str());        
                istringstream ss(char_array);

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
                    address = char_array;
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
                        sscanf(temp.c_str(), "%f", &tempFloat);
                        transactions.push_back(tempFloat);
                        //read timestamps
                        getline(ss, temp, '[');
                        getline(ss, temp, ']');
                        tranDates.push_back(temp); 
                    }
                    else if(temp.compare("-") == 0){
                        getline(ss, temp, ' '); //amount (dollars)
                        sscanf(temp.c_str(), "%f", &tempFloat);
                        tempFloat *= -1; 
                        transactions.push_back(tempFloat);
                        //read timestamps
                        getline(ss, temp, '[');
                        getline(ss, temp, ']');
                        tranDates.push_back(temp); 
                    }  

                    //read balance
                    if(temp.compare("Balance: ") == 0) {
                        getline(ss, temp, '$');
                        getline(ss, temp);
                        sscanf(temp.c_str(), "%f", &balance);

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
                    getline(ss, temp, '[');
                    if(temp.compare("!!! ACCOUNT LOCKED !!! ACCOUNT WILL UNLOCK AT ")==0)
                    {
                        getline(ss, unlockTimestamp, ']');
                    }
                    if (!unlockTimestamp.empty()){
                        isLocked = true;
                    } 
                }
                currLine++;

            } while(c != EOF); 
            fclose(f); 
            cout << "\nSuccessfully loaded your piggyCard file...\n" << endl; 
        }
        else{
            throw "Error! Failed to open, specified debit file not found.";
        }   
    }
    catch(...){
        cout << "Failed to read debit file. Most likely it does not exists." << endl;
        return false;
    }
    return true;
}


//Primary author: Dan
//Description: builds and outputs all of file into a string by combining all data into file format structure
//Outputs: Entire file as a string
string createOutputString(){
    ostringstream temp;
    temp << fixed <<setprecision(2) << balance;
    string header = "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Bank of Piggy~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    string accountInfo = fname + " " + lname + "\n" + to_string(dob[0])+ "/" + to_string(dob[1])+ "/" + to_string(dob[2]) + "\n" + address + "Account #: " + account + 
                        "\nData Validation code: " + dataValidationCode + "\n\n";
    string transHeader = "Transactions \n~~~~~~~~~~~~~\n\n";
    string transactionsHistory;
    string footer = "~~~~~~\nBalance: $" + temp.str() + "\n\n~~~~~~~~~~~~~~~~~" + hashedPin + "~~~~~~~~~~~~~~~~~";

    if(isLocked == true){
        footer += "\n\n!!! ACCOUNT LOCKED !!! ACCOUNT WILL UNLOCK AT [" + unlockTimestamp +"]";
    }
    
    for(int i=0; i<transactions.size();i++){
        if(transactions.at(i) >= 0){
            temp.str("");
            temp.clear();
            temp << fixed <<setprecision(2) << transactions.at(i);
            transactionsHistory += "+$";
            transactionsHistory += temp.str();
        }
        else{
            float negate = transactions.at(i)*-1;
            temp.str("");
            temp.clear();
            temp << fixed <<setprecision(2) << negate;
            transactionsHistory += "-$";
            transactionsHistory += temp.str();            
        }
        transactionsHistory += "                                                                     [";
        transactionsHistory += ( tranDates[i] + "]\n\n");
    }
    
    return header + accountInfo + transHeader + transactionsHistory + footer;
}

// Primary Author: Dan
// Description: Calls function to generate file contents and writes it to the output file
//              can be used to create new file or recreate existing file with updated information.
void createOutputFile(const char *filename){
    string fileTemplate = createOutputString();
    string encryptedFileTemplate = encrypt1(fileTemplate);
    cout << fileTemplate << endl;
    FILE *f;
    f=fopen(filename, "w");
    if(f){
        fputs(encryptedFileTemplate.c_str(), f);
    }
    else{
        cout << "Failed to create outputfile" << endl;
    }
    fclose(f);
}

// Primary Author: Dan
// Description: Takes entire file and returns it as a hash string that is used to verify file integrity
string calculateDataValidationCode(const char *debit){
    dataValidationCode = "1";
    entireFile = createOutputString() + debit;
    return hashString(entireFile);
}

// Radek
void updateDebit(const char *filename){
    dataValidationCode = calculateDataValidationCode(filename);
    createOutputFile(filename);
}

// Primary Author: Dan
// Description: Generates name of new debit file by users "[last name]_PiggyCard_[creation timestamp]"" format
//              Timestamp of creation is users account number, folder location added as prefix to create in correct location
void generateFileName(){

    FILE *f;
    time_t t = time(0);   // get time now
    tm* currTime = std::localtime(&t);
    string timestamp = to_string((currTime->tm_mon + 1)) + "." + to_string(currTime->tm_mday) + "."+ to_string((currTime->tm_year + 1900))+"." 
                      + to_string(currTime->tm_hour-4) + "." + to_string(currTime->tm_min) + "." + to_string(currTime->tm_sec) ;

    string filename = DEBIT_FOLDER + lname + "_PiggyCard_" + timestamp + ".txt";
    account = timestamp;
    updateDebit((filename).c_str());
    
    cout << "Sucessfully created new account!" << endl;
    cout << "Your online piggy bank card is: " << filename << endl;
}

// Ani, user input for first name
void takeFName(){
    bool valid;
    string specialChars = "'."; 
    do{
        valid = false;
        cout << "Please enter in your first name: ";
        cin >> fname;
        int len = fname.size();
        if (len < 2){
            cout << "First name is too short. Retype first name please." << endl;
        }else{
            if (len > 20){
                cout << "First name is too long. Retype first name please." << endl;
            }else{
                if (isalpha(fname[0])){
                    for(int i = 0; i < fname.size(); i++){
                        if(!isalpha(fname[i]) && (specialChars.find_first_of(fname[i]) == string::npos)){
                            valid = false;
                            cout << "Invalid input. Please only use letters and special signs {'.}. Retype first name please." << endl;
                            break;
                        }
                     valid = true;
                    }
                }else{
                    cout << "First character has to be a letter. Retype first name please." << endl;
                }
            }
        }
        cin.clear();
        cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
        
    }while(valid == false);
}

// Ani, user input for last name
void takeLName(){
    bool valid;
    string specialChars = "'."; 
    do{
        valid = false;
        cout << "Please enter in your last name: ";
        cin >> lname;
        int len = lname.size();
        if (len < 2){
            cout << "Last name is too short. Retype last name please." << endl;
        }else{
            if (len > 20){
                cout << "Last name is too long. Retype last name please." << endl;
            }else{
                if (isalpha(lname[0])){
                    for(int i = 0; i < lname.size(); i++){
                        if(!isalpha(lname[i]) && (specialChars.find_first_of(lname[i]) == string::npos)){
                            valid = false;
                            cout << "Invalid input. Please only use letters and special signs {'.}. Retype last name please." << endl;
                            break;
                        }
                     valid = true;
                    }
                }else{
                    cout << "Last character has to be a letter. Retype last name please." << endl;
                }
            }
        }
        cin.clear();
        cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
        
    }while(valid == false);
}

// Ani, user input for address
void takeAddress(){
    cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
    bool valid;
    string specialChars = "'# ."; 
    do{
        valid = false;
        cout << "Please enter in your address:";   
        getline(cin, address);
        int len = address.size();
        if (len < 6){
            cout << "Address is too short. Retype address please." << endl;
        }else{
            if (len > 100){
                cout << "Address is too long. Retype address please." << endl;
            }else{
                for(int i = 0; i < address.size(); i++){
                    if(!isalpha(address[i]) && !(isdigit(address[i])) && (specialChars.find_first_of(address[i]) == string::npos)){
                        valid = false;
                        cout << "Invalid input. Please only use letter, numbers, and special signs {'# .}. Retype address please." << endl;
                        break;
                    }
                    valid = true;
                }
            }
        }
        // cin.clear();
        // cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
        
    }while(valid == false);
    address += "\n";
}

// Ani, user input for PIN
string takePin(){
    string tempPin;
    bool finish = false;
    
    while (!finish){
        cout << "Enter your pin [4 digit only]:" << endl;
        showInput(false);
        cin >> tempPin;
        showInput(true);
        if(tempPin.length() == 4){
            if (isNumber(tempPin)){
                finish = true;
            }else{
                cout << "Input Error: Please enter numbers only." << endl;
            }
        }else{
            cout << "Input Error: Pin has to be 4 digits only!." << endl;
        }
    }
    hashedPin = hashString(tempPin);
    return tempPin;
}

// Ani
// Function to create new account
// using local varaibles for now, only validaty check is to see if DOB.pin is in correct format. 
void newAcc(){
    // call standalone functions to get first and last name
    takeFName();
    takeLName();
    
    // Ask for user's dob. 
    bool birthdayVerified = false;
    while (!birthdayVerified){
        cout << "Please enter your birthday (mm/dd/yyyy). Enter one after another seperated by /: ";
        cin >> dob[0];
        if (cin.fail()){
            cout << "Please enter numbers as a month." << endl;
            cin.clear();
            cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        // Makes sure the dob is entered in using correct format.
        if(cin.get() == '/'){
            cin >> dob[1];
            if (cin.fail()){
                cout << "Please enter numbers as a day." << endl;
                cin.clear();
                cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
            if(cin.get() == '/'){
                cin >> dob[2];
                if (cin.fail()){
                    cout << "Please enter numbers as a year." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }
                // Call function that will check if the dob is valid
                if(isValidDOB(dob[0], dob[1], dob[2]) == false) cout << "Date of birth is invalid" << endl;
                else birthdayVerified = true;
            }else cout << "Please use the / to sperate the day and year and only use numbers. " << endl;
        }else cout << "Please use the / to seperate the month and day and only use numbers." << endl;
    }
    takeAddress();
    char pin[5];
    char * pinRetype = new char[5];
    bool cont = true;
    do{
        strcpy(pin, takePin().c_str());
        cout << "Re-enter the same pin please to validate the previous pin:" << endl;
        strcpy(pinRetype, takePin().c_str());
        if (strcmp(pin, pinRetype)!=0)
            cout << endl << "Pins do not match. Re-enter new pin," << endl;
        else{
            cont=false;
            delete [] pinRetype;
        } 
    }while(cont);
    cout << "Pins matched!" << endl;
    cout << "Your info is " << fname << " " << lname << " from " << address << " born on " << dob[0] << "/" << dob[1]  << "/" << dob[2] << "\n" <<endl;   
    generateFileName();   
}

// Radek
void usage(){
    cout << "Usage: PiggyVBank [-d <argument> -nh] (Only one flag is accepted)" << endl;
    cout << "-d <argument> Enter debit card number of the text file" << endl;
    cout << "-n            User is directed to create a new file" << endl;
    cout << "-h            Display Help" << endl;
}

// Primary author: Dan
// Description: Display all of the users unencrypted data
// Addresses vulnerability #8 (Information leakage) by only allowing users who have successfully
// loaded a valid file and entered correct pin viewing access of debit account file data.
void showAccount(){
    string info = createOutputString();
    cout << info << endl;
}

//Primary author: Dan
//Description: takes user input for amount of money to withdrawl and returns to main menu
//Vulnerabilities addressed:
//#7 (Failure to Handle Errors Correctly): Catching invalid user input by verifying it is in valid money format
void withdraw(){
    string withdrawAmountInput;
    float withdrawlAmount;
    cout << "Current balance: $" <<  fixed << setprecision(2) << balance << endl;
    cout << "How much would you like to withdraw? ";

    try {
        // check if valid money format (2 , 2.00, $2, $2.00 all valid inputs)
        cin >> withdrawAmountInput;
        withdrawlAmount = moneyStringToNumber(withdrawAmountInput);
    } catch (const char *msg){
        cerr << msg << "\n" << endl;
        withdraw();
        return;
    }

    if(withdrawlAmount > balance){
        cout << "You dont have that much money!\n" << endl;
        withdraw();
    }
    else if(withdrawlAmount < 0){
        cout << "You can't withdrawl negative money!\n" << endl;
        withdraw();
    }
    else if(withdrawlAmount == 0){
        cout << "You withdrawl nothing!\n" << endl;
    }
    else{
        time_t t = time(0);   // get time now
        string timestamp = getTime(t);
        tranDates.push_back(timestamp);

        balance -= withdrawlAmount;

        cout << "You withdrew $" <<  fixed << setprecision(2) << withdrawlAmount  << endl;

        withdrawlAmount *= -1;
        transactions.push_back(withdrawlAmount);
        
        cout << "New balance: $" << fixed << setprecision(2) << balance << endl;
        cout << "Returning to main menu...\n" <<endl;
    }
}

//Primary author: Dan
//Description: takes user input for amount of money to deposit and returns to main menu
//Vulnerabilities addressed:
//#7 (Failure to Handle Errors Correctly): Catching invalid user input by verifying it is in valid money format
void deposit(){
    string depositAmountInput;
    float depositAmount;
    cout << "Current balance: $" <<  fixed << setprecision(2) << balance << endl;
    cout << "How much would you like to deposit? ";
    
    try {
        // check if valid money format (2 , 2.00, $2, $2.00 all valid inputs)
        cin >> depositAmountInput;
        depositAmount = moneyStringToNumber(depositAmountInput);
    } catch (const char *msg){
        cerr << msg << "\n" << endl;
        deposit();
        return;
    }
        
    if(depositAmount > 1000){
        cout << "You may only deposit up to $1000 at a time\n" << endl;
        deposit();
    }
    else if(depositAmount < 0){
        cout << "You can't deposit negative money!\n" << endl;
        deposit();
    }
    else if(depositAmount == 0){
        cout << "You deposited nothing!\n" << endl;
    }
    else{
        try {
            balance = floatAdd(balance, depositAmount);
        }catch(...){
            cerr << "Adding this amount of money would overflow the piggy bank! Transaction was unsuccesful." << endl;
            cout << "Current balance: $" << fixed << setprecision(2) << balance << endl;
            cout << "Returning to main menu." << endl;
            return;
        }

        time_t t = time(0);   // get time now
        string timestamp = getTime(t);
        tranDates.push_back(timestamp);

        cout << "You deposit $" <<  fixed << setprecision(2) << depositAmount << endl;

        transactions.push_back(depositAmount);
        
        cout << "New balance: $" << fixed << setprecision(2) << balance << endl;
        cout << "Returning to main menu...\n" <<endl;
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
                  string pin;
                string pinRetype;
                bool cont = true;
                do{
                    pin = takePin();
                    cout <<"Re-enter the same pin please to validate the previous pin:" << endl;
                    pinRetype = takePin();
                    if (pin.compare(pinRetype)!=0)
                        cout << "Pins do not match. Re0enter new pin," << endl;
                    else cont=false;
                }while(cont);
                cout << "Pins matched!" << endl;
                break;
            }
                       
        }else if(answer[0] == 'n' || answer[0] == 'n')
            break;
        else cout << "Please enter either 'y' or n!" << endl;
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
void process_debit(const char * debit){
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
        else if (input[0] == 'w' || input[0] == 'W'){
            withdraw();
            updateDebit(debit);
        }
        else if (input[0] == 'd' || input[0] == 'D'){
            deposit();
            updateDebit(debit);
        }
        else if (input[0] == 'a' || input[0] == 'A'){
            changeSettings();
            updateDebit(debit);
            cout << "Changed settings successfully!" << endl;
        }
        else if (input[0] == 'q' || input[0] == 'Q'){
            /* TODO Gracefully exit the program */
            updateDebit(debit);
            cout << "\nLogging off..." << endl;
            exit(0);
        }else cout << "Command not found!" << endl;
    }

}


// Primary author: Dan 
// Description: used for verifying file has not been tampered with
// Inputs: debit file name
// Outputs: true if file is same as when it was saved (not modified outside of program)
//          false if file has been corrupted (modified by user / outside of program)
bool isDebitLegit(const char * debit){
    cout << "Validating file integrity..." <<endl;
    tempDVC = dataValidationCode;
    string currentHash = calculateDataValidationCode(debit);
    dataValidationCode = tempDVC;

    if(dataValidationCode.compare(currentHash) == 0){
        cout << "Data validation successful\n" << endl;
        return true;
    }else{
        cout << "\nFailure to verify file integrity, the Debit Card looks broken and cannot be used." << endl;
        cout << "If you have made modifications to your debit card file, undo them and try again." << endl;
        return false;
    }
}

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
    try{
        if (loadInputFile(debit) ){
            cout << "File loaded!" << endl;
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
                            process_debit(debit);
                        }else{
                            tries--;
                            cout << "Pin does not match! Please try again. " << tries << " remaining before locked down." << endl;
                            if (!tries){
                                isLocked = true;
                                unlockTimestamp = to_string(time(0) + 15);
                                updateDebit(debit);
                                long unlockTime = stol(unlockTimestamp);
                                string timestamp = getTime(unlockTime);
                                fprintf (stderr, "You entered wrong pin 3 times! Your account has been locked until %s (UTC Time)!\n", timestamp.c_str());  
                                exit(1);                            
                            }
                        }
                    }
                }else{
                    long unlockTime = stol(unlockTimestamp);
                    string timestamp = getTime(unlockTime);
                    fprintf (stderr, "You entered wrong pin 3 times! Your account has been locked until %s (UTC Time)!\n", timestamp.c_str());                           
                    exit(1);
                }
            }else{
                fprintf (stderr, "Please contact Piggy Bank Customer support.\n\n");
                exit(1);
            }
        }else{
            exit(1);
        }
    }catch (const char *msg){
        cerr << msg << endl;
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
            string debit  = DEBIT_FOLDER + dvalue;
            start_debit(debit.c_str());
        }else if (nflag){ 
            fprintf (stdout, "Creating an account now.\n");
            newAcc();
        }else if (hflag){
            usage();
        }else{
            fprintf (stderr, "No arguments provided. Here is the help menu:\n");
            usage();
            exit(1);
        }
}
