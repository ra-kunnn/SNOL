/*


SNOL

THE ADD DOESNT ACTUALLY EXIST FOR THIS ASSIGNMENT !!! JUST A PLACEHOLDER

*/

#include <iostream>
#include <stack>
#include <string>
#include <sstream>
#include <vector>
#include <limits>

using namespace std;

//we get, BEG PRINT EXIT! NUMBER VARIABLE OPERATOR


// Function to split the command into tokens
vector<string> tokenize(const string& command) {
    vector<string> tokens;
    istringstream stream(command);
    string token;
    while (stream >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to handle the HELP command
void printHelp() {
    cout << "Available commands:\n";
    cout << "HELP               - Display this help message\n";
    cout << "BEG <variable>     - Input value for given var\n";
    cout << "PRINT <variable>   - Display value of given variable\n";
    cout << "ADD <number>       - Add the number to the current value\n"; //to remove after use
    cout << "EXIT               - Exit the SNOL environment\n";
}

// handles ADD, removing this later just for reference
void add(stack<int>& valueStack, const string& numberStr) {
    int number = stoi(numberStr);
    if (valueStack.empty()) {
        valueStack.push(number);
    } else {
        int current = valueStack.top();
        valueStack.pop();
        valueStack.push(current + number);
    }
    cout << "Current value: " << valueStack.top();
}

void beg(){}

void print(){}



int main() {
    cout << "The SNOL Environment is now active, you may proceed with giving your commands. Enter HELP for full command list";
    
    stack<int> valueStack; //add stack, delete later

    while (true) {
        cout << "\n\nEnter Command: ";
        string command;
        getline(cin, command);

        vector<string> tokens = tokenize(command);

        if (tokens.empty()){
            cout << "\n\nPlease enter a command.";
            continue;
        }  

        string cmd = tokens[0];

        switch(cmd){
            case "HELP":
                printHelp();
                break;
            case "BEG":
                if (tokens.size() != 2) {
                    cout << "Error: BEG. Wait for the SNOL to ask for value of variable.";
                }
                else beg();
                break;
            case "PRINT":
                if (tokens.size() != 2) {
                    cout << "Error: PRINT. Cannot print multiple variables.";
                }
                else print(tokens[1]);
                break;
            case "ADD": //placeholder
                add(valueStack, tokens[1]);
                break;
            case "EXIT!":
                cout << "\n\nExiting SNOL...";
                return 0;
            default:
                cout << "Unknown command. Enter HELP for a list of available commands.";
                break;
        }

}
