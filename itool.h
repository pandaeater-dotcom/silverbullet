// itool.h
#pragma once

#include <random>
#include <vector>
#include <string>

using namespace std;
class ITool {
public:
    virtual ~ITool() = default;
    virtual void execute(const vector<string>& args) = 0;
    virtual string getDescription() const = 0;
};
