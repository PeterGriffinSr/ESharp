#include "error.hpp"
#include <sstream>

static std::string expandTabs(const std::string& line, int tabSize = 4) {
    std::string result;
    for (char c : line) {
        if (c == '\t')
            result.append(tabSize, ' ');
        else
            result.push_back(c);
    }
    return result;
}

static int visualColumn(const std::string& line, int col, int tabSize = 4) {
    int visualCol = 0;
    for (int i = 0; i < col - 1 && i < (int)line.size(); i++) {
        if (line[i] == '\t') {
            visualCol += tabSize - (visualCol % tabSize);
        } else {
            visualCol++;
        }
    }
    return visualCol;
}

LexerError::LexerError(const std::string& msg, int line, int col, const std::string& sourceLine)
    : message(msg), line(line), col(col), sourceLine(sourceLine) {
    formattedMessage = formatMessage();
}

const char* LexerError::what() const noexcept {
    return formattedMessage.c_str();
}

int LexerError::getLine() const {
    return line;
}

int LexerError::getCol() const {
    return col;
}

std::string LexerError::formatMessage() const {
    std::ostringstream oss;
    oss << "Lexer error at line " << line << ", col " << col << ": " << message << "\n";

    if (!sourceLine.empty()) {
        std::string displayLine = expandTabs(sourceLine);
        int caretPos = visualColumn(sourceLine, col);
        oss << displayLine << "\n";
        oss << std::string(caretPos, ' ') << "^";
    }
    return oss.str();
}
