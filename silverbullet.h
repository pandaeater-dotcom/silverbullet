// silverbullet.h
#pragma once

#include <memory>
#include <vector>
#include <map>
#include "itool.h"

using namespace std;

class SilverBullet {
private:
    map<string, unique_ptr<ITool>> tools;

    void registerTools();
    void helpCommand(const vector<string>& args);

public:
    SilverBullet();
    void run(int argc, char* argv[]);
};
