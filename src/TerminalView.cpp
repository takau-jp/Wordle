#include "TerminalView.hpp"

#include <cctype>
#include <iostream>
#include <stdexcept>

#include "Color.hpp"

namespace {
// Map logical Wordle states to ANSI escape sequences.
// The terminal interprets these strings as formatting commands rather than
// visible text, which lets us draw colored cells in plain stdout.
// Wordle の論理的な状態を ANSI エスケープシーケンスへ対応付けます。
// ターミナルはこれらの文字列を可視文字ではなく書式指定命令として解釈するため、
// 標準出力だけで色付きセルを描画できます。
const std::string& stateColor(LetterState state) {
    if (state == LetterState::Green) {
        return Color::green;
    }
    if (state == LetterState::Yellow) {
        return Color::yellow;
    }
    return Color::gray;
}
}

void TerminalView::printWelcome() const {
    std::cout << "Welcome to Wordle CLI" << std::endl;
    std::cout << "Guess the 5-letter word in 6 tries." << std::endl;
    if (!std::cout) {
        throw std::runtime_error("failed to write welcome message");
    }
}

void TerminalView::printBoard(const std::vector<GuessResult>& history) const {
    std::cout << std::endl;
    std::cout << "Board:" << std::endl;
    for (std::size_t row = 0; row < history.size(); ++row) {
        for (std::size_t column = 0; column < history[row].guess.size(); ++column) {
            // The surrounding spaces create a wider colored cell so the board
            // reads more like Wordle tiles than plain single characters.
            // 周囲の空白によって色付きセルの幅を広げ、
            // 盤面が単なる1文字表示ではなく Wordle のタイルらしく見えるようにします。
            const char letter = static_cast<char>(std::toupper(static_cast<unsigned char>(history[row].guess[column])));
            std::cout << stateColor(history[row].states[column]) << ' ' << letter << ' ' << Color::reset;
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    if (!std::cout) {
        throw std::runtime_error("failed to write board");
    }
}

void TerminalView::printError(const std::string& message) const {
    std::cout << "Error: " << message << std::endl;
    if (!std::cout) {
        throw std::runtime_error("failed to write error message");
    }
}

void TerminalView::printWin(int attempts) const {
    std::cout << "You win in " << attempts << " attempts." << std::endl;
    if (!std::cout) {
        throw std::runtime_error("failed to write win message");
    }
}

void TerminalView::printLose(const std::string& answer) const {
    std::cout << "You lose. Answer: " << answer << std::endl;
    if (!std::cout) {
        throw std::runtime_error("failed to write lose message");
    }
}
