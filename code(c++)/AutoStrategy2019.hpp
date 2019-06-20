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
    const static int kCM2DotScale = 5;
    const static int kDotWidth = kCospaceWidth / kCM2DotScale;
    const static int kDotHeight = kCospaceHeight / kCM2DotScale;
    const static int kUSLimit = 186;

    const int kBorderSameObjNum = 2;
    const int kFindObjDuration = 45;
    const int kProcessingNumOfOneThread = 10;
    const int kThreadNum = static_cast<int>((kDotHeight + kProcessingNumOfOneThread - 1) / kProcessingNumOfOneThread);

    int log_superobj_num, log_superobj_x[10], log_superobj_y[10];
    long same_time, super_sameoperate;
    int process;
    int searching_object;
    int prev_repeated_num;

    ProcessingTime pt;

    enum MapInfo
    {
        MAP_YELLOW = -3,    // カラーセンサーの値によって決まる
        MAP_WALL = -2,      // 超音波センサの値によってきまる
        MAP_SWAMPLAND = -1, // カラーセンサーの値によって決まる
        MAP_UNKNOWN = 0,    // 不明な場合
        MAP_UNKNOWN_NOT_WALL = 1,
        MAP_WHITE = 2,     // カラーセンサーの値によって決まる
        MAP_DEPOSIT = 3,   // カラーセンサーの値によって決まる
        MAP_SUPER_AREA = 4 // カラーセンサーの値によって決まる
    };

    // 0:床情報 1:red 2:cyan 3:black
    int map[4][kDotHeight][kDotWidth];
    int map_arrived_times[kDotHeight][kDotWidth];
    int map_from[kDotHeight][kDotWidth];
    int map_cost[kDotHeight][kDotWidth];
    int map_status[kDotHeight][kDotWidth];

    int log_x,
        log_y,
        log_compass;
    long robot_dot_positions[3][2];

    long now_dot_id;

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
    int isNearTheFloor(MapInfo color, int x, int y, int cm_radius);
    void autoSearch(float parameter);
};

#endif //!AUTO_STRATEGY_HPP
