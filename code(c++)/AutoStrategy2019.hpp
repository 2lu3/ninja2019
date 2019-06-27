#ifndef AUTO_STRATEGY2019_HPP
#define AUTO_STRATEGY2019_HPP

#include "CospaceSettings.hpp"
#include "CommonTools.hpp"
#include <climits>

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
    const static int kCM2AreaScale = 30;
    const static int kDot2AreaScale = kCM2AreaScale / kCM2DotScale;
    const static int kAreaWidth = kCospaceWidth / kCM2AreaScale;
    const static int kAreaHeight = kCospaceHeight / kCM2AreaScale;

    const int kBorderSameObjNum = 2;
    const int kFindObjDuration = 45;
    const int kDepositObjDuration = 45;
    const int kProcessingNumOfOneThread = 90;
    const int kThreadNum = static_cast<int>((kDotHeight + kProcessingNumOfOneThread - 1) / kProcessingNumOfOneThread);

    int log_superobj_num, log_superobj_x[10], log_superobj_y[10];
    long same_time, super_sameoperate;
    int process;
    int searching_object;
    int prev_repeated_num;

    ProcessingTime pt;

    class CospaceMap
    {
    public:
        CospaceMap();
        enum MapInfo
        {
            MAP_ERROR = map_wall_index;
            MAP_YELLOW = -2,    // カラーセンサーの値によって決まる
            MAP_SWAMPLAND = -1, // カラーセンサーの値によって決まる
            MAP_UNKNOWN = 0,    // 不明な場合
            MAP_WALL = 1,
            MAP_WHITE = 2,      // カラーセンサーの値によって決まる
            MAP_DEPOSIT = 3,    // カラーセンサーの値によって決まる
            MAP_SUPER_AREA = 4, // カラーセンサーの値によって決まる
        };
        inline int setMapInfo(int x, int y, MapInfo info);
        inline int setMapInfo(int x, int y, MapInfo info, int times);
        inline MapInfo getMapInfo(int x, int y);
        inline int setMapObjInfo(int x, int y, int object_loaded_id);
        inline int setMapObjInfo(int x, int y, int object_loaded_id, int value);
        inline int getMapObjInfo(int x, int y, int object_loaded_id);
        inline void addMapArrivedTimes(int x, int y, int times);
        inline void addMapArrivedTimes(int x, int y);
        inline void setMapFrom(int x, int y, int from_x, int from_y);
        inline void getMapFrom(int x, int y, int *from_x, int *from_y);
        inline void setMapCost(int x, int y, int cost);
        inline int getMapCost(int x, int y);
        inline void setMapTotalCost(int x, int y, int cost);
        inline int getMapTotalCost(int x, int y);
        inline void setMapStatus(int x, int y, int status);
        inline int getMapStatus(int x, int y);
        const static int kSuccess = 0;
        const static int kFailure = INT_MIN;
        const static int kGuessedMapSize = 5;

    private:
        const static int map_wall_index = 4;
        // 0:床情報 1:red 2:cyan 3:black 4:壁情報
        int map[5][kDotHeight][kDotWidth];
        int map_arrived_times[kDotHeight][kDotWidth];
        int map_from[kDotHeight][kDotWidth][2];
        int map_cost[kDotHeight][kDotWidth];
        int map_total_cost[kDotHeight][kDotWidth];
        int map_status[kDotHeight][kDotWidth];
    };
    CospaceMap cospaceMap;

    int pos_x,
        pos_y,
        pos_compass;
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
    int isNearTheFloor(CospaceMap::MapInfo color, int x, int y, int cm_radius);
    void autoSearch(float parameter);
    void Astar(int x, int y);
    inline double sigmoid(double gain, double value, double scale);
    inline double sigmoid(double value, double scale);
    inline int i_sigmoid(double gain, double value, double scale);
    inline int i_sigmoid(double value, double scale);
};

#endif //!AUTO_STRATEGY_HPP
