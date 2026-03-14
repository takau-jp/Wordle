#ifndef DICTIONARY_HPP
#define DICTIONARY_HPP

#include <string>
#include <unordered_set>
#include <vector>

class Dictionary {
public:
    explicit Dictionary(const std::string& path);

    void load();
    bool contains(const std::string& word) const;
    std::string chooseRandomWord() const;
    std::string sampleWord() const;

private:
    std::string path_;
    std::vector<std::string> words_;
    std::unordered_set<std::string> lookup_;
};

#endif
