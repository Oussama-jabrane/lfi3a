#include "Parser.hpp"
#include <iostream>
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), pos(0) {}

Token Parser::peek() {
    return pos < tokens.size() ? tokens[pos] : tokens.back();
}

Token Parser::peekNext() {
    return pos + 1 < tokens.size() ? tokens[pos + 1] : tokens.back();
}

Token Parser::advance() {
    Token t = peek();
    if (pos < tokens.size()) pos++;
    return t;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(TokenType type) {
    return peek().type == type;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    std::cerr << "Error: " << message << " at token: " << peek().value << std::endl;
    exit(1);
}

std::vector<ASTNodePtr> Parser::parse() {
    std::vector<ASTNodePtr> nodes;
    
    while (!check(END)) {
        ASTNodePtr stmt = statement();
        if (stmt) nodes.push_back(stmt);
        
        // Skip optional semicolons
        while (match(SEMICOLON)) {}
    }
    
    return nodes;
}

ASTNodePtr Parser::statement() {
    // Skip semicolons
    while (match(SEMICOLON)) {}
    
    if (peek().type == END) return nullptr;
    
    switch (peek().type) {
        case DIR:
            return varDeclaration();
        case JIB:
            return importStatement();
        case MAN:
            return importStatement();  // man ... jib ...
        case KTEB:
            return printStatement();
        case ILA:
            return ifStatement();
        case MA7AD:
            return whileStatement();
        case KOL:
            return forStatement();
        case DALLA:
            return functionDeclaration();
        case RJE3:
            return returnStatement();
        case LBRACE:
            return block();
        default:
            return assignmentOrExpression();
    }
}

ASTNodePtr Parser::varDeclaration() {
    consume(DIR, "Expected 'dir'");
    Token name = consume(IDENT, "Expected variable name");
    
    consume(EQUAL, "Expected '=' in variable declaration");
    
    ASTNodePtr value = expression();
    
    auto node = std::make_shared<ASTNode>();
    node->type = NodeType::VAR_DECL;
    node->value = name.value;
    node->children.push_back(value);
    
    return node;
}

ASTNodePtr Parser::importStatement() {
    auto node = std::make_shared<ASTNode>();
    
    // Check for "man YYY jib XXX" (from YYY import XXX)
    if (match(MAN)) {
        Token module = consume(IDENT, "Expected module name after 'man'");
        consume(JIB, "Expected 'jib' after module name");
        Token item = consume(IDENT, "Expected import name after 'jib'");
        
        node->type = NodeType::IMPORT_FROM;
        node->value = module.value;  // module name
        node->params.push_back(item.value);  // imported item
        
        return node;
    }
    // Check for "jib XXXX" (import XXXX)
    else if (match(JIB)) {
        Token module = consume(IDENT, "Expected module name after 'jib'");
        
        node->type = NodeType::IMPORT;
        node->value = module.value;  // module name
        
        return node;
    }
    
    throw std::runtime_error("Expected 'jib' or 'man' for import statement");
}

ASTNodePtr Parser::printStatement() {
    consume(KTEB, "Expected 'kteb'");
    consume(LPAREN, "Expected '(' after kteb");
    
    auto node = std::make_shared<ASTNode>();
    node->type = NodeType::PRINT;
    
    if (!check(RPAREN)) {
        node->children.push_back(expression());
        while (match(COMMA)) {
            node->children.push_back(expression());
        }
    }
    
    consume(RPAREN, "Expected ')' after kteb arguments");
    
    return node;
}

ASTNodePtr Parser::ifStatement() {
    consume(ILA, "Expected 'ila'");
    consume(LPAREN, "Expected '(' after ila");
    
    ASTNodePtr condition = expression();
    
    consume(RPAREN, "Expected ')' after condition");
    consume(LBRACE, "Expected '{' for if block");
    
    ASTNodePtr thenBlock = block();
    
    auto node = std::make_shared<ASTNode>();
    node->type = NodeType::IF;
    node->children.push_back(condition);
    node->children.push_back(thenBlock);
    
    // Handle wila (else if) and wla (else)
    while (peek().type == WILA) {
        advance();
        consume(LPAREN, "Expected '(' after wila");
        
        ASTNodePtr elseifCond = expression();
        
        consume(RPAREN, "Expected ')' after condition");
        consume(LBRACE, "Expected '{' for wila block");
        
        ASTNodePtr elseifBlock = block();
        
        auto elseifNode = std::make_shared<ASTNode>();
        elseifNode->type = NodeType::IF;
        elseifNode->children.push_back(elseifCond);
        elseifNode->children.push_back(elseifBlock);
        
        node->children.push_back(elseifNode);
    }
    
    if (peek().type == WLA && peekNext().type != W) { // wla alone means else
        advance();
        consume(LBRACE, "Expected '{' for else block");
        
        ASTNodePtr elseBlock = block();
        node->children.push_back(elseBlock);
    }
    
    return node;
}

ASTNodePtr Parser::whileStatement() {
    consume(MA7AD, "Expected 'ma7ad'");
    consume(LPAREN, "Expected '(' after ma7ad");
    
    ASTNodePtr condition = expression();
    
    consume(RPAREN, "Expected ')' after condition");
    consume(LBRACE, "Expected '{' for while block");
    
    ASTNodePtr body = block();
    
    auto node = std::make_shared<ASTNode>();
    node->type = NodeType::WHILE;
    node->children.push_back(condition);
    node->children.push_back(body);
    
    return node;
}

ASTNodePtr Parser::forStatement() {
    consume(KOL, "Expected 'kol'");
    consume(LPAREN, "Expected '(' after kol");
    
    // Parse init
    ASTNodePtr init = assignmentOrExpression();
    consume(SEMICOLON, "Expected ';' after for init");
    
    // Parse condition
    ASTNodePtr condition = expression();
    consume(SEMICOLON, "Expected ';' after for condition");
    
    // Parse increment
    ASTNodePtr increment = assignmentOrExpression();
    consume(RPAREN, "Expected ')' after for clauses");
    
    consume(LBRACE, "Expected '{' for for block");
    ASTNodePtr body = block();
    
    auto node = std::make_shared<ASTNode>();
    node->type = NodeType::FOR;
    node->children.push_back(init);
    node->children.push_back(condition);
    node->children.push_back(increment);
    node->children.push_back(body);
    
    return node;
}

ASTNodePtr Parser::functionDeclaration() {
    consume(DALLA, "Expected 'dalla'");
    Token name = consume(IDENT, "Expected function name");
    
    consume(LPAREN, "Expected '(' after function name");
    
    auto node = std::make_shared<ASTNode>();
    node->type = NodeType::FUNCTION_DECL;
    node->value = name.value;
    
    // Parse parameters
    if (!check(RPAREN)) {
        node->params.push_back(consume(IDENT, "Expected parameter name").value);
        while (match(COMMA)) {
            node->params.push_back(consume(IDENT, "Expected parameter name").value);
        }
    }
    
    consume(RPAREN, "Expected ')' after parameters");
    consume(LBRACE, "Expected '{' for function body");
    
    node->body = block();
    
    return node;
}

ASTNodePtr Parser::returnStatement() {
    consume(RJE3, "Expected 'rje3'");
    
    auto node = std::make_shared<ASTNode>();
    node->type = NodeType::RETURN;
    
    if (!check(SEMICOLON) && !check(RBRACE)) {
        node->children.push_back(expression());
    }
    
    return node;
}

ASTNodePtr Parser::block() {
    std::vector<ASTNodePtr> statements;
    
    while (!check(RBRACE) && !check(END)) {
        ASTNodePtr stmt = statement();
        if (stmt) statements.push_back(stmt);
        
        while (match(SEMICOLON)) {}
    }
    
    if (check(RBRACE)) advance();
    
    auto node = std::make_shared<ASTNode>();
    node->type = NodeType::BLOCK;
    node->children = statements;
    
    return node;
}

ASTNodePtr Parser::assignmentOrExpression() {
    ASTNodePtr expr = expression();
    
    // Check for assignment
    if (match(EQUAL)) {
        ASTNodePtr value = expression();
        
        auto node = std::make_shared<ASTNode>();
        node->type = NodeType::ASSIGNMENT;
        node->value = expr->value; // Variable name
        node->children.push_back(value);
        
        return node;
    }
    
    return expr;
}

ASTNodePtr Parser::expression() {
    return logicalOr();
}

ASTNodePtr Parser::logicalOr() {
    ASTNodePtr expr = logicalAnd();
    
    while (peek().type == WLA && peekNext().type != W) { // wla alone is or
        Token op = advance();
        ASTNodePtr right = logicalAnd();
        
        auto node = std::make_shared<ASTNode>();
        node->type = NodeType::BINARY_OP;
        node->op = op.value;
        node->children.push_back(expr);
        node->children.push_back(right);
        expr = node;
    }
    
    return expr;
}

ASTNodePtr Parser::logicalAnd() {
    ASTNodePtr expr = equality();
    
    while (peek().type == W) {
        Token op = advance();
        ASTNodePtr right = equality();
        
        auto node = std::make_shared<ASTNode>();
        node->type = NodeType::BINARY_OP;
        node->op = op.value;
        node->children.push_back(expr);
        node->children.push_back(right);
        expr = node;
    }
    
    return expr;
}

ASTNodePtr Parser::equality() {
    ASTNodePtr expr = comparison();
    
    while (peek().type == EQ_EQ || peek().type == NOT_EQ) {
        Token op = advance();
        ASTNodePtr right = comparison();
        
        auto node = std::make_shared<ASTNode>();
        node->type = NodeType::BINARY_OP;
        node->op = op.value;
        node->children.push_back(expr);
        node->children.push_back(right);
        expr = node;
    }
    
    return expr;
}

ASTNodePtr Parser::comparison() {
    ASTNodePtr expr = addition();
    
    while (peek().type == LT || peek().type == GT || peek().type == LE || peek().type == GE) {
        Token op = advance();
        ASTNodePtr right = addition();
        
        auto node = std::make_shared<ASTNode>();
        node->type = NodeType::BINARY_OP;
        node->op = op.value;
        node->children.push_back(expr);
        node->children.push_back(right);
        expr = node;
    }
    
    return expr;
}

ASTNodePtr Parser::addition() {
    ASTNodePtr expr = multiplication();
    
    while (peek().type == PLUS || peek().type == MINUS) {
        Token op = advance();
        ASTNodePtr right = multiplication();
        
        auto node = std::make_shared<ASTNode>();
        node->type = NodeType::BINARY_OP;
        node->op = op.value;
        node->children.push_back(expr);
        node->children.push_back(right);
        expr = node;
    }
    
    return expr;
}

ASTNodePtr Parser::multiplication() {
    ASTNodePtr expr = unary();
    
    while (peek().type == STAR || peek().type == SLASH) {
        Token op = advance();
        ASTNodePtr right = unary();
        
        auto node = std::make_shared<ASTNode>();
        node->type = NodeType::BINARY_OP;
        node->op = op.value;
        node->children.push_back(expr);
        node->children.push_back(right);
        expr = node;
    }
    
    return expr;
}

ASTNodePtr Parser::unary() {
    if (peek().type == MINUS) {
        Token op = advance();
        ASTNodePtr expr = unary();
        
        auto node = std::make_shared<ASTNode>();
        node->type = NodeType::UNARY_OP;
        node->op = op.value;
        node->children.push_back(expr);
        
        return node;
    }
    
    return postfix();
}

ASTNodePtr Parser::postfix() {
    ASTNodePtr expr = primary();
    
    while (peek().type == PLUS_PLUS) {
        Token op = advance();
        
        auto node = std::make_shared<ASTNode>();
        node->type = NodeType::UNARY_OP;
        node->op = "post++";
        node->children.push_back(expr);
        expr = node;
    }
    
    return expr;
}

ASTNodePtr Parser::primary() {
    // Numbers
    if (peek().type == NUMBER) {
        Token num = advance();
        auto node = std::make_shared<ASTNode>();
        node->type = NodeType::NUMBER;
        node->value = num.value;
        return node;
    }
    
    // Strings
    if (peek().type == STRING) {
        Token str = advance();
        auto node = std::make_shared<ASTNode>();
        node->type = NodeType::STRING;
        node->value = str.value;
        return node;
    }
    
    // Booleans
    if (peek().type == S7I7 || peek().type == GHALAT) {
        Token bool_tok = advance();
        auto node = std::make_shared<ASTNode>();
        node->type = NodeType::BOOLEAN;
        node->value = bool_tok.value;
        return node;
    }
    
    // Identifiers and function calls
    if (peek().type == IDENT) {
        Token ident = advance();
        
        // Check for function call
        if (peek().type == LPAREN) {
            advance();
            
            auto node = std::make_shared<ASTNode>();
            node->type = NodeType::CALL;
            node->value = ident.value;
            
            if (!check(RPAREN)) {
                node->children.push_back(expression());
                while (match(COMMA)) {
                    node->children.push_back(expression());
                }
            }
            
            consume(RPAREN, "Expected ')' after function arguments");
            
            return node;
        }
        
        // Just an identifier
        auto node = std::make_shared<ASTNode>();
        node->type = NodeType::IDENTIFIER;
        node->value = ident.value;
        return node;
    }
    
    // Parenthesized expression
    if (peek().type == LPAREN) {
        advance();
        ASTNodePtr expr = expression();
        consume(RPAREN, "Expected ')' after expression");
        return expr;
    }
    
    std::cerr << "Error: Unexpected token: " << peek().value << std::endl;
    exit(1);
}
