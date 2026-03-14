#include <algorithm>
#include <cctype>
#include <cstdint>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <unordered_set>
#include <vector>

#define GUESS_LIMIT 6
#define WORD_LEN 5

bool isLowerWord(const std::string &s);
std::string selectRandomWord(const std::unordered_set<std::string> &word_set);
void showResult(const std::string &answer,
				const std::vector<std::string> &guesses);
void readGuess(const std::unordered_set<std::string> &word_set,
			   std::vector<std::string> &guesses);

int main(void)
{
	try
	{
		// words.txtを読み込む
		std::ifstream ifs("words.txt");
		if (!ifs.is_open())
			throw std::ios_base::failure("Could not open words.txt");
		std::unordered_set<std::string> word_set;
		std::string line;
		while (std::getline(ifs, line))
		{
			// すべて英小文字かつ5文字の行のみを追加する
			if (isLowerWord(line) && line.length() == WORD_LEN)
				word_set.insert(line);
		}
		if (!ifs.eof())
			throw std::ios_base::failure("Failed to read words.txt");
		// 空は成立しないのでエラー
		if (word_set.empty())
			throw std::runtime_error("words.txt doesn't have any valid words.");

		// 答えをランダムに選ぶ
		std::string answer = selectRandomWord(word_set);

		std::cout << "Total words available: " << word_set.size() << std::endl;
		std::cout << "ANSWER: " << answer << std::endl;

		std::vector<std::string> guesses;
		showResult(answer, guesses);
		for (int i = 0; i < GUESS_LIMIT; ++i)
		{
			readGuess(word_set, guesses);
			showResult(answer, guesses);
			if (guesses[i] == answer)
			{
				std::cout << "Congratulations you found the word " << answer
						  << " in " << i + 1 << " guesses" << std::endl;
				break;
			}
		}
	}
	catch (std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	catch (...)
	{
		std::cerr << "Error" << std::endl;
		return (1);
	}
	return (0);
}

bool isLowerWord(const std::string &s)
{
	return (std::all_of(s.begin(), s.end(),
						[](unsigned char c) { return (std::islower(c)); }));
}

std::string selectRandomWord(const std::unordered_set<std::string> &word_set)
{
	std::random_device seed_gen;
	std::uint32_t seed = seed_gen();
	std::mt19937 engine(seed);

	auto it = word_set.begin();
	std::advance(it, engine() % word_set.size());
	return (*it);
}

void showResult(const std::string &answer,
				const std::vector<std::string> &guesses)
{
	for (size_t i = 0; i < GUESS_LIMIT; ++i)
	{
		std::cout << "          ";
		if (i < guesses.size())
		{
			size_t j = 0;
			while (j < guesses[i].length() && j < WORD_LEN)
			{
				std::cout << " ";
				if (answer[j] == guesses[i][j])
					std::cout << "\033[0;92m" << guesses[i][j] << "\033[0;97m";
				else if (answer.find(guesses[i][j]) != std::string::npos)
					std::cout << "\033[0;93m" << guesses[i][j] << "\033[0;97m";
				else
					std::cout << guesses[i][j];
				++j;
			}
			while (j < WORD_LEN)
			{
				std::cout << " _";
				++j;
			}
		}
		else
		{
			std::cout << " _ _ _ _ _";
		}
		std::cout << std::endl << std::endl;
		if (!std::cout)
			throw std::ios_base::failure("Failed to output");
	}
}

void readGuess(const std::unordered_set<std::string> &word_set,
			   std::vector<std::string> &guesses)
{
	while (true)
	{
		std::cout << "\rinput: ";
		if (!std::cout)
			throw std::ios_base::failure("Failed to output");

		std::string input;
		if (!std::getline(std::cin, input))
		{
			if (std::cin.eof())
			{
				std::cout << std::endl;
				throw std::runtime_error("Unexpected EOF detected");
			}
			else
				throw std::ios_base::failure("Failed to input");
		}
		if (isLowerWord(input) && input.length() == WORD_LEN &&
			word_set.count(input))
		{
			guesses.push_back(input);
			return;
		}
	}
}
