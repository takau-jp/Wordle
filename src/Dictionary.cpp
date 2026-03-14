#include "Dictionary.hpp"

#include <cctype>
#include <fstream>
#include <random>
#include <stdexcept>

namespace {
bool isValidDictionaryWord(const std::string& word) {
    if (word.empty() || word.size() != 5) {
        return false;
    }
    for (std::size_t index = 0; index < word.size(); ++index) {
        if (!std::isalpha(static_cast<unsigned char>(word[index]))) {
            return false;
        }
    }
    return true;
}

std::string normalizeDictionaryWord(const std::string& line) {
    std::string normalized = line;

    if (!normalized.empty() && normalized[normalized.size() - 1] == '\r') {
        normalized.erase(normalized.size() - 1);
    }
    for (std::size_t index = 0; index < normalized.size(); ++index) {
        normalized[index] = static_cast<char>(std::tolower(static_cast<unsigned char>(normalized[index])));
    }
    return normalized;
}
}

Dictionary::Dictionary(const std::string& path) : path_(path) {}

void Dictionary::load() {
    std::ifstream file(path_.c_str());
    std::string line;

    if (!file) {
        throw std::runtime_error("failed to open dictionary file");
    }
    // We keep both an ordered list and a hash set:
    // - vector: stable random access when picking an answer
    // - unordered_set: fast membership checks for player guesses
    // 順序付きリストとハッシュ集合の両方を保持します:
    // - vector: 正解語を選ぶときに安定したランダムアクセスができる
    // - unordered_set: プレイヤーの入力語が存在するかを高速に確認できる
    // Skip malformed lines so accidental blanks or bad tokens do not poison
    // the in-memory dictionary used by the game.
    // 不正な行は読み飛ばし、空行や不正な単語がゲーム内の辞書に
    // 混入しないようにします。
    while (std::getline(file, line)) {
        const std::string word = normalizeDictionaryWord(line);

        if (!isValidDictionaryWord(word)) {
            continue;
        }
        words_.push_back(word);
        lookup_.insert(word);
    }
    if (words_.empty()) {
        throw std::runtime_error("dictionary is empty");
    }
}

bool Dictionary::contains(const std::string& word) const {
    return lookup_.find(word) != lookup_.end();
}

std::string Dictionary::chooseRandomWord() const {
    // The engine is kept static so repeated calls reuse the same generator
    // instead of reseeding every time.
    // 乱数エンジンを static にしているのは、呼び出しのたびに再シードせず、
    // 同じ生成器を再利用するためです。
    static std::random_device random_device;
    static std::mt19937 engine(random_device());
    std::uniform_int_distribution<std::size_t> distribution(0, words_.size() - 1);
    return words_[distribution(engine)];
}

std::string Dictionary::sampleWord() const {
    // A deterministic answer makes manual testing predictable.
    // 決定的な正解語にしておくと、手動テストの結果を予測しやすくなります。
    return "apple";
}
