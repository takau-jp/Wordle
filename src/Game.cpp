#include "Game.hpp"

#include <cctype>
#include <iostream>
#include <stdexcept>

Game::Game(Dictionary& dictionary, TerminalView& view, const GameConfig& config)
    : dictionary_(dictionary), view_(view), config_(config) {}

int Game::run() {
    // The answer is resolved once at the beginning and remains unchanged
    // for the full game session.
    // 正解語は開始時に一度だけ決定し、
    // そのゲームセッション中は変化しません。
    const std::string answer = selectAnswer();
    std::string error_message;

    view_.printWelcome();
    for (int attempt = 0; attempt < config_.max_attempts; ++attempt) {
        // Read and normalize raw terminal input before validating it.
        // 生のターミナル入力を受け取り、検証前に正規化します。
        const std::string guess = readGuess();

        if (!isValidGuess(guess, error_message)) {
            // Invalid inputs do not consume one of the six official tries.
            // 無効な入力は、正式な6回の試行回数を消費しません。
            view_.printError(error_message);
            --attempt;
            continue;
        }

        // Store every accepted guess so the full board can be redrawn
        // after each turn.
        // 受理された入力はすべて保存し、
        // 各ターン後に盤面全体を再描画できるようにします。
        history_.push_back(evaluator_.evaluate(guess, answer));
        view_.printBoard(history_);
        if (guess == answer) {
            view_.printWin(static_cast<int>(history_.size()));
            return 0;
        }
    }
    view_.printLose(answer);
    return 0;
}

std::string Game::selectAnswer() const {
    // Highest priority: explicit command-line answer for deterministic tests.
    // 最優先: 再現性のあるテストのために、コマンドラインで明示された正解語を使います。
    if (!config_.fixed_answer.empty()) {
        if (!dictionary_.contains(config_.fixed_answer)) {
            throw std::runtime_error("fixed answer is not in dictionary");
        }
        return config_.fixed_answer;
    }

    // Secondary path: a built-in test word for predictable local checks.
    // 次の候補: ローカルで予測しやすい確認ができるよう、組み込みのテスト用単語を使います。
    if (config_.use_test_word) {
        return dictionary_.sampleWord();
    }

    // Default gameplay path: choose a random valid word from the dictionary.
    // 通常プレイの経路: 辞書から有効な単語をランダムに選びます。
    return dictionary_.chooseRandomWord();
}

std::string Game::readGuess() const {
    std::string input;

    // Keep the prompt simple and leave formatting concerns to the view.
    // 入力プロンプトは簡潔に保ち、表示上の複雑さは View 側に持たせます。
    std::cout << "Enter guess: ";
    if (!std::cout) {
        throw std::runtime_error("failed to write input prompt");
    }
    std::getline(std::cin, input);
    if (std::cin.bad()) {
        throw std::runtime_error("input stream became unusable");
    }
    if (!std::cin) {
        throw std::runtime_error("failed to read input");
    }
    return normalizeWord(input);
}

std::string Game::normalizeWord(const std::string& input) const {
    std::string normalized;

    // We strip whitespace entirely and lowercase the remaining characters.
    // This makes the validation rules easier to express later.
    // 空白文字はすべて取り除き、残った文字を小文字に変換します。
    // こうしておくと、後続の入力検証ルールを単純に書けます。
    for (std::size_t index = 0; index < input.size(); ++index) {
        if (!std::isspace(static_cast<unsigned char>(input[index]))) {
            normalized.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(input[index]))));
        }
    }
    return normalized;
}

bool Game::isValidGuess(const std::string& guess, std::string& message) const {
    // Validation is ordered from cheapest checks to more semantic checks,
    // so the player gets a precise and immediate error message.
    // 検証はコストの低いものから意味的に重いものへ順に行い、
    // プレイヤーに正確で即時のエラーメッセージを返せるようにします。
    if (guess.empty()) {
        message = "please enter a word";
        return false;
    }
    if (guess.size() != 5) {
        message = "word must be exactly 5 letters";
        return false;
    }
    for (std::size_t index = 0; index < guess.size(); ++index) {
        if (!std::isalpha(static_cast<unsigned char>(guess[index]))) {
            message = "word must contain letters only";
            return false;
        }
    }
    if (!dictionary_.contains(guess)) {
        message = "word is not in the dictionary";
        return false;
    }
    return true;
}
