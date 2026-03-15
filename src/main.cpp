#include <cctype>
#include <exception>
#include <iostream>
#include <string>

#include "Dictionary.hpp"
#include "Game.hpp"
#include "TerminalView.hpp"
#include "Types.hpp"

namespace {
// Command-line answers should behave the same as player input,
// so we normalize them to lowercase before storing them in the config.
// コマンドラインから渡された正解語もプレイヤー入力と同じように扱うべきなので、
// config に保存する前に小文字へ正規化します。
std::string normalizeArgument(const std::string& input) {
    std::string normalized;

    for (std::size_t index = 0; index < input.size(); ++index) {
        normalized.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(input[index]))));
    }
    return normalized;
}
}

int main(int argc, char** argv) {
    try {
        // Escalate broken standard streams to exceptions. We intentionally do
        // not treat ordinary EOF as exceptional, so only badbit is enabled.
        // 壊れた標準ストリームは例外に昇格させます。通常の EOF は例外扱いに
        // したくないため、badbit だけを有効にしています。
        std::cin.exceptions(std::ios::badbit);
        std::cout.exceptions(std::ios::badbit);

        // Centralized runtime options. Keeping them together makes it easy
        // to switch between normal play and deterministic test runs.
        // 実行時オプションを一か所に集約しています。まとめておくことで、
        // 通常プレイと再現性のあるテスト実行を切り替えやすくしています。
        GameConfig config;
        if (argc >= 2) {
            // If the user passes a word on the command line, we treat it as
            // the answer. This is useful when checking the evaluator by hand.
            // ユーザーがコマンドラインで単語を渡した場合は、
            // それを正解語として扱います。判定ロジックを手動で確認するときに便利です。
            config.fixed_answer = normalizeArgument(argv[1]);
        }

        // Load all valid five-letter words before the game starts so that
        // validation and random answer selection are both fast during play.
        // ゲーム開始前に有効な5文字単語をすべて読み込んでおくことで、
        // プレイ中の入力検証とランダムな正解語選択をどちらも高速に行えます。
        Dictionary dictionary(config.dictionary_path);
        dictionary.load();

        // The view owns terminal-specific presentation details, while Game
        // owns the loop and rule enforcement.
        // View はターミナル固有の表示詳細を担当し、
        // Game はゲームループとルール適用を担当します。
        TerminalView view;
        Game game(dictionary, view, config);
        return game.run();
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << std::endl;
        return 1;
    }
}
