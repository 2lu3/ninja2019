#ifndef AUTO_STRATEGY2019_HPP
#define AUTO_STRATEGY2019_HPP

#include "CospaceSettings.hpp"
#include "CommonTools.hpp"

class AutoStrategy
{
public:
    AutoStrategy();
    void setup();
    void loop();

private:
    const static int kCospaceWidth = 270;
    const static int kCospaceHeight = 360;
    // 1:床情報 2:
    int map[4][kCospaceWidth][kCospaceHeight];

};

#endif //!AUTO_STRATEGY_HPP
