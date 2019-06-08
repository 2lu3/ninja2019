#ifndef AUTO_STRATEGY2019_HPP
#define AUTO_STRATEGY2019_HPP

#include "CospaceSettings.hpp"
#include "CommonTools.hpp"

class AutoStrategy : public UserGame1
{
public:
    AutoStrategy();
    ~AutoStrategy() override;
    void setup() override;
    void loop() override;

private:
    const static int kCospaceWidth = 360;
    const static int kCospaceHeight = 270;
    const static int kCM2DotScale = 3;
    const static int kDotWidth = kCospaceWidth / kCM2DotScale;
    const static int kDotHeight = kCospaceHeight / kCM2DotScale;

    ProcessingTime pt;

    enum MapInfo
    {
        MAP_YELLOW = -3,
        MAP_WALL = -2,
        MAP_SWAMPLAND = -1,
        MAP_UNKNOWN = 0,
        MAP_WHITE = 1,
        MAP_DEPOSIT = 2,
        MAP_SUPER_AREA = 3
    };

    // 0:床情報 1:red 2:cyan 3:black
    char map[4][kDotHeight][kDotWidth];

    int log_x, log_y, log_compass;
    long robot_dot_positions[3][2];

    void CheckNowDot();
};

#endif //!AUTO_STRATEGY_HPP
