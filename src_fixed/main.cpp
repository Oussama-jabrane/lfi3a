#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Interpreter.hpp"

namespace fs = std::filesystem;

static bool endsWith(const std::string &s, const std::string &suffix) {
    return s.size() >= suffix.size() &&
           s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: lfi3a <file.lfi3a>\n";
        return 1;
    }

    std::string path = argv[1];
    if (!endsWith(path, ".lfi3a")) {
        std::cerr << "Error: Only .lfi3a files are allowed\n";
        return 1;
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file '" << path << "'\n";
        return 1;
    }

    std::string code((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
    file.close();

    // Get the directory of the input file
    fs::path filePath(path);
    std::string fileDir = fs::absolute(filePath).parent_path().string();

    // Lexer
    Lexer lexer(code);
    auto tokens = lexer.tokenize();

    // Parser
    Parser parser(tokens);
    auto ast = parser.parse();

    // Interpreter
    Interpreter interpreter;
    interpreter.run(ast, fileDir);

    return 0;
}
