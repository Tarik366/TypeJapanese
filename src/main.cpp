#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>

enum class tokenType {
    _return,
    int_lit,
    semi
};

struct Token {
    tokenType type;
    std::optional<std::string> value{};
};

std::vector<Token> tokenize(const std::string& str) {
    std::vector<Token> tokens;

    std::string buf;
    for (int i = 0; i < str.length(); i++) {
        char c = str.at(i);
        if (std::isalpha(c)) {
            buf.push_back(c);
            i++;
            while (std::isalnum(str.at(i))) {
                buf.push_back(str.at(i));
                i++;
            }
            i--;

            if (buf == "return") {
                tokens.push_back({.type = tokenType::_return});
                buf.clear();
                continue;
            } else {
                std::cerr << "済んだ：戻事出来なかった" << std::endl;
                exit(EXIT_FAILURE);
            }

        }
        else if (std::isdigit(c)) {
            buf.push_back(c);
            i++;
            while (std::isdigit(str.at(i))) {
                buf.push_back(str.at(i));
                i++;
            }
            i--;
            tokens.push_back({.type= tokenType::int_lit, .value = buf});
            buf.clear();
        }
        else if (c == ';') {
            tokens.push_back({.type = tokenType::semi});
        }
        else if (std::isspace(c)) {
            continue;
        } else {
            std::cerr << "済んだ：ファイルにｔｊと繋がり事見つかりません" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    return tokens;
}

std::string tokensToAsm(const std::vector<Token>& tokens) {
    std::stringstream output;
    output << "global _start\n_start:\n";
    for (int i = 0; i < tokens.size(); i++) {
        const Token& token = tokens.at(i);
        if (token.type == tokenType::_return) {
            if (i + 1 < tokens.size() && tokens.at(i + 1).type == tokenType::int_lit) {
                if (i + 2 < tokens.size() && tokens.at(i + 2).type == tokenType::semi) {
                    output << "    mov rax, 60\n";
                    output << "    mov rdi, " << tokens.at(i + 1).value.value() << "\n";
                    output << "    syscall";
                }
            }
        }
    }
    return output.str();
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cerr << "Incorrect usage. Correct usage is..." << std::endl << "tinihon <input.tn>";
        return EXIT_FAILURE;
    }

    std::string contents;
    {
        std::stringstream contentsStream;
        std::fstream input(argv[1], std::ios::in);
        contentsStream << input.rdbuf();
        contents = contentsStream.str();
    }

    std::vector<Token> tokens = tokenize(contents);
    {
        std::fstream file("out.asm", std::ios::out);
        file << tokensToAsm(tokens);
    }

    system("nasm -felf64 out.asm && ld out.o -o out");

    return EXIT_SUCCESS;
}