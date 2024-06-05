#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <unordered_map>
#include <sstream>
#include <limits>

using namespace std;

enum TokenType {
    COMMAND,
    VARIABLE,
    OPERATOR,
    ASSIGNMENT,
    NUMBER,
    LPAREN,
    RPAREN,
    UNKNOWN
};

struct Token {
    TokenType type;
    string value;
};

vector<Token> tokenize(const string& command) {
    vector<Token> tokens;
    regex tokenPattern("(BEG|PRINT|HELP|EXIT!|[a-zA-Z_][a-zA-Z0-9_]*|\\d*\\.?\\d+|=|\\+|\\-|\\*|\\/|\\%|\\(|\\))");
    auto words_begin = sregex_iterator(command.begin(), command.end(), tokenPattern);
    auto words_end = sregex_iterator();

    for (sregex_iterator i = words_begin; i != words_end; ++i) {
        smatch match = *i;
        string token = match.str();
        
        if (regex_match(token, regex("(BEG|PRINT|HELP|EXIT!)"))) {
            tokens.push_back({COMMAND, token});
        } else if (regex_match(token, regex("[a-zA-Z_][a-zA-Z0-9_]*"))) {
            tokens.push_back({VARIABLE, token});
        } else if (regex_match(token, regex("\\d*\\.?\\d+"))) {
            tokens.push_back({NUMBER, token});
        } else if (token == "=") {
            tokens.push_back({ASSIGNMENT, token});
        } else if (token == "(") {
            tokens.push_back({LPAREN, token});
        } else if (token == ")") {
            tokens.push_back({RPAREN, token});
        } else if (token == "+" || token == "-" || token == "*" || token == "/" || token == "%") {
            tokens.push_back({OPERATOR, token});
        } else {
            tokens.push_back({UNKNOWN, token});
        }
    }
    
    return tokens;
}

// Function to handle the HELP command
void printHelp() {
    cout << "Available commands:\n";
    cout << "HELP                                    - Display this help message\n";
    cout << "BEG <variable>                          - Input value for given var\n";
    cout << "PRINT <variable>                        - Display value of given variable\n";
    cout << "<variable> = <number> | <expression>    - Display value of given variable\n";
    cout << "EXIT!                                   - Exit the SNOL environment\n";
}

// Storage for variables and their types
unordered_map<string, pair<string, float>> variables;

// Function to handle the BEG command
// Function to handle the BEG command
void beg(const string& varName) {
    cout << "SNOL> Please enter value for [" << varName << "]\nInput: ";
    string input;
    getline(cin, input);
    stringstream ss(input);
    float value;
    ss >> value;
    
    // Check if the input is a float or int
    if (input.find('.') != string::npos) {
        variables[varName] = {"float", value};
    } else {
        variables[varName] = {"int", value};
    }
}

// Function to handle the PRINT command
void print(const string& varName) {
    if (variables.find(varName) != variables.end()) {
        cout << "SNOL> [" << varName << "] = " << variables[varName].second << endl;
    } else {
        cout << "Error: Variable " << varName << " not found." << endl;
    }
}

// Placeholder for variable assignment
void assignVar(unordered_map<string, pair<string, float>>& variables, const string& givenVar, const vector<Token>& tokens) {
    // Implementation of assignment operation
    // Example: var = 10 or var = var2 + 5
}

int main() {
    cout << "The SNOL Environment is now active, you may proceed with giving your commands. Enter HELP for full command list";
    
    while (true) {
        cout << "\n\nEnter Command: ";
        string command;
        getline(cin, command);

        vector<Token> tokens = tokenize(command);

        if (tokens.empty()) {
            cout << "\n\nPlease enter a command.";
            continue;
        }  

        string cmd = tokens[0].value;

        if (cmd == "HELP") {
            printHelp();
        } else if (cmd == "EXIT!") {
            cout << "\n\nExiting SNOL...";
            break;
        } else if (cmd == "BEG") {
            if (tokens.size() != 2) {
                cout << "Error: BEG command requires exactly one variable name.";
            } else {
                beg(tokens[1].value);
            }
        } else if (cmd == "PRINT") {
            if (tokens.size() != 2) {
                cout << "Error: PRINT command requires exactly one variable name.";
            } else {
                print(tokens[1].value);
            }
        } else if (tokens[0].type == VARIABLE && tokens.size() > 1 && tokens[1].type == ASSIGNMENT) {
            // Handle variable assignment
            assignVar(variables, tokens[0].value, tokens); 
        } else {
            cout << "Unknown command. Enter HELP for a list of available commands.";
        }
    }

    return 0;
}
