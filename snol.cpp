/*

SNOL

*/

#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <unordered_map>

using namespace std;

//we get, BEG PRINT EXIT! NUMBER VARIABLE OPERATORS PARENTHESIS

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

void beg(unordered_map<string, float>& variables, const string& varName) {
    float value;
    cout << "SNOL> Please enter value for [" << varName << "]\nInput: ";
    cin >> value;
    variables[varName] = value;
}

void print(unordered_map<string, float>& variables, const string& varName) {
    if (variables.find(varName)!= variables.end()) {
        cout << "SNOL> " << "[" << varName << "]" << " = " << variables[varName] << endl;
    } else {
        cout << "Error! [" << varName << "] is not defined." << endl;
    }
}

void assignVar(unordered_map<string, float>& variables, const string& varName, const vector<Token>& tokens) {
    if (tokens.size() == 3 && tokens[2].type == NUMBER) {
        variables[varName] = stof(tokens[2].value);
    } else {
        cout << "Error! Invalid assignment." << endl;
    }
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

int main() {
    cout << "The SNOL Environment is now active, you may proceed with giving your commands. Enter HELP for full command list";
       
    unordered_map<string, float> variables; //stores key value pairs, so like num = var, it assigns the string to the float (w/c already includes the errm.... iint)

    while (true) {
        cout << "\n\nCommand: ";
        string command;
        getline(cin, command);

        vector<Token> tokens = tokenize(command);

        if (tokens.empty()){
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
                cout << "Error! BEG. Invalid variable.";
            } else {
                beg(variables, tokens[1].value);
            }
        } else if (cmd == "PRINT") {
            if (tokens.size() != 2) {
                cout << "Error! PRINT. Cannot print multiple variables.";
            } else {
                print(variables, tokens[1].value);
            }
        } else if (tokens[1].value == "=") {
            assignVar(variables, cmd, tokens);
        }
    }

    return 0;
}
