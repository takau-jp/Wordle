#include "Dictionary.hpp"

#include <cctype>
#include <fstream>
#include <iterator>
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
    // Treat low-level stream corruption as exceptional. We still rely on the
    // normal loop condition for EOF, but a broken file stream should abort.
    // 低レベルのストリーム破損は例外として扱います。EOF は通常のループ条件で処理し、
    // 壊れたファイルストリームはその場で中断させます。
    file.exceptions(std::ifstream::badbit);

    // A single unordered_set is enough here:
    // - membership checks stay fast
    // - random selection is needed only once per run, so a linear advance
    //   over the set iterator is acceptable and avoids duplicating memory
    // 今回は unordered_set ひとつで十分です:
    // - 存在確認は引き続き高速に行える
    // - ランダム選択は1実行につき1回だけなので、set のイテレータを
    //   線形に進める方法でも十分で、メモリの二重保持を避けられる
    // Skip malformed lines so accidental blanks or bad tokens do not poison
    // the in-memory dictionary used by the game.
    // 不正な行は読み飛ばし、空行や不正な単語がゲーム内の辞書に
    // 混入しないようにします。
    while (std::getline(file, line)) {
        const std::string word = normalizeDictionaryWord(line);

        if (!isValidDictionaryWord(word)) {
            continue;
        }
        word_set_.insert(word);
    }
    if (!file.eof() && file.fail()) {
        throw std::runtime_error("failed while reading dictionary file");
    }
    if (words_.empty()) {
        throw std::runtime_error("dictionary is empty");
    }
}

bool Dictionary::contains(const std::string& word) const {
    return word_set_.find(word) != word_set_.end();
}

std::string Dictionary::chooseRandomWord() const {
    // Random selection happens once per program execution, so reseeding here
    // keeps the code simple without introducing a meaningful cost.
    // ランダム選択はプログラム実行ごとに1回だけなので、ここで再シードしても
    // 実用上のコストはほとんどなく、実装を単純に保てます。
    std::random_device random_device;
    std::mt19937 engine(random_device());
    std::uniform_int_distribution<std::size_t> distribution(0, word_set_.size() - 1);
    std::unordered_set<std::string>::const_iterator it = word_set_.begin();

    std::advance(it, static_cast<long>(distribution(engine)));
    return *it;
}

std::string Dictionary::sampleWord() const {
    // A deterministic answer makes manual testing predictable.
    // 決定的な正解語にしておくと、手動テストの結果を予測しやすくなります。
    return "apple";
}
