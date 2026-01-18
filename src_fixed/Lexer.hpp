#ifndef LFI3A_LEXER_HPP
#define LFI3A_LEXER_HPP

#include <string>
#include <vector>

enum TokenType {
    // Literals
    IDENT,
    STRING,
    NUMBER,
    
    // Keywords
    DIR,        // var declaration
    KTEB,       // print
    ILA,        // if
    WILA,       // else if
    WLA,        // else / or
    MA7AD,      // while
    KOL,        // for
    DALLA,      // function def
    KALLA,      // function call (implicit)
    RJE3,       // return
    S7I7,       // true
    GHALAT,     // false
    W,          // and
    JIB,        // import
    MAN,        // from (in import statement) like from ... import ...
    
    // Operators
    LPAREN,     // (
    RPAREN,     // )
    LBRACE,     // {
    RBRACE,     // }
    SEMICOLON,  // ;
    COMMA,      // ,
    EQUAL,      // =
    PLUS,       // +
    MINUS,      // -
    STAR,       // *
    SLASH,      // /
    EQ_EQ,      // ==
    NOT_EQ,     // !=
    LT,         // <
    GT,         // >
    LE,         // <=
    GE,         // >=
    PLUS_PLUS,  // ++
    
    // Special
    END,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line = 1;
    int column = 1;
};

class Lexer {
public:
    Lexer(const std::string& src);
    std::vector<Token> tokenize();

private:
    std::string src;
    size_t pos = 0;
    int line = 1;
    int column = 1;

    char peek(size_t offset = 0);
    char advance();
    void skipWhitespace();
    void skipComment();
    Token string();
    Token identifier();
    Token number();
};

#endif
