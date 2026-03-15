#ifndef DICTIONARY_HPP
#define DICTIONARY_HPP

#include <string>
#include <unordered_set>

class Dictionary {
public:
    explicit Dictionary(const std::string& path);

    void load();
    bool contains(const std::string& word) const;
    std::string chooseRandomWord() const;
    std::string sampleWord() const;

private:
    std::string path_;
    std::unordered_set<std::string> word_set_;
};

#endif
