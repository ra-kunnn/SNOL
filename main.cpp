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
    NUMBER,
    LPAREN,
    RPAREN,
    UNKNOWN
};


struct Token {
    TokenType type;
    string value;
}

vector<Token> tokenize(const string& command) {
    vector<string> tokens;
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
    cout << "HELP               - Display this help message\n";
    cout << "BEG <variable>     - Input value for given var\n";
    cout << "PRINT <variable>   - Display value of given variable\n";
    cout << "EXIT!               - Exit the SNOL environment\n";
}

void assignVar(){} //variable assignment ex. num = 10+2

void beg(){} //beg command BEG var, then ask input var>

void print(){}



int main() {
    cout << "The SNOL Environment is now active, you may proceed with giving your commands. Enter HELP for full command list";
    
    unordered_map<string, float> variables; //stores key value pairs, so like num = var, it assigns the string to the float (w/c already includes the errm.... iint)

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

        if (cmd == "HELP") {
            printHelp();
        } else if (cmd == "EXIT!") {
            cout << "\n\nExiting SNOL...";
            break;
        } else if (cmd == "BEG") {
            //idk what variables ud need
            beg();
        } else if (cmd == "PRINT") {
            // prints the variable value  lol
            if (tokens.size() != 2) {
                cout << "Error: PRINT. Cannot print multiple variables.";
            } else {
                print(tokens[1].value); // token.value this gives print the variable name
            }
        } else if (tokens[0].type == VARIABLE && tokens.size() > 1 && tokens[1].type == OPERATOR) {
            // Handle variable assignment
            assignVar(variables, tokens[0].value, tokens); 
            //variables is the unordered map of the given var, token0 is name of var, tokens is all the tokens in given command. tho idk bahala u ano need mo
        } else {
            cout << "Unknown command. Enter HELP for a list of available commands.";
        }
    }

    return 0;
}