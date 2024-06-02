#include <iostream>
#include <stack>
#include <string>
#include <sstream>
#include <vector>
#include <limits>

using namespace std;

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
    cout << "ADD <number>    - Add the number to the current value\n";
    cout << "HELP            - Display this help message\n";
    cout << "EXIT            - Exit the SNOL environment\n";
}

// Function to handle the ADD command
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

int main() {
    cout << "The SNOL Environment is now active, you may proceed with giving your commands. Enter HELP for full command list";
    
    stack<int> valueStack;

    while (true) {
        cout << "\n\nEnter Command: ";
        string command;
        getline(cin, command);

        vector<string> tokens = tokenize(command);

        if (tokens.empty()) continue;

        string cmd = tokens[0];

        if (cmd == "HELP") {
            printHelp();
        } else if (cmd == "ADD") {
            if (tokens.size() != 2) {
                cout << "Error: ADD command requires a single operand.";
            } else {
                add(valueStack, tokens[1]);
            }
        } else if (cmd == "EXIT") {
            break;
        } else {
            cout << "Unknown command. Enter HELP for a list of available commands.";
        }
    }

    cout << "Exiting the SNOL Environment. Goodbye!";
    return 0;
}
