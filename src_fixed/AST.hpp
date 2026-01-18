#ifndef LFI3A_AST_HPP
#define LFI3A_AST_HPP

#include <string>
#include <vector>
#include <memory>

// Forward declaration
struct ASTNode;
using ASTNodePtr = std::shared_ptr<ASTNode>;

enum class NodeType {
    // Literals
    NUMBER,
    STRING,
    BOOLEAN,
    IDENTIFIER,
    
    // Expressions
    BINARY_OP,
    UNARY_OP,
    CALL,
    
    // Statements
    VAR_DECL,
    PRINT,
    IF,
    WHILE,
    FOR,
    FUNCTION_DECL,
    RETURN,
    BLOCK,
    ASSIGNMENT,
    IMPORT,
    IMPORT_FROM
};

struct ASTNode {
    NodeType type;
    std::string value;  // For literals and identifiers
    std::string op;     // For operators
    std::vector<ASTNodePtr> children;  // For expressions, statements, etc.
    
    // For function declarations
    std::vector<std::string> params;
    ASTNodePtr body;
};

#endif
