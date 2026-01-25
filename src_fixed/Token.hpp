#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum class TokenType {
    IDENT,
    STRING,
    LPAREN,
    RPAREN,
    END
};

struct Token {
    TokenType type;
    std::string text;
};

#endif
