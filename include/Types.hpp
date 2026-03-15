#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
#include <vector>

enum class LetterState {
    Gray,
    Yellow,
    Green
};

struct GuessResult {
    std::string guess;
    std::vector<LetterState> states;
};

struct GameConfig {
    std::string dictionary_path = "data/words.txt";
    int max_attempts = 6;
    bool use_test_word = false;
    std::string fixed_answer;
};

#endif
