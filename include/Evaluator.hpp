#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include <string>

#include "Types.hpp"

class Evaluator {
public:
    GuessResult evaluate(const std::string& guess, const std::string& answer) const;
};

#endif
