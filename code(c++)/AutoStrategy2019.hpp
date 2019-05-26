#ifndef AUTO_STRATEGY2019_HPP
#define AUTO_STRATEGY2019_HPP

#include "CospaceSettings.hpp"
#include "CommonTools.hpp"

class AutoStrategy : public UserGame1
{
public:
    AutoStrategy();
    void setup() override;
    void loop() override;

private:
    const static int kCospaceWidth = 360;
    const static int kCospaceHeight = 270;
    const static int kCM2DotScale = 1;
    const static int kDotWidth = kCospaceWidth / kCM2DotScale;
    const static int kDotHeight = kCospaceHeight / kCM2DotScale;
    ProcessingTime pt;

    enum MapInfo
    {
        YELLOW = -3,
        WALL = -2,
        SWAMPLAND = -1,
        UNKNOWN = 0,
        WHITE = 1,
        DEPOSIT = 2,
        SUPER_AREA = 3
    };

    // 0:床情報 1:red 2:cyan 3:black
    char map[4][kDotHeight][kDotWidth];

    int log_x, log_y, log_compass;
    long now_dot_id, left_color_dot_id, right_color_dot_id;

    void CheckNowDot(long *left_dot_id, long *middle_dot_id, long *right_dot_id);
};

#endif //!AUTO_STRATEGY_HPP
