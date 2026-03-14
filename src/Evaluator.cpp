#include "Evaluator.hpp"

#include <array>

GuessResult Evaluator::evaluate(const std::string& guess, const std::string& answer) const {
    GuessResult result;
    std::array<int, 26> remaining = {};

    result.guess = guess;
    result.states.assign(answer.size(), LetterState::Gray);

    // First pass:
    // mark exact matches as green and count only the still-unmatched
    // answer letters. This avoids over-crediting duplicate letters.
    // 1回目の走査:
    // 完全一致を green として確定し、まだ一致していない
    // 正解語側の文字だけを数えます。これにより重複文字を過剰に評価しません。
    for (std::size_t index = 0; index < answer.size(); ++index) {
        if (guess[index] == answer[index]) {
            result.states[index] = LetterState::Green;
        } else {
            ++remaining[answer[index] - 'a'];
        }
    }

    // Second pass:
    // for non-green letters, spend from the remaining pool to decide
    // whether a letter is yellow. If the pool is empty, it stays gray.
    // 2回目の走査:
    // green ではない文字について、残っている文字数を消費しながら
    // yellow にできるかを判定します。残数がなければ gray のままです。
    for (std::size_t index = 0; index < guess.size(); ++index) {
        if (result.states[index] == LetterState::Green) {
            continue;
        }
        const int letter_index = guess[index] - 'a';
        if (remaining[letter_index] > 0) {
            result.states[index] = LetterState::Yellow;
            --remaining[letter_index];
        }
    }
    return result;
}
