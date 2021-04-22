To run the program, run the following lines:

    "g++ -lcrypto PiggyBank.cpp"
    "[output execute name] [argument (See Argument Help Menu)]"


Argument Help Menu:

    Usage: PiggyVBank [-d <argument> -nh] (Only one flag is accepted)

    -d <argument> Enter debit card number of the text file
    -n            User is directed to create a new file
    -h            Display Help

Examples:

    ./a.out -n 
    ./a.out -d Bezos_PiggyCard_4.16.2021.15.53.0.txt