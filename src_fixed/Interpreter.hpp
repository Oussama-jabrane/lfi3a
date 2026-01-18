#ifndef LFI3A_INTERPRETER_HPP
#define LFI3A_INTERPRETER_HPP

#include <unordered_map>
#include <memory>
#include <vector>
#include <set>
#include <string>
#include "AST.hpp"

class Interpreter {
public:
    void run(const std::vector<ASTNodePtr>& nodes, const std::string& currentDir = "");
    
private:
    std::unordered_map<std::string, std::string> vars;
    std::unordered_map<std::string, ASTNodePtr> functions;
    std::set<std::string> importedModules;  // Track imported modules to prevent cycles
    std::string returnValue;
    bool hasReturned = false;
    std::string currentDirectory;  // Directory of the current file being executed
    
    std::string evaluate(const ASTNodePtr& node);
    void execute(const ASTNodePtr& node);
    void loadModule(const std::string& moduleName);
    void loadModuleItem(const std::string& moduleName, const std::string& itemName);
    bool isTruthy(const std::string& value);
    bool tryParseDouble(const std::string& str, double& result);
    std::string toNumber(const std::string& value);
    std::string toString(const std::string& value);
};

#endif
