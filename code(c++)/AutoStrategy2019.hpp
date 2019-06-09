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
    const static int kCM2DotScale = 10;
    const static int kDotWidth = kCospaceWidth / kCM2DotScale;
    const static int kDotHeight = kCospaceHeight / kCM2DotScale;
    const static int kUSLimit = 186;

    const int kBorderSameObjNum = 2;
    const int kFindObjDuration = 45;

    int log_superobj_num, log_superobj_x[10], log_superobj_y[10];
    long same_time, super_sameoperate;
    int process;
    int searching_object;
    int prev_repeated_num;

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
    void GoToAngle(int angle, int distance);
    int GoToPosition(int x, int y, int wide_decide_x, int wide_decide_y, int wide_judge_arrived);
    void Dijkstra(void);
    void Astar(void);
    int GoToDot(int x, int y);
    long WhereIsMotor(void);
    long WhereIsColorSensor(void);
    void InputDotInformation(void);
    int GoToDots(int x, int y, int wide_decide_x, int wide_decide_y);
    int GoInDots(int x, int y, int wide_decide_x, int wide_decide_y, int color);
    int HowManyCurved(int id);
    int isNearTheFloor(MapInfo color, int x, int y, int radius);
};

#endif //!AUTO_STRATEGY_HPP
