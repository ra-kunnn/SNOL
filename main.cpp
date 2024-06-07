#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <unordered_map>
#include <sstream>
#include <limits>
#include <cmath>

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

    // Check for unknown words
    string remaining = command;
    for (const auto& token : tokens) {
        size_t pos = remaining.find(token.value);
        if (pos != string::npos) {
            remaining.erase(pos, token.value.length());
        }
    }
    remaining = regex_replace(remaining, regex("\\s+"), "");

    for (char c : remaining) {
        if (!isspace(c)) {
            cout << "Unknown word [" << remaining << "]" << endl;
            tokens.clear();
            break;
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
void beg(const string& varName) {
    cout << "SNOL> Please enter value for [" << varName << "]\nInput: ";
    string input;
    getline(cin, input);
    stringstream ss(input);
    float value;
    ss >> value;

    // Check for invalid number format
    if (ss.fail() || !ss.eof()) {
        cout << "Invalid number format [" << input << "]" << endl;
        return;
    }

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
        cout << "Undefined variable [" << varName << "]" << endl;
    }
}

// Function to evaluate an arithmetic expression
float evaluateExpression(const string& var1, const string& op, const string& var2) {
    float value1, value2;

    // Check if var1 and var2 are numbers or variables
    if (variables.find(var1) != variables.end()) {
        value1 = variables[var1].second;
    } else {
        value1 = stof(var1);
    }

    if (variables.find(var2) != variables.end()) {
        value2 = variables[var2].second;
    } else {
        value2 = stof(var2);
    }

    // Perform the operation
    if (op == "+") return value1 + value2;
    if (op == "-") return value1 - value2;
    if (op == "*") return value1 * value2;
    if (op == "/") return value1 / value2;
    if (op == "%") return fmod(value1, value2);

    throw invalid_argument("Invalid operator");
}

// Function to handle the assignment command
void assignVar(unordered_map<string, pair<string, float>>& variables, const string& givenVar, const vector<Token>& tokens) {
    if (tokens.size() == 3 && tokens[1].type == ASSIGNMENT && tokens[2].type == NUMBER) {
        float value = stof(tokens[2].value);
        if (tokens[2].value.find('.') != string::npos) {
            variables[givenVar] = {"float", value};
        } else {
            variables[givenVar] = {"int", value};
        }
    } else if (tokens.size() == 5 && tokens[1].type == ASSIGNMENT && tokens[3].type == OPERATOR) {
        string var1 = tokens[2].value;
        string op = tokens[3].value;
        string var2 = tokens[4].value;

        if (variables.find(var1) == variables.end() && !regex_match(var1, regex("\\d*\\.?\\d+"))) {
            cout << "Undefined variable [" << var1 << "]" << endl;
            return;
        }

        if (variables.find(var2) == variables.end() && !regex_match(var2, regex("\\d*\\.?\\d+"))) {
            cout << "Undefined variable [" << var2 << "]" << endl;
            return;
        }

        string type1 = (variables.find(var1) != variables.end()) ? variables[var1].first : ((var1.find('.') != string::npos) ? "float" : "int");
        string type2 = (variables.find(var2) != variables.end()) ? variables[var2].first : ((var2.find('.') != string::npos) ? "float" : "int");

        if (type1 != type2) {
            cout << "SNOL> Error! Operands must be of the same type in an arithmetic operation!" << endl;
            return;
        }

        try {
            float result = evaluateExpression(var1, op, var2);
            variables[givenVar] = {type1, result};
        } catch (const invalid_argument& e) {
            cout << "Error: " << e.what() << endl;
        }
    } else {
        cout << "Unknown command. Enter HELP for a list of available commands." << endl;
    }
}

// Function to handle non-assignment operations
void handleOperation(const vector<Token>& tokens) {
    if (tokens.size() == 3 && tokens[1].type == OPERATOR) {
        string var1 = tokens[0].value;
        string op = tokens[1].value;
        string var2 = tokens[2].value;

        if (variables.find(var1) == variables.end() && !regex_match(var1, regex("\\d*\\.?\\d+"))) {
            cout << "Undefined variable [" << var1 << "]" << endl;
            return;
        }

        if (variables.find(var2) == variables.end() && !regex_match(var2, regex("\\d*\\.?\\d+"))) {
            cout << "Undefined variable [" << var2 << "]" << endl;
            return;
        }

        string type1 = (variables.find(var1) != variables.end()) ? variables[var1].first : ((var1.find('.') != string::npos) ? "float" : "int");
        string type2 = (variables.find(var2) != variables.end()) ? variables[var2].first : ((var2.find('.') != string::npos) ? "float" : "int");

        if (type1 != type2) {
            cout << "SNOL> Error! Operands must be of the same type in an arithmetic operation!" << endl;
        }
    }
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
        } else if (tokens.size() == 3 && tokens[1].type == OPERATOR) {
            // Handle non-assignment operations
            handleOperation(tokens);
        } else {
            cout << "Unknown command. Enter HELP for a list of available commands.";
        }
    }

    return 0;
}