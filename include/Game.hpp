#ifndef GAME_HPP
#define GAME_HPP

#include <string>
#include <vector>

#include "Dictionary.hpp"
#include "Evaluator.hpp"
#include "TerminalView.hpp"
#include "Types.hpp"

class Game {
public:
    Game(Dictionary& dictionary, TerminalView& view, const GameConfig& config);
    int run();

private:
    std::string selectAnswer() const;
    std::string readGuess() const;
    std::string normalizeWord(const std::string& input) const;
    bool isValidGuess(const std::string& guess, std::string& message) const;

    Dictionary& dictionary_;
    TerminalView& view_;
    Evaluator evaluator_;
    GameConfig config_;
    std::vector<GuessResult> history_;
};

#endif
