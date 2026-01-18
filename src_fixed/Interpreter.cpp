#include "Interpreter.hpp"
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include "Lexer.hpp"
#include "Parser.hpp"

namespace fs = std::filesystem;

void Interpreter::run(const std::vector<ASTNodePtr>& nodes, const std::string& currentDir) {
    currentDirectory = currentDir;
    for (const auto& node : nodes) {
        if (hasReturned) break;
        execute(node);
    }
}

// FIXED: Helper function to safely convert string to double
bool Interpreter::tryParseDouble(const std::string& str, double& result) {
    try {
        size_t pos = 0;
        result = std::stod(str, &pos);
        // Check if entire string was consumed
        return pos == str.length();
    } catch (const std::invalid_argument&) {
        return false;
    } catch (const std::out_of_range&) {
        return false;
    }
}

void Interpreter::execute(const ASTNodePtr& node) {
    if (!node) return;
    
    switch (node->type) {
        case NodeType::VAR_DECL: {
            std::string value = evaluate(node->children[0]);
            vars[node->value] = value;
            break;
        }
        
        case NodeType::ASSIGNMENT: {
            std::string value = evaluate(node->children[0]);
            vars[node->value] = value;
            break;
        }
        
        case NodeType::PRINT: {
            for (size_t i = 0; i < node->children.size(); ++i) {
                if (i > 0) std::cout << " ";
                std::cout << evaluate(node->children[i]);
            }
            std::cout << std::endl;
            break;
        }
        
        case NodeType::IF: {
            std::string condValue = evaluate(node->children[0]);
            if (isTruthy(condValue)) {
                execute(node->children[1]);
            } else {
                // Check for else if and else
                for (size_t i = 2; i < node->children.size(); ++i) {
                    auto& child = node->children[i];
                    if (child->type == NodeType::IF) {
                        std::string elifCond = evaluate(child->children[0]);
                        if (isTruthy(elifCond)) {
                            execute(child->children[1]);
                            return;
                        }
                    } else if (child->type == NodeType::BLOCK) {
                        // This is the else block
                        execute(child);
                        return;
                    }
                }
            }
            break;
        }
        
        case NodeType::WHILE: {
            while (isTruthy(evaluate(node->children[0]))) {
                execute(node->children[1]);
                if (hasReturned) break;
            }
            break;
        }
        
        case NodeType::FOR: {
            execute(node->children[0]); // init
            while (isTruthy(evaluate(node->children[1]))) { // condition
                execute(node->children[3]); // body
                if (hasReturned) break;
                evaluate(node->children[2]); // increment (evaluate, not execute)
            }
            break;
        }
        
        case NodeType::FUNCTION_DECL: {
            functions[node->value] = node;
            break;
        }
        
        case NodeType::RETURN: {
            if (!node->children.empty()) {
                returnValue = evaluate(node->children[0]);
            } else {
                returnValue = "0";
            }
            hasReturned = true;
            break;
        }
        
        case NodeType::BLOCK: {
            for (const auto& stmt : node->children) {
                execute(stmt);
                if (hasReturned) break;
            }
            break;
        }
        
        case NodeType::IMPORT: {
            // jib XXXX - import module XXXX.lfi3a
            loadModule(node->value);
            break;
        }
        
        case NodeType::IMPORT_FROM: {
            // man YYY jib XXX - from YYY.lfi3a import XXX
            loadModuleItem(node->value, node->params[0]);
            break;
        }
        
        default:
            break;
    }
}

std::string Interpreter::evaluate(const ASTNodePtr& node) {
    if (!node) return "0";
    
    switch (node->type) {
        case NodeType::NUMBER:
            return node->value;
        
        case NodeType::STRING:
            return node->value;
        
        case NodeType::BOOLEAN:
            return node->value; // "s7i7" or "ghalat"
        
        case NodeType::IDENTIFIER: {
            auto it = vars.find(node->value);
            if (it != vars.end()) {
                return it->second;
            }
            std::cerr << "Error: Undefined variable '" << node->value << "'\n";
            exit(1);
        }
        
        case NodeType::BINARY_OP: {
            std::string left = evaluate(node->children[0]);
            std::string right = evaluate(node->children[1]);
            std::string op = node->op;
            
            if (op == "+") {
                // Try numeric addition first, if that fails, do string concat
                double l, r;
                if (tryParseDouble(left, l) && tryParseDouble(right, r)) {
                    double result = l + r;
                    if (result == (int)result) {
                        return std::to_string((int)result);
                    }
                    return std::to_string(result);
                } else {
                    // String concatenation
                    return left + right;
                }
            } else if (op == "-") {
                // FIXED: Safe parsing with error message
                double l, r;
                if (!tryParseDouble(left, l)) {
                    std::cerr << "Error: Cannot subtract - left operand '" << left << "' is not a number\n";
                    exit(1);
                }
                if (!tryParseDouble(right, r)) {
                    std::cerr << "Error: Cannot subtract - right operand '" << right << "' is not a number\n";
                    exit(1);
                }
                double result = l - r;
                if (result == (int)result) {
                    return std::to_string((int)result);
                }
                return std::to_string(result);
            } else if (op == "*") {
                // FIXED: Safe parsing with error message
                double l, r;
                if (!tryParseDouble(left, l)) {
                    std::cerr << "Error: Cannot multiply - left operand '" << left << "' is not a number\n";
                    exit(1);
                }
                if (!tryParseDouble(right, r)) {
                    std::cerr << "Error: Cannot multiply - right operand '" << right << "' is not a number\n";
                    exit(1);
                }
                double result = l * r;
                if (result == (int)result) {
                    return std::to_string((int)result);
                }
                return std::to_string(result);
            } else if (op == "/") {
                // FIXED: Safe parsing with error message
                double l, r;
                if (!tryParseDouble(left, l)) {
                    std::cerr << "Error: Cannot divide - left operand '" << left << "' is not a number\n";
                    exit(1);
                }
                if (!tryParseDouble(right, r)) {
                    std::cerr << "Error: Cannot divide - right operand '" << right << "' is not a number\n";
                    exit(1);
                }
                if (r == 0) {
                    std::cerr << "Error: Division by zero\n";
                    exit(1);
                }
                double result = l / r;
                if (result == (int)result) {
                    return std::to_string((int)result);
                }
                return std::to_string(result);
            } else if (op == "==") {
                return (left == right) ? "s7i7" : "ghalat";
            } else if (op == "!=") {
                return (left != right) ? "s7i7" : "ghalat";
            } else if (op == "<") {
                // FIXED: Safe parsing with error message
                double l, r;
                if (!tryParseDouble(left, l)) {
                    std::cerr << "Error: Cannot compare (<) - left operand '" << left << "' is not a number\n";
                    exit(1);
                }
                if (!tryParseDouble(right, r)) {
                    std::cerr << "Error: Cannot compare (<) - right operand '" << right << "' is not a number\n";
                    exit(1);
                }
                return (l < r) ? "s7i7" : "ghalat";
            } else if (op == ">") {
                // FIXED: Safe parsing with error message
                double l, r;
                if (!tryParseDouble(left, l)) {
                    std::cerr << "Error: Cannot compare (>) - left operand '" << left << "' is not a number\n";
                    exit(1);
                }
                if (!tryParseDouble(right, r)) {
                    std::cerr << "Error: Cannot compare (>) - right operand '" << right << "' is not a number\n";
                    exit(1);
                }
                return (l > r) ? "s7i7" : "ghalat";
            } else if (op == "<=") {
                // FIXED: Safe parsing with error message
                double l, r;
                if (!tryParseDouble(left, l)) {
                    std::cerr << "Error: Cannot compare (<=) - left operand '" << left << "' is not a number\n";
                    exit(1);
                }
                if (!tryParseDouble(right, r)) {
                    std::cerr << "Error: Cannot compare (<=) - right operand '" << right << "' is not a number\n";
                    exit(1);
                }
                return (l <= r) ? "s7i7" : "ghalat";
            } else if (op == ">=") {
                // FIXED: Safe parsing with error message
                double l, r;
                if (!tryParseDouble(left, l)) {
                    std::cerr << "Error: Cannot compare (>=) - left operand '" << left << "' is not a number\n";
                    exit(1);
                }
                if (!tryParseDouble(right, r)) {
                    std::cerr << "Error: Cannot compare (>=) - right operand '" << right << "' is not a number\n";
                    exit(1);
                }
                return (l >= r) ? "s7i7" : "ghalat";
            } else if (op == "w") {
                return (isTruthy(left) && isTruthy(right)) ? "s7i7" : "ghalat";
            } else if (op == "wla") {
                return (isTruthy(left) || isTruthy(right)) ? "s7i7" : "ghalat";
            }
            break;
        }
        
        case NodeType::UNARY_OP: {
            std::string operand = evaluate(node->children[0]);
            std::string op = node->op;
            
            if (op == "-") {
                // FIXED: Safe parsing with error message
                double val;
                if (!tryParseDouble(operand, val)) {
                    std::cerr << "Error: Cannot negate - operand '" << operand << "' is not a number\n";
                    exit(1);
                }
                double result = -val;
                if (result == (int)result) {
                    return std::to_string((int)result);
                }
                return std::to_string(result);
            } else if (op == "post++") {
                // FIXED: Safe parsing for post-increment
                if (node->children[0]->type == NodeType::IDENTIFIER) {
                    double val;
                    if (!tryParseDouble(operand, val)) {
                        std::cerr << "Error: Cannot increment - operand '" << operand << "' is not a number\n";
                        exit(1);
                    }
                    vars[node->children[0]->value] = std::to_string((int)(val + 1));
                    if (val == (int)val) {
                        return std::to_string((int)val);
                    }
                    return std::to_string(val);
                }
            }
            break;
        }
        
        case NodeType::CALL: {
            auto it = functions.find(node->value);
            if (it != functions.end()) {
                // Function exists
                auto funcNode = it->second;
                
                // Save current variables
                auto savedVars = vars;
                bool savedHasReturned = hasReturned;
                std::string savedReturnValue = returnValue;
                
                hasReturned = false;
                returnValue = "0";
                
                // Bind parameters
                for (size_t i = 0; i < funcNode->params.size() && i < node->children.size(); ++i) {
                    vars[funcNode->params[i]] = evaluate(node->children[i]);
                }
                
                // Execute function body
                execute(funcNode->body);
                
                std::string result = returnValue;
                
                // Restore variables
                vars = savedVars;
                hasReturned = savedHasReturned;
                returnValue = savedReturnValue;
                
                return result;
            } else {
                std::cerr << "Error: Undefined function '" << node->value << "'\n";
                exit(1);
            }
        }
        
        default:
            break;
    }
    
    return "0";
}

bool Interpreter::isTruthy(const std::string& value) {
    if (value == "ghalat" || value == "0" || value == "" || value == "0.0") {
        return false;
    }
    return true;
}

std::string Interpreter::toNumber(const std::string& value) {
    double result;
    if (tryParseDouble(value, result)) {
        return std::to_string(result);
    }
    return "0";
}

std::string Interpreter::toString(const std::string& value) {
    return value;
}
void Interpreter::loadModule(const std::string& moduleName) {
    // Check if already imported to prevent cycles
    if (importedModules.find(moduleName) != importedModules.end()) {
        return;
    }
    
    // Try to find the module file in the current directory first, then in current working directory
    std::string filename = moduleName + ".lfi3a";
    std::string fullPath = filename;
    
    if (!currentDirectory.empty()) {
        fs::path dirPath = fs::path(currentDirectory) / filename;
        if (fs::exists(dirPath)) {
            fullPath = dirPath.string();
        }
    }
    
    std::ifstream file(fullPath);
    
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open module file '" << filename << "'\n";
        exit(1);
    }
    
    std::string code((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
    file.close();
    
    // Mark module as imported before parsing to handle circular imports
    importedModules.insert(moduleName);
    
    // Lexer
    Lexer lexer(code);
    auto tokens = lexer.tokenize();
    
    // Parser
    Parser parser(tokens);
    auto ast = parser.parse();
    
    // Execute module code (which will populate vars and functions)
    for (const auto& node : ast) {
        if (hasReturned) {
            hasReturned = false;  // Reset return flag for module execution
        }
        execute(node);
    }
}

void Interpreter::loadModuleItem(const std::string& moduleName, const std::string& itemName) {
    // Check if already imported to prevent cycles
    if (importedModules.find(moduleName) != importedModules.end()) {
        // Module already loaded, item should be available
        return;
    }
    
    // Try to find the module file in the current directory first, then in current working directory
    std::string filename = moduleName + ".lfi3a";
    std::string fullPath = filename;
    
    if (!currentDirectory.empty()) {
        fs::path dirPath = fs::path(currentDirectory) / filename;
        if (fs::exists(dirPath)) {
            fullPath = dirPath.string();
        }
    }
    
    std::ifstream file(fullPath);
    
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open module file '" << filename << "'\n";
        exit(1);
    }
    
    std::string code((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
    file.close();
    
    // Mark module as imported before parsing to handle circular imports
    importedModules.insert(moduleName);
    
    // Lexer
    Lexer lexer(code);
    auto tokens = lexer.tokenize();
    
    // Parser
    Parser parser(tokens);
    auto ast = parser.parse();
    
    // Execute module code (which will populate vars and functions)
    for (const auto& node : ast) {
        if (hasReturned) {
            hasReturned = false;  // Reset return flag for module execution
        }
        execute(node);
    }
    
    // Check if the requested item exists
    bool found = (vars.find(itemName) != vars.end()) || 
                 (functions.find(itemName) != functions.end());
    
    if (!found) {
        std::cerr << "Error: Cannot import '" << itemName << "' from module '" << moduleName << "'\n";
        exit(1);
    }
}