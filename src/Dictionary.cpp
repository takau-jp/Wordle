#include "Dictionary.hpp"

#include <fstream>
#include <random>
#include <stdexcept>

Dictionary::Dictionary(const std::string& path) : path_(path) {}

void Dictionary::load() {
    std::ifstream file(path_.c_str());
    std::string word;

    if (!file) {
        throw std::runtime_error("failed to open dictionary file");
    }
    // Treat low-level stream corruption as exceptional. We still rely on the
    // normal loop condition for EOF, but a broken file stream should abort.
    // 低レベルのストリーム破損は例外として扱います。EOF は通常のループ条件で処理し、
    // 壊れたファイルストリームはその場で中断させます。
    file.exceptions(std::ifstream::badbit);
    // We keep both an ordered list and a hash set:
    // - vector: stable random access when picking an answer
    // - unordered_set: fast membership checks for player guesses
    // 順序付きリストとハッシュ集合の両方を保持します:
    // - vector: 正解語を選ぶときに安定したランダムアクセスができる
    // - unordered_set: プレイヤーの入力語が存在するかを高速に確認できる
    while (file >> word) {
        words_.push_back(word);
        lookup_.insert(word);
    }
    if (!file.eof() && file.fail()) {
        throw std::runtime_error("failed while reading dictionary file");
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
