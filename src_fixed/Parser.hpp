#ifndef LFI3A_PARSER_HPP
#define LFI3A_PARSER_HPP

#include <vector>
#include <memory>
#include "Lexer.hpp"
#include "AST.hpp"

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::vector<ASTNodePtr> parse();

private:
    std::vector<Token> tokens;
    size_t pos = 0;

    Token peek();
    Token peekNext();
    Token advance();
    bool match(TokenType type);
    bool check(TokenType type);
    Token consume(TokenType type, const std::string& message);
    Token peekNext() const;
    
    ASTNodePtr statement();
    ASTNodePtr declaration();
    ASTNodePtr varDeclaration();
    ASTNodePtr importStatement();
    ASTNodePtr ifStatement();
    ASTNodePtr whileStatement();
    ASTNodePtr forStatement();
    ASTNodePtr functionDeclaration();
    ASTNodePtr returnStatement();
    ASTNodePtr printStatement();
    ASTNodePtr assignmentOrExpression();
    ASTNodePtr block();
    
    ASTNodePtr expression();
    ASTNodePtr logicalOr();
    ASTNodePtr logicalAnd();
    ASTNodePtr equality();
    ASTNodePtr comparison();
    ASTNodePtr addition();
    ASTNodePtr multiplication();
    ASTNodePtr unary();
    ASTNodePtr postfix();
    ASTNodePtr primary();
};

#endif
