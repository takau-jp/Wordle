#ifndef TERMINALVIEW_HPP
#define TERMINALVIEW_HPP

#include <string>
#include <vector>

#include "Types.hpp"

class TerminalView {
public:
    void printWelcome() const;
    void printBoard(const std::vector<GuessResult>& history) const;
    void printError(const std::string& message) const;
    void printWin(int attempts) const;
    void printLose(const std::string& answer) const;
};

#endif
