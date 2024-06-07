#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <unordered_map>
#include <sstream>
#include <limits>
#include <cmath>
#include <stack>

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
    regex tokenPattern("(BEG|PRINT|HELP|EXIT!|[a-zA-Z_][a-zA-Z0-9_]*|[-]?\\d*\\.?\\d+|=|\\+|\\-|\\*|\\/|\\%|\\(|\\))");
    auto words_begin = sregex_iterator(command.begin(), command.end(), tokenPattern);
    auto words_end = sregex_iterator();

    for (sregex_iterator i = words_begin; i != words_end; ++i) {
        smatch match = *i;
        string token = match.str();
        
        if (regex_match(token, regex("(BEG|PRINT|HELP|EXIT!)"))) {
            tokens.push_back({COMMAND, token});
        } else if (regex_match(token, regex("[a-zA-Z_][a-zA-Z0-9_]*"))) {
            tokens.push_back({VARIABLE, token});
        } else if (regex_match(token, regex("[-]?\\d*\\.?\\d+"))) {
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
    // Validate variable name
    if (!regex_match(varName, regex("^[a-zA-Z_][a-zA-Z0-9_]*$"))) {
        cout << "Error: Invalid variable name [" << varName << "]" << endl;
        return;
    }
    
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

// Helper function to get the precedence of an operator
int getPrecedence(const string& op) {
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/" || op == "%") return 2;
    return 0;
}

// Helper function to perform arithmetic operations
float applyOperator(float a, float b, const string& op) {
    if (op == "+") return a + b;
    if (op == "-") return a - b;
    if (op == "*") return a * b;
    if (op == "/") return a / b;
    if (op == "%") return fmod(a, b);
    throw invalid_argument("Invalid operator");
}

// Function to check for balanced parentheses
bool checkParentheses(const vector<Token>& tokens) {
    stack<char> parentheses;
    for (const auto& token : tokens) {
        if (token.type == LPAREN) {
            parentheses.push('(');
        } else if (token.type == RPAREN) {
            if (parentheses.empty()) {
                return false;
            }
            parentheses.pop();
        }
    }
    return parentheses.empty();
}

// Function to evaluate an arithmetic expression using the Shunting Yard algorithm and the Reverse Polish Notation (RPN) evaluation
float evaluateExpression(const vector<Token>& tokens, const string& type) {
    stack<float> values;
    stack<string> operators;
    bool containsInt = false;
    bool containsFloat = false;

    for (const auto& token : tokens) {
        if (token.type == NUMBER) {
            float value = stof(token.value);
            values.push(value);
            if (token.value.find('.') != string::npos) {
                containsFloat = true;
            } else {
                containsInt = true;
            }
        } else if (token.type == VARIABLE) {
            if (variables.find(token.value) == variables.end()) {
                cout << "Undefined variable [" << token.value << "]" << endl;
                throw invalid_argument("Undefined variable");
            }
            values.push(variables[token.value].second);
            if (variables[token.value].first == "float") {
                containsFloat = true;
            } else {
                containsInt = true;
            }
        } else if (token.type == OPERATOR) {
            if (token.value == "%" && (containsFloat || (!values.empty() && modf(values.top(), &values.top()) != 0))) {
                cout << "Modulo operation is not allowed on floats." << endl;
                throw invalid_argument("Modulo on floats");
            }
            while (!operators.empty() && getPrecedence(operators.top()) >= getPrecedence(token.value)) {
                float b = values.top();
                values.pop();
                float a = values.top();
                values.pop();
                string op = operators.top();
                operators.pop();
                values.push(applyOperator(a, b, op));
            }
            operators.push(token.value);
        } else if (token.type == LPAREN) {
            operators.push(token.value);
        } else if (token.type == RPAREN) {
            while (!operators.empty() && operators.top() != "(") {
                float b = values.top();
                values.pop();
                float a = values.top();
                values.pop();
                string op = operators.top();
                operators.pop();
                values.push(applyOperator(a, b, op));
            }
            if (operators.empty() || operators.top() != "(") {
                cout << "Mismatched parentheses" << endl;
                throw invalid_argument("Mismatched parentheses");
            }
            operators.pop(); // Pop the left parenthesis
        }
    }

    while (!operators.empty()) {
        float b = values.top();
        values.pop();
        float a = values.top();
        values.pop();
        string op = operators.top();
        operators.pop();
        values.push(applyOperator(a, b, op));
    }

    // Check for type matching
    if (containsInt && containsFloat) {
        cout << "SNOL> Error! Operands must be of the same type in an arithmetic operation!" << endl;
        throw invalid_argument("Type mismatch");
    }

    return values.top();
}

// Function to handle the assignment command
void assignVar(unordered_map<string, pair<string, float>>& variables, const string& givenVar, const vector<Token>& tokens) {
    if (tokens.size() >= 3 && tokens[1].type == ASSIGNMENT) {
        vector<Token> exprTokens(tokens.begin() + 2, tokens.end());

        // Check if expression is valid
        if (exprTokens.empty() || (exprTokens[0].type != NUMBER && exprTokens[0].type != VARIABLE && exprTokens[0].type != LPAREN)) {
            cout << "Unknown command. Enter HELP for a list of available commands." << endl;
            return;
        }

        // Check for mismatched types in the expression
        string type = "int";
        for (const auto& token : exprTokens) {
            if (token.type == NUMBER && token.value.find('.') != string::npos) {
                type = "float";
                break;
            } else if (token.type == VARIABLE && variables.find(token.value) != variables.end() && variables[token.value].first == "float") {
                type = "float";
                break;
            }
        }

        // Validate that the expression does not contain invalid sequences
        bool lastWasOperator = false;
        for (const auto& token : exprTokens) {
            if (token.type == OPERATOR) {
                if (lastWasOperator) {
                    cout << "Unknown command. Enter HELP for a list of available commands." << endl;
                    return;
                }
                lastWasOperator = true;
            } else {
                lastWasOperator = false;
            }
        }

        // Check for balanced parentheses
        if (!checkParentheses(exprTokens)) {
            cout << "Mismatched parentheses" << endl;
            return;
        }

        try {
            float result = evaluateExpression(exprTokens, type);
            variables[givenVar] = {type, result};
        } catch (const invalid_argument& e) {
            // Error message already printed, do nothing
        }
    } else {
        cout << "Unknown command. Enter HELP for a list of available commands." << endl;
    }
}

// Function to handle non-assignment operations
void handleOperation(const vector<Token>& tokens) {
    if (tokens.size() >= 3 && tokens[1].type == OPERATOR) {
        vector<Token> exprTokens(tokens.begin(), tokens.end());

        // Check if expression is valid
        if (exprTokens.empty() || (exprTokens[0].type != NUMBER && exprTokens[0].type != VARIABLE && exprTokens[0].type != LPAREN)) {
            cout << "Unknown command. Enter HELP for a list of available commands." << endl;
            return;
        }

        // Check for mismatched types in the expression
        string type = "int";
        for (const auto& token : exprTokens) {
            if (token.type == NUMBER && token.value.find('.') != string::npos) {
                type = "float";
                break;
            } else if (token.type == VARIABLE && variables.find(token.value) != variables.end() && variables[token.value].first == "float") {
                type = "float";
                break;
            }
        }

        // Validate that the expression does not contain invalid sequences
        bool lastWasOperator = false;
        for (const auto& token : exprTokens) {
            if (token.type == OPERATOR) {
                if (lastWasOperator) {
                    cout << "Unknown command. Enter HELP for a list of available commands." << endl;
                    return;
                }
                lastWasOperator = true;
            } else {
                lastWasOperator = false;
            }
        }

        // Check for balanced parentheses
        if (!checkParentheses(exprTokens)) {
            cout << "Mismatched parentheses" << endl;
            return;
        }

        try {
            evaluateExpression(exprTokens, type);
        } catch (const invalid_argument& e) {
            // Error message already printed, do nothing
        }
    } else {
        cout << "Unknown command. Enter HELP for a list of available commands." << endl;
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
        } else if (tokens.size() >= 3 && tokens[1].type == OPERATOR) {
            // Handle non-assignment operations
            handleOperation(tokens);
        } else if (tokens[0].type == VARIABLE && tokens.size() == 1) {
            continue;
        } else {
            cout << "Unknown command. Enter HELP for a list of available commands.";
        }
    }

    return 0;
}
