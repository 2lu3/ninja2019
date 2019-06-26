#include "AutoStrategy2019.hpp"

#define TO_INT(VALUE) static_cast<int>((VALUE))
#define IF if
#define LOG_MESSAGE(MESSAGE, OPTION) \
    IF((OPTION) <= getRunMode()) { logErrorMessage.logMessage((MESSAGE), (OPTION)); }
#define ERROR_MESSAGE(MESSAGE, OPTION) \
    IF((OPTION) <= getRunMode()) { logErrorMessage.errorMessage((MESSAGE), (OPTION)); }

using std::cout;
using std::endl;
using std::extent;
using std::string;
using std::to_string;

AutoStrategy::~AutoStrategy()
{
    rep(i, TO_INT((extent<decltype(map), 0>::value)))
    {
        rep(j, TO_INT((extent<decltype(map), 1>::value)))
        {
            rep(k, TO_INT((extent<decltype(map), 2>::value)))
            {
                map[i][j][k] = 0;
            }
        }
    }
}

AutoStrategy::AutoStrategy()
{
    pt.start();
    // setRunMode(MODE_NORMAL);
    setRunMode(MODE_VERBOSE);
    setDefaultRunMode(MODE_NORMAL);
    setIsOutputLogMessage2Console(false);
    setIsOutputErrorMessage2Console(true);

    logErrorMessage.delErrorFile();
    logErrorMessage.delLogFile();
    logErrorMessage.delOutFile("out.txt");
    pt.print("AutoStrategy::AutoStrategy() :");
}

void AutoStrategy::setup()
{
    pt.start();
    UserGame1::setup();

    system("chcp 65001");
    resetLoadedObjects();

    cout << "run mode : " << TO_INT(getRunMode()) << endl;
    pt.print("AutoStrategy::setup() :");
}

void AutoStrategy::loop()
{
    pt.start();
    UserGame1::loop();
    LOG_MESSAGE("Auto Strategy loop start", MODE_DEBUG);

    // static int same_time = 0;
    // static int prev_repeated_num = 0;
    if (PositionX != 0 || PositionY != 0)
    {
        LOG_MESSAGE(FUNCNAME + "(): not in PLA and (x, y) is (" + to_string(PositionX) + ", " + to_string(PositionY) + ")", MODE_DEBUG);
        pos_x = PositionX;
        pos_y = PositionY;
        pos_compass = Compass;

        if (pos_x < 0)
            pos_x = 0;
        if (pos_x >= kCospaceWidth)
            pos_x = kCospaceWidth - 1;
        if (pos_y < 0)
            pos_y = 0;
        if (pos_y >= kCospaceHeight)
            pos_y = kCospaceHeight - 1;
        CheckNowDot();
        // arrived_timesを更新
        map[4][robot_dot_positions[0][1]][robot_dot_positions[0][0]]++;
        map[4][robot_dot_positions[2][1]][robot_dot_positions[2][0]]++;
        // int range = 0;
        // rep(hi, range * 2 + 1)
        // {
        //     rep(wj, range * 2 + 1)
        //     {
        //         // int y = hi + robot_dot_positions[1][1] - range;
        //         // int x = wj + robot_dot_positions[0][0] - range;
        //         // if (x < 0 || x >= kDotWidth || y < 0 || y >= kDotHeight)
        //         // {
        //         //     continue;
        //         // }
        //         // // dot[y * kDotWidth + x].arrived_times++;
        //     }
        // }
    }
    else
    {
        PositionX = -1;
        int range = 2;
        for (int wi = 0; wi < range * 2; wi++)
        {
            for (int hj = 0; hj < range * 2; hj++)
            {
                int x = pos_x / kCM2DotScale + wi - range;
                int y = pos_y / kCM2DotScale + hj - range;
                if (x < 0 || x >= kDotWidth || y < 0 || y >= kDotHeight)
                {
                    continue;
                }
                map[4][y][x] = map[4][y][x] + 2;
            }
        }
    }

    // 色のデータを保存
    {
        if (ColorJudgeLeft(object_box))
        {
            map[0][robot_dot_positions[0][1]][robot_dot_positions[0][0]] = MAP_DEPOSIT;
            LOG_MESSAGE("deposit " + to_string(robot_dot_positions[0][0] * kCM2DotScale) + " " + to_string(robot_dot_positions[0][1] * kCM2DotScale), MODE_VERBOSE);
        }
        else if (ColorJudgeLeft(trap_line))
        {
            map[0][robot_dot_positions[0][1]][robot_dot_positions[0][0]] = MAP_YELLOW;
            LOG_MESSAGE("yellow " + to_string(robot_dot_positions[0][0] * kCM2DotScale) + " " + to_string(robot_dot_positions[0][1] * kCM2DotScale), MODE_VERBOSE);
        }
        else if (ColorJudgeLeft(gray_zone))
        {
            for (int yi = robot_dot_positions[0][1] - TO_INT(kGuessedMapSize / kCM2DotScale); yi <= robot_dot_positions[0][1] + TO_INT(kGuessedMapSize / kCM2DotScale); ++yi)
            {
                if (yi < 0 || yi >= kDotHeight)
                {
                    continue;
                }
                for (int xj = robot_dot_positions[0][0] - TO_INT(kGuessedMapSize / kCM2DotScale); xj <= robot_dot_positions[0][0] + TO_INT(kGuessedMapSize / kCM2DotScale); ++xj)
                {
                    if (xj < 0 || xj >= kDotWidth)
                    {
                        continue;
                    }
                    if (map[0][yi][xj] == MAP_UNKNOWN || map[0][yi][xj] == MAP_UNKNOWN_NOT_WALL)
                    {
                        map[0][yi][xj] = MAP_SWAMPLAND;
                        LOG_MESSAGE("swampland " + to_string(xj) + " " + to_string(yi), MODE_VERBOSE);
                    }
                }
            }
        }
        else if (ColorJudgeLeft(blue_zone))
        {
            map[0][robot_dot_positions[0][1]][robot_dot_positions[0][0]] = MAP_SUPER_AREA;
            LOG_MESSAGE("blue floor " + to_string(robot_dot_positions[0][0]) + " " + to_string(robot_dot_positions[0][1]), MODE_VERBOSE);
        }
        else if (ColorJudgeLeft(black_obj))
        {
            for (int yi = robot_dot_positions[0][1] - TO_INT(kGuessedMapSize / kCM2DotScale); yi <= robot_dot_positions[0][1] + TO_INT(kGuessedMapSize / kCM2DotScale); ++yi)
            {
                if (yi < 0 || yi >= kDotHeight)
                {
                    continue;
                }
                for (int xj = robot_dot_positions[0][0] - TO_INT(kGuessedMapSize / kCM2DotScale); xj <= robot_dot_positions[0][0] + TO_INT(kGuessedMapSize / kCM2DotScale); ++xj)
                {
                    if (xj < 0 || xj >= kDotWidth)
                    {
                        continue;
                    }

                    map[BLACK_LOADED_ID][robot_dot_positions[0][1]][robot_dot_positions[0][0]]++;
                    LOG_MESSAGE("black obj " + to_string(robot_dot_positions[0][0]) + " " + to_string(robot_dot_positions[0][1]), MODE_VERBOSE);
                }
            }
        }
        else if (ColorJudgeLeft(cyan_obj))
        {
            for (int yi = robot_dot_positions[0][1] - TO_INT(kGuessedMapSize / kCM2DotScale); yi <= robot_dot_positions[0][1] + TO_INT(kGuessedMapSize / kCM2DotScale); ++yi)
            {
                if (yi < 0 || yi >= kDotHeight)
                {
                    continue;
                }
                for (int xj = robot_dot_positions[0][0] - TO_INT(kGuessedMapSize / kCM2DotScale); xj <= robot_dot_positions[0][0] + TO_INT(kGuessedMapSize / kCM2DotScale); ++xj)
                {
                    if (xj < 0 || xj >= kDotWidth)
                    {
                        continue;
                    }
                    map[CYAN_LOADED_ID][robot_dot_positions[2][1]][robot_dot_positions[2][0]]++;
                    LOG_MESSAGE("cyan obj " + to_string(robot_dot_positions[2][0]) + " " + to_string(robot_dot_positions[2][1]), MODE_VERBOSE);
                }
            }
        }
        else if (ColorJudgeLeft(red_obj))
        {
            for (int yi = robot_dot_positions[0][1] - TO_INT(kGuessedMapSize / kCM2DotScale); yi <= robot_dot_positions[0][1] + TO_INT(kGuessedMapSize / kCM2DotScale); ++yi)
            {
                if (yi < 0 || yi >= kDotHeight)
                {
                    continue;
                }
                for (int xj = robot_dot_positions[0][0] - TO_INT(kGuessedMapSize / kCM2DotScale); xj <= robot_dot_positions[0][0] + TO_INT(kGuessedMapSize / kCM2DotScale); ++xj)
                {
                    if (xj < 0 || xj >= kDotWidth)
                    {
                        continue;
                    }

                    map[RED_LOADED_ID][robot_dot_positions[2][1]][robot_dot_positions[2][0]]++;
                    LOG_MESSAGE("red obj " + to_string(robot_dot_positions[2][0]) + " " + to_string(robot_dot_positions[2][1]), MODE_VERBOSE);
                }
            }
        }
        else if (ColorJudgeLeft(white_zone))
        {
            map[0][robot_dot_positions[0][1]][robot_dot_positions[0][0]] = MAP_WHITE;
            LOG_MESSAGE("white area " + to_string(robot_dot_positions[0][0]) + " " + to_string(robot_dot_positions[0][1]), MODE_VERBOSE);
        }
        else
        {
            map[0][robot_dot_positions[0][1]][robot_dot_positions[0][0]] = MAP_WHITE;
            LOG_MESSAGE("white area(maye be) " + to_string(robot_dot_positions[0][0]) + " " + to_string(robot_dot_positions[0][1]), MODE_VERBOSE);
        }

        if (ColorJudgeRight(object_box))
        {
            map[0][robot_dot_positions[2][1]][robot_dot_positions[2][0]] = MAP_DEPOSIT;
            LOG_MESSAGE("deposit " + to_string(robot_dot_positions[2][0]) + " " + to_string(robot_dot_positions[2][1]), MODE_VERBOSE);
        }
        else if (ColorJudgeRight(trap_line))
        {
            map[0][robot_dot_positions[2][1]][robot_dot_positions[2][0]] = MAP_YELLOW;
            LOG_MESSAGE(to_string(robot_dot_positions[2][0]) + " " + to_string(robot_dot_positions[2][1]), MODE_VERBOSE);
        }
        else if (ColorJudgeRight(gray_zone))
        {
            for (int yi = robot_dot_positions[2][1] - TO_INT(kGuessedMapSize / kCM2DotScale); yi <= robot_dot_positions[2][1] + TO_INT(kGuessedMapSize / kCM2DotScale); ++yi)
            {
                if (yi < 0 || yi >= kDotHeight)
                {
                    continue;
                }
                for (int xj = robot_dot_positions[2][0] - TO_INT(kGuessedMapSize / kCM2DotScale); xj <= robot_dot_positions[2][0] + TO_INT(kGuessedMapSize / kCM2DotScale); ++xj)
                {
                    if (xj < 0 || xj >= kDotWidth)
                    {
                        continue;
                    }
                    if (map[0][yi][xj] == MAP_UNKNOWN || map[0][yi][xj] == MAP_UNKNOWN_NOT_WALL)
                    {
                        map[0][yi][xj] = MAP_SWAMPLAND;
                        LOG_MESSAGE("swampland " + to_string(xj) + " " + to_string(yi), MODE_VERBOSE);
                    }
                }
            }
        }
        else if (ColorJudgeRight(blue_zone))
        {
            map[0][robot_dot_positions[2][1]][robot_dot_positions[2][0]] = MAP_SUPER_AREA;
            LOG_MESSAGE("blue floor " + to_string(robot_dot_positions[2][0]) + " " + to_string(robot_dot_positions[2][1]), MODE_VERBOSE);
        }
        else if (ColorJudgeRight(black_obj))
        {
            for (int yi = robot_dot_positions[2][1] - TO_INT(kGuessedMapSize / kCM2DotScale); yi <= robot_dot_positions[2][1] + TO_INT(kGuessedMapSize / kCM2DotScale); ++yi)
            {
                if (yi < 0 || yi >= kDotHeight)
                {
                    continue;
                }
                for (int xj = robot_dot_positions[2][0] - TO_INT(kGuessedMapSize / kCM2DotScale); xj <= robot_dot_positions[2][0] + TO_INT(kGuessedMapSize / kCM2DotScale); ++xj)
                {
                    if (xj < 0 || xj >= kDotWidth)
                    {
                        continue;
                    }
                    map[BLACK_LOADED_ID][robot_dot_positions[2][1]][robot_dot_positions[2][0]]++;
                    LOG_MESSAGE("black obj " + to_string(robot_dot_positions[2][0]) + " " + to_string(robot_dot_positions[2][1]), MODE_VERBOSE);
                }
            }
        }
        else if (ColorJudgeRight(cyan_obj))
        {
            for (int yi = robot_dot_positions[2][1] - TO_INT(kGuessedMapSize / kCM2DotScale); yi <= robot_dot_positions[2][1] + TO_INT(kGuessedMapSize / kCM2DotScale); ++yi)
            {
                if (yi < 0 || yi >= kDotHeight)
                {
                    continue;
                }
                for (int xj = robot_dot_positions[2][0] - TO_INT(kGuessedMapSize / kCM2DotScale); xj <= robot_dot_positions[2][0] + TO_INT(kGuessedMapSize / kCM2DotScale); ++xj)
                {
                    if (xj < 0 || xj >= kDotWidth)
                    {
                        continue;
                    }
                    map[CYAN_LOADED_ID][robot_dot_positions[2][1]][robot_dot_positions[2][0]]++;
                    LOG_MESSAGE("cyan obj " + to_string(robot_dot_positions[2][0]) + " " + to_string(robot_dot_positions[2][1]), MODE_VERBOSE);
                }
            }
        }
        else if (ColorJudgeRight(red_obj))
        {
            for (int yi = robot_dot_positions[2][1] - TO_INT(kGuessedMapSize / kCM2DotScale); yi <= robot_dot_positions[2][1] + TO_INT(kGuessedMapSize / kCM2DotScale); ++yi)
            {
                if (yi < 0 || yi >= kDotHeight)
                {
                    continue;
                }
                for (int xj = robot_dot_positions[2][0] - TO_INT(kGuessedMapSize / kCM2DotScale); xj <= robot_dot_positions[2][0] + TO_INT(kGuessedMapSize / kCM2DotScale); ++xj)
                {
                    if (xj < 0 || xj >= kDotWidth)
                    {
                        continue;
                    }
                    map[RED_LOADED_ID][robot_dot_positions[2][1]][robot_dot_positions[2][0]]++;
                    LOG_MESSAGE("red obj " + to_string(robot_dot_positions[2][0]) + " " + to_string(robot_dot_positions[2][1]), MODE_VERBOSE);
                }
            }
        }
        else if (ColorJudgeRight(white_zone))
        {
            map[0][robot_dot_positions[2][1]][robot_dot_positions[2][0]] = MAP_WHITE;
            LOG_MESSAGE("white area " + to_string(robot_dot_positions[2][0]) + " " + to_string(robot_dot_positions[2][1]), MODE_VERBOSE);
        }
        else
        {
            map[0][robot_dot_positions[2][1]][robot_dot_positions[2][0]] = MAP_WHITE;
            LOG_MESSAGE("white area(may be) " + to_string(robot_dot_positions[2][0]) + " " + to_string(robot_dot_positions[2][1]), MODE_VERBOSE);
        }
    }

    // 壁の位置を計算
    {
        LOG_MESSAGE(FUNCNAME + "():" + "壁の位置の計算を開始", MODE_DEBUG);

        // 0: left & right 1: front
        int difference_us_position[2] = {9, 9};
        int us_sensors[3] = {US_Left, US_Front, US_Right};
        LOG_MESSAGE(FUNCNAME + "(): " + "US " + to_string(US_Left) + " " + to_string(US_Front) + " " + to_string(US_Right), MODE_DEBUG);
        string us_names[3] = {"Left", "Front", "Right"};
        int angles[3] = {40, 0, -40};
        int calculated_relative_coordinate[3][2];
        int calculated_absolute_dot_position[3][2];
        for (int i = 0; i < 3; ++i)
        {
            angles[i] += Compass + 90;
            angles[i] %= 360;
            if (us_sensors[i] > kUSLimit - 1)
            {
                us_sensors[i] = kUSLimit;
            }
            us_sensors[i] += difference_us_position[i % 2];
            // 壁の位置とロボットの相対座標
            calculated_relative_coordinate[i][0] = TO_INT(cos(angles[i] * M_PI / 180) * us_sensors[i]);
            calculated_relative_coordinate[i][1] = TO_INT(sin(angles[i] * M_PI / 180) * us_sensors[i]);
            LOG_MESSAGE(FUNCNAME + "(): calculated relative coordinate (" + to_string(calculated_relative_coordinate[i][0]) + ", " + to_string(calculated_relative_coordinate[i][1]) + ")", MODE_VERBOSE);

            // ドット上での壁の絶対座標
            calculated_absolute_dot_position[i][0] = TO_INT((pos_x + calculated_relative_coordinate[i][0] + kCM2DotScale / 2) / kCM2DotScale);
            calculated_absolute_dot_position[i][1] = TO_INT((pos_y + calculated_relative_coordinate[i][1] + kCM2DotScale / 2) / kCM2DotScale);
                if (0 <= calculated_absolute_dot_position[i][0] && calculated_absolute_dot_position[i][0] kDotWidth || calculated_absolute_dot_position[i][1] < 0 || calculated_absolute_dot_position[i][1] >= kDotHeight)
                {
                    continue;
                }
                // 壁はないので、MAP_WALLを登録する必要がない
                if (us_sensors[i] >= kUSLimit + difference_us_position[i % 2])
                {
                    continue;
                }
                if (map[0][calculated_absolute_dot_position[i][1]][calculated_absolute_dot_position[i][0]] == MAP_UNKNOWN)
                {
                    map[0][calculated_absolute_dot_position[i][1]][calculated_absolute_dot_position[i][0]] = MAP_WALL;
                    LOG_MESSAGE(FUNCNAME + "(): set here as Wall; pos: " + to_string(calculated_absolute_dot_position[i][0] * kCM2DotScale) + "," + to_string(calculated_absolute_dot_position[i][1] * kCM2DotScale), MODE_VERBOSE);
                    cout << "wall " << to_string(calculated_absolute_dot_position[i][0] * kCM2DotScale) + "," + to_string(calculated_absolute_dot_position[i][1] * kCM2DotScale) << endl;
                }
            // x[0], y[0] -> x[1], y[1]までMAP_WALLをMAP_WHITEに変更する
            // 壁の位置(壁から多少離れた位置)とロボットそれぞれの絶対座標
            // 基本的に、実際の壁との距離から0.7倍程度にするが、kCM2DotScaleが最低2つはあけないといけない
            // 1cm先に壁がある場合、MAP_WHITEは登録しない
            const int kRange4Wall = 10;
            int x[2], y[2];
            x[0] = robot_dot_positions[1][0];
            y[0] = robot_dot_positions[1][1];
            if (abs(calculated_relative_coordinate[i][0]) * 0.3 < kRange4Wall)
            {
                // 壁との距離が非常に近いとき
                if(abs(calculated_relative_coordinate[i][0]) < kRange4Wall) {
                    continue;
                }
                else {

                }
            }
            const int x[2] = {
                robot_dot_positions[1][0], TO_INT(((pos_x + (abs(calculated_relative_coordinate[i][0]) * 0.3 < kCM2DotScale * 2 ? calculated_relative_coordinate[1][0] - kCM2DotScale * 2 : calculated_relative_coordinate[i][0] * 0.7) + kCM2DotScale / 2) / kCM2DotScale))};
            const int y[2] = {
                robot_dot_positions[1][1], TO_INT(((pos_y + (calculated_relative_coordinate[i][1] * 0.3 < kCM2DotScale * 2 ? calculated_relative_coordinate[i][1] - kCM2DotScale * 2 : calculated_relative_coordinate[i][1] * 0.7) + kCM2DotScale / 2) / kCM2DotScale))};
            LOG_MESSAGE(FUNCNAME + "(): calculated wall position us: " + us_names[i] + " pos: " + to_string(pos_x + calculated_relative_coordinate[i][0]) + "," + to_string(pos_y + calculated_relative_coordinate[i][1]) + " registered pos:" + to_string(calculated_absolute_dot_position[i][0] * kCM2DotScale) + "," + to_string(calculated_absolute_dot_position[i][1] * kCM2DotScale), MODE_VERBOSE);
            // if()

            // (x[0], y[0]) -> (x[1], y[1])まで、MAP_WALLをMAP_UNKNOWN_NOT_WALLに変更する
            if (x[0] == x[1]) // 縦方向の直線の場合
            {
                if (0 <= x[0] && x[0] < kDotWidth)
                {
                    int y_start = y[0], y_end = y[1];
                    // 上から変更しようと、下から変更しようと変わらない
                    if (y[0] > y[1])
                    {
                        y_start = y[1];
                        y_end = y[0];
                    }
                    for (int yi = y_start; yi <= y_end; ++yi)
                    {
                        if (yi < 0)
                        {
                            yi = -1;
                            continue;
                        }
                        if (yi >= kDotHeight)
                        {
                            break;
                        }
                        if (map[0][yi][x[0]] == MAP_WALL)
                        {
                            map[0][yi][x[0]] = MAP_UNKNOWN_NOT_WALL;
                            LOG_MESSAGE(FUNCNAME + "(): set here as white(not wall) " + to_string(x[0] * kCM2DotScale) + ", " + to_string(yi * kCM2DotScale), MODE_VERBOSE);
                        }
                    }
                }
            }
            else
            {
                // x[0]<x[1]の場合、x[1]<x[0]の場合、両方ともtiltは、正常な傾きを表す
                float tilt = static_cast<float>(y[1] - y[0]) / static_cast<float>(x[1] - x[0]);
                LOG_MESSAGE(FUNCNAME + "(): tilt is " + to_string(tilt), MODE_VERBOSE);
                // x[0]>x[1]は、入れ替えるだけ
                int x_start = x[0], x_end = x[1];

                if (x_start > x_end)
                {
                    x_start = x[1];
                    x_end = x[0];
                }
                for (int xi = x_start; xi < x_end; ++xi)
                {
                    if (xi < 0)
                    {
                        xi = -1;
                        continue;
                    }
                    if (xi >= kDotWidth)
                    {
                        break;
                    }
                    int y_start = x_start + TO_INT(tilt * static_cast<float>(xi - x_start));
                    int y_end = x_start + TO_INT(floor(tilt * (static_cast<float>(xi + 1 - x_start))));
                    if (y_start > y_end)
                    {
                        int temp = y_start;
                        y_start = y_end;
                        y_end = temp;
                    }
                    for (int yj = y_start; yj <= y_end; ++yj)
                    {
                        if (yj < 0)
                        {
                            yj = -1;
                            continue;
                        }
                        if (yj >= kDotHeight)
                        {
                            break;
                        }
                        if (map[0][yj][xi] == MAP_WALL)
                        {
                            map[0][yj][xi] = MAP_UNKNOWN_NOT_WALL;
                            LOG_MESSAGE(FUNCNAME + "(): set here as white(not wall) space (" + to_string(xi) + ", " + to_string(yj) + ")", MODE_VERBOSE);
                        }
                    }
                }

            }
        }
        // // 0 < 1にする
        // if (x[0] > x[1])
        // {
        //     // x[0]とx[1]を入れ替え
        //     int temp = x[0];
        //     x[0] = x[1];
        //     x[1] = temp;
        // }
        // if (y[0] > y[1])
        // {
        //     // y[0]とy[1]を入れ替え
        //     int temp = y[0];
        //     y[0] = y[1];
        //     y[1] = temp;
        // }

        // // 傾き
        // float tilt;
        // if (calculated_relative_coordinate[i][0] == 0)
        // {
        //     tilt = 1000000000;
        // }
        // else
        // {
        //     tilt = fabs(static_cast<float>(calculated_relative_coordinate[i][1] / calculated_relative_coordinate[i][0]));
        // }

        // // x[0] -> x[1]まで、順番にyの値を調べ、それぞれのドットにMAP_WHITEを代入していく
        // // ただし、x[0]とx[1]はMAP_WHITEを代入しない
        // // x[0]かx[1]のうちどちらかは壁である
        // // map[0][y][x] = MAP_WALLできるのは、map[0][y][x] == MAP_UNKNOWNのときだけ
        // for (int xi = x[0] + 1; xi < x[1]; ++xi)
        // {
        //     // LOG_MESSAGE(FUNCNAME + "()")
        //     if (xi < 0)
        //     {
        //         continue;
        //     }
        //     if (xi >= kDotWidth)
        //     {
        //         break;
        //     }
        //     for (int yj = TO_INT(static_cast<float>(xi - x[0]) * tilt) + y[0]; static_cast<float>(yj - y[0]) <= static_cast<float>(xi - x[0] + 1) * tilt; ++yj)
        //     {
        //         if (yj < 0)
        //         {
        //             continue;
        //         }
        //         if (yj >= kDotHeight)
        //         {
        //             break;
        //         }
        //         if (map[0][yj][xi] == MAP_WALL)
        //         {
        //             map[0][yj][xi] = MAP_UNKNOWN_NOT_WALL;
        //             LOG_MESSAGE(FUNCNAME + "(): set here as unknow space; pos:" + to_string(xi * kCM2DotScale) + "," + to_string(yj * kCM2DotScale), MODE_VERBOSE);
        //         }
        //     }
        // }

        // }
    }

    // スレッドを使って、整形
    {
        LOG_MESSAGE(FUNCNAME + "(): データの整形を開始", MODE_DEBUG);
        int start_len = (getRepeatedNum() % kThreadNum) * kProcessingNumOfOneThread;
        int end_len = start_len + kProcessingNumOfOneThread;
        if (end_len > kDotHeight)
        {
            end_len = kDotHeight;
        }
        for (int yi = start_len; yi < end_len; ++yi)
        {
            if (yi < 0 || yi >= kDotHeight)
            {
                ERROR_MESSAGE(FUNCNAME + "(): thread yi is abnormal; " + to_string(yi), MODE_NORMAL);
            }
            if (yi != 0 && yi != kDotHeight - 1)
            {
                int color_id[20];
                char color_num[20];
                int color_pointer = 0; // 最大値18(=20-2)
                int color_pointer_limit = 18;
                for (int xj = 1; xj < kDotWidth - 1; ++xj)
                {
                    color_pointer = 0;
                    // そのドットの周りで一番多い床の色を探す
                    for (int y = yi - 1; y <= yi + 1; ++y)
                    {
                        for (int x = xj - 1; x <= xj + 1; ++x)
                        {
                            if (map[0][y][x] == static_cast<char>(MAP_UNKNOWN))
                            {
                                continue;
                            }
                            color_id[color_pointer] = map[0][y][x];
                            rep(i, color_pointer + 1)
                            {
                                if (color_id[i] == map[0][y][x])
                                {
                                    if (i == color_pointer)
                                    {
                                        // 新しい色を追加
                                        if (color_pointer < color_pointer_limit)
                                        {
                                            color_id[color_pointer] = map[0][y][x];
                                            color_num[color_pointer] = 1;
                                            color_pointer++;
                                        }
                                        else
                                        {
                                            ERROR_MESSAGE(FUNCNAME + "(): error; To fix the floor map, the array num is over " + to_string(color_pointer_limit), MODE_NORMAL);
                                        }
                                    }
                                    else
                                    {
                                        // 色の数を++
                                        color_num[i]++;
                                    }
                                    break;
                                }
                            }
                        }
                    }

                    int max_num = 0, max_id = -1;
                    rep(i, color_pointer)
                    {
                        if (color_num[i] > max_num)
                        {
                            max_num = color_num[i];
                            max_id = color_id[i];
                        }
                    }

                    if (max_id == -1)
                    {
                        // ERROR_MESSAGE(FUNCNAME + "(): error; color_num has fatal error", MODE_NORMAL);
                        continue;
                    }

                    if (max_num >= 6)
                    {
                        map[0][yi][xj] = max_id;
                    }
                    // else if (max_num == 3)
                    // {
                    //     // 64128256
                    //     //  8 16 32
                    //     //  1  2  4
                    //     int sum = 0;
                    //     for (int y = yi - 1; y <= yi + 1; ++y)
                    //     {
                    //         for (int x = xj - 1; x <= xj + 1; ++x)
                    //         {
                    //             if (map[0][y][x] == max_id && (x != 0 || y != 0))
                    //             {
                    //                 sum += TO_INT(pow(2, (yi - y + 1) * 3 + (xj - x + 1)));
                    //             }
                    //         }
                    //     }
                    //     if (sum != 1 + 8 + 64 && sum != 1 + 2 + 4 && sum != 4 + 32 + 256 && sum != 64 + 128 + 256)
                    //     {
                    //         // 一列になっていないので、変更する
                    //         map[0][yi][xj] = static_cast<char>(max_id);
                    //     }
                    // }
                    // else
                    // {
                    //     int pos_x, pos_y, flag = 0;
                    //     for (int y = yi - 1; y <= yi + 1; ++y)
                    //     {
                    //         for (int x = xj - 1; x <= xj + 1; ++x)
                    //         {
                    //             if (map[0][y][x] == max_id)
                    //             {
                    //                 if (flag == 0)
                    //                 {
                    //                     pos_x = x;
                    //                     pos_y = y;
                    //                 }
                    //                 else
                    //                 {
                    //                     if (pos_x != x && pos_y != y)
                    //                     {
                    //                         // 縦横が重なっていないので、変更する
                    //                         map[0][yi][xj] = static_cast<char>(max_id);
                    //                     }
                    //                 }
                    //             }
                    //         }
                    //     }
                    // }
                }

                // xj == 0の場合 上下の比較のみおこなう
                int temp_map_color = map[0][yi - 1][0];
                if (temp_map_color == map[0][yi + 1][0])
                {
                    if (temp_map_color != static_cast<char>(MAP_UNKNOWN))
                    {
                        map[0][yi][0] = temp_map_color;
                    }
                }
                for (int xj = 1; xj < kDotWidth - 1; ++xj)
                {

                    // 左右の比較
                    if (map[0][yi][xj - 1] == map[0][yi][xj + 1])
                    {
                        if (map[0][yi][xj + 1] != static_cast<char>(MAP_UNKNOWN))
                        {
                            map[0][yi][xj] = map[0][yi][xj + 1];
                        }
                    }
                    // 上下の比較
                    else if (map[0][yi - 1][xj] == map[0][yi + 1][xj])
                    {
                        if (map[0][yi + 1][xj] != static_cast<char>(MAP_UNKNOWN))
                        {
                            map[0][yi][xj] = map[0][yi + 1][xj];
                        }
                    }
                }
                // xj == kDotWidth - 1 の場合、上下の比較のみ行う
                temp_map_color = map[0][yi - 1][kDotWidth - 1];
                if (temp_map_color == map[0][yi + 1][kDotWidth - 1])
                {
                    if (temp_map_color != static_cast<char>(MAP_UNKNOWN))
                    {
                        map[0][yi][kDotWidth - 1] = temp_map_color;
                    }
                }
            }
            else
            {
                // 左右の比較しか行わない
                for (int xj = 1; xj < kDotWidth - 1; ++xj)
                {
                    // 左右の比較
                    if (map[0][yi][xj - 1] == map[0][yi][xj + 1])
                    {
                        if (map[0][yi][xj + 1] != MAP_UNKNOWN)
                        {
                            map[0][yi][xj] = map[0][yi][xj + 1];
                        }
                    }
                }
            }
        }
        LOG_MESSAGE(FUNCNAME + "(): データの整形終了", MODE_DEBUG)
    }

    // データの出力
    if (getRepeatedNum() == 60 * 1000 / 60)
    {
        cout << "output!" << endl;
        logErrorMessage.outputData("out.txt", "\n");
        rep(i, 5)
        {
            rep(yi, kDotHeight)
            {
                rep(xj, kDotWidth)
                {
                    logErrorMessage.outputData("out.txt", to_string(map[i][kDotHeight - 1 - yi][xj]));
                }
                logErrorMessage.outputData("out.txt", "\n");
            }
            logErrorMessage.outputData("out.txt", "\n");
            logErrorMessage.outputData("out.txt", "\n");
        }
        logErrorMessage.outputData("out.txt", "\n");
        cout << "output! finished" << endl;
    }

    if (SuperDuration > 0)
    {
        --SuperDuration;
    }
    else if (IsOnBlackObj() && loaded_objects[BLACK_LOADED_ID] < kBorderSameObjNum)
    {
        setAction(FIND_OBJ);
        SuperDuration = kFindObjDuration;
        ++loaded_objects[BLACK_LOADED_ID];
        LOG_MESSAGE("Find Black Object!", MODE_DEBUG);
    }
    else if (IsOnCyanObj() && loaded_objects[CYAN_LOADED_ID] < kBorderSameObjNum)
    {
        setAction(FIND_OBJ);
        SuperDuration = kFindObjDuration;
        ++loaded_objects[CYAN_LOADED_ID];
        LOG_MESSAGE("Find Cyan Object!", MODE_DEBUG);
    }
    else if (IsOnRedObj() && loaded_objects[RED_LOADED_ID] < kBorderSameObjNum)
    {
        setAction(FIND_OBJ);
        SuperDuration = kFindObjDuration;
        ++loaded_objects[RED_LOADED_ID];
        LOG_MESSAGE("Find Red Object!", MODE_DEBUG);
    }
    else if (IsOnDepositArea() && LoadedObjects >= 5)
    {
        switch (IsOnDepositArea())
        {
        case 1:
            motor(5, 3);
            break;
        case 2:
            motor(3, 5);
            break;
        case 3:
            setAction(DEPOSIT_OBJ);
            SuperDuration = kDepositObjDuration;
            break;
        default:
            ERROR_MESSAGE(FUNCNAME + "(): switch(IsOnDepositArea) value is " + to_string(IsOnDepositArea()), MODE_NORMAL);
            break;
        }
    }
    else if (Duration > 0)
    {
        --Duration;
    }
    else if (IsOnYellowLine())
    {
        setAction(YELLOW_AVOIDANCE);
        Duration = 5;
    }
    else
    {
        autoSearch(0);
    }
    // GoToDot(200 / kCM2DotScale, 150 / kCM2DotScale);

    switch (TO_INT(getAction()))
    {
    case DEFINED:
        break;
    case YELLOW_AVOIDANCE:
        motor(-5, -3);
        break;
    case FIND_OBJ:
        if (Duration == kFindObjDuration || SuperDuration == kFindObjDuration)
        {
            LOG_MESSAGE("Add 1 to LoadedObjects", MODE_VERBOSE);
            LoadedObjects++;
        }
        LED_1 = 1;
        MyState = 0;
        motor_no_action_change(0, 0);
        if (Duration == 0 && SuperDuration == 0)
        {
            LED_1 = 0;
        }
        break;
        break;
    case DEPOSIT_OBJ:
        motor_no_action_change(0, 0);
        LED_1 = 2;
        MyState = 0;
        LoadedObjects = 0;
        // loaded_objects全体の大きさ / loaded_objects[0]の大きさ
        resetLoadedObjects();

        motor_no_action_change(0, 0);

        if (Duration == 0 && SuperDuration == 0)
        {
            LED_1 = 0;
        }
        else if (!BothColorJudge(object_box))
        {
            ERROR_MESSAGE("this robot is not in object_box", MODE_NORMAL);
            LoadedObjects = 6;
            Duration = 0;
            for (int i = 0; i < 3; i++)
            {
                loaded_objects[i] = 2;
            }
        }
        break;
    default:
        ERROR_MESSAGE(FUNCNAME + "(): action's value is ubnormal : " + to_string(TO_INT(getAction())), MODE_NORMAL);
        break;
    }

    pt.print("AutoStrategy loop time :");
}

void AutoStrategy::CheckNowDot()
{
    // 左のカラーセンサ、右のカラーセンサ、座標の中心点をそれぞれA,B,Cとすると、
    // 三角形ABCは、一辺5-6の正三角形
    // x,yはそれぞれ左のカラーセンサ、座標センサの場所、右のカラーセンサの場所のx,y座標
    int side = 6;
    int x[3] = {
        TO_INT(pos_x + cos((Compass + 90 + 30) * M_PI / 180) * side),
        pos_x,
        TO_INT(pos_x + cos((Compass + 90 - 30) * M_PI / 180) * side)};
    int y[3] = {
        TO_INT(pos_y + sin((Compass + 90 + 30) * M_PI / 180) * side),
        pos_y,
        TO_INT(pos_y + sin((Compass + 90 - 30) * M_PI / 180) * side)};

    rep(i, 3)
    {
        // 範囲外の場合
        if (x[i] < 0)
            x[i] = 0;
        if (x[i] >= kCospaceWidth)
            x[i] = kCospaceWidth - 1;
        if (y[i] < 0)
            y[i] = 0;
        if (y[i] >= kCospaceHeight)
            y[i] = kCospaceHeight - 1;

        x[i] /= kCM2DotScale;
        y[i] /= kCM2DotScale;
        if (x[i] < 0)
        {
            x[i] = 0;
        }
        if (x[i] >= kDotWidth)
        {
            x[i] = kDotWidth - 1;
        }
        if (y[i] < 0)
        {
            y[i] = 0;
        }
        if (y[i] >= kDotHeight)
        {
            y[i] = kDotHeight - 1;
        }
        // そのドットが壁の場合
        if (map[0][y[i]][x[i]] == MAP_WALL)
        {
            // 近くのドットの中で、壁の中ではないドットにする
            int range = (10 + kCM2DotScale - 1) / kCM2DotScale;
            int min_position[2] = {-1, -1};
            int min_value = 1000000;
            for (int hi = -range; hi <= range; hi++)
            {
                for (int wj = -range; wj <= range; wj++)
                {
                    int temp_x = x[i] + wj, temp_y = y[i] + hi;
                    if (temp_y < 0 || temp_y >= kCospaceHeight || temp_x < 0 || temp_x >= kCospaceWidth)
                    {
                        continue;
                    }
                    if (map[0][temp_y][temp_x] != MAP_WALL)
                    {
                        if (min_value > ABS(hi) + ABS(wj))
                        {
                            min_value = ABS(hi) + ABS(wj);
                            min_position[0] = temp_x;
                            min_position[1] = temp_y;
                        }
                    }
                }
            }
            if (min_position[0] < 0)
            {
                // y * kDotWidth + x -> -1
                logErrorMessage.errorMessage(FUNCNAME + "(): there is no not wall dot near " + to_string(x[i]) + " " + to_string(y[i]), MODE_NORMAL);
                y[i] = -1;
                x[i] = -1;
            }
        }
        robot_dot_positions[i][0] = x[i];
        robot_dot_positions[i][1] = y[i];
    }
    now_dot_id = robot_dot_positions[1][1] * kDotWidth + robot_dot_positions[1][0];
}

long AutoStrategy::WhereIsMotor(void)
{
    LOG_MESSAGE(FUNCNAME + "(): start", MODE_VERBOSE);
    long x, y;
    if (pos_x < 0)
    {
        x = 0;
    }
    if (pos_x >= kCospaceWidth)
    {
        x = kCospaceWidth - 1;
    }
    if (pos_y < 0)
    {
        y = 0;
    }
    if (pos_y >= kCospaceHeight)
    {
        y = kCospaceHeight - 1;
    }
    x = pos_x - static_cast<long>(cos((Compass + 90) * 3.14 / 180) * 5);
    y = pos_y - static_cast<long>(sin((Compass + 90) * 3.14 / 180) * 5);
    LOG_MESSAGE(FUNCNAME + "(): return " + to_string(y * 1000 + x) + " (x, y) : (" + to_string(x) + ", " + to_string(y) + ")", MODE_VERBOSE);
    return y * 1000 + x;
}

long AutoStrategy::WhereIsColorSensor(void)
{
    LOG_MESSAGE(FUNCNAME + "(): start", MODE_VERBOSE);
    long x, y;
    if (pos_x < 0 || pos_x >= kCospaceWidth || pos_y < 0 || pos_y >= kCospaceHeight)
    {
        pos_x = kCospaceWidth / 2;
        pos_y = kCospaceHeight / 2;
    }
    x = pos_x + static_cast<long>(cos((Compass + 90) * 3.14 / 180) * 5);
    y = pos_y + static_cast<long>(sin((Compass + 90) * 3.14 / 180) * 5);
    //fprintf(logfile, "%4d WhereIsColorSensor() (x, y) = (%ld, %ld)\n", getRepeatedNum(), x, y);
    if (x < 0)
    {
        //fprintf(errfile, "%4d WhereIsColorSensor() (x, y) = (%ld, %ld)\n", getRepeatedNum(), x, y);
        x = 0;
    }
    if (y < 0)
    {
        //fprintf(errfile, "%4d WhereIsColorSensor() (x, y) = (%ld, %ld)\n", getRepeatedNum(), x, y);
        y = 0;
    }
    if (x >= kCospaceWidth)
    {
        //fprintf(errfile, "%4d WhereIsColorSensor() (x, y) = (%ld, %ld)\n", getRepeatedNum(), x, y);
        x = kCospaceWidth - 1;
    }
    if (y >= kCospaceHeight)
    {
        //fprintf(errfile, "%4d WhereIsColorSensor() (x, y) = (%ld, %ld)\n", getRepeatedNum(), x, y);
        y = kCospaceHeight - 1;
    }
    LOG_MESSAGE(FUNCNAME + "(): return " + to_string(y * 1000 + x) + " (x, y) : (" + to_string(x) + ", " + to_string(y) + ")", MODE_VERBOSE);
    return y * 1000 + x;
}

int AutoStrategy::GoToPosition(int x, int y, int wide_decide_x, int wide_decide_y, int wide_judge_arrived)
{
    //fprintf(logfile, " %d Start GoToPosition(%d, %d, %d, %d, %d)\n", getRepeatedNum(), x, y, wide_decide_x, wide_decide_y, wide_judge_arrived);

    static int absolute_x = -1;
    static int absolute_y = -1;
    static int absolute_distance = -1;
    static int same_operate = -1;

    static int repeated_num_log = -1;
    static int objects_num_log = -1;
    if (repeated_num_log == -1)
    {
        repeated_num_log = getRepeatedNum() - 1;
        objects_num_log = LoadedObjects;
    }

    //引数の値がおかしい場合
    if (x < 0 || y < 0 || x > kCospaceWidth || y > kCospaceHeight || wide_decide_x < 0 || wide_decide_y < 0 || wide_judge_arrived < 0)
    {
        printf("GoToPosition(): 引数が(%d, %d, %d, %d, %d)\n", x, y, wide_decide_x, wide_decide_y, wide_judge_arrived);
        //fprintf(errfile, "%d GoToPosition(): 引数が(%d, %d, %d, %d, %d)\n", getRepeatedNum(), x, y, wide_decide_x, wide_decide_y, wide_judge_arrived);
        //fprintf(logfile, " %d GoToPosition(): 引数が(%d, %d, %d, %d, %d)\n", getRepeatedNum(), x, y, wide_decide_x, wide_decide_y, wide_judge_arrived);
        return 0;
    }

    //absolute_x,yの値が、x, yの値からずれている場合
    if (absolute_x == -1 || !(PLUSMINUS(absolute_x, x, wide_decide_x) && PLUSMINUS(absolute_y, y, wide_decide_y)))
    {
        int i = 0;
        do
        {
            if (i > 5)
            {
                absolute_x = x;
                absolute_y = y;

                ERROR_MESSAGE("warming GoToPosition(): absolute_x, absolute_yが決まりません\n", MODE_NORMAL);
                break;
            }
            absolute_x = x - wide_decide_x + (rand() + 1) % (wide_decide_x * 2 + 1);
            absolute_y = y - wide_decide_y + (rand() + 1) % (wide_decide_y * 2 + 1);
            i++;
        } while (absolute_x < 10 || absolute_x > kCospaceWidth - 10 || absolute_y < 10 || absolute_y > kCospaceHeight - 10);
        //same_operate = 0;
    }

    if (absolute_distance <= -1)
    {
        absolute_distance = TO_INT(sqrt(pow(absolute_x - pos_x, 2) + pow(absolute_y - pos_y, 2))) + 40;
    }

    if (absolute_distance < same_operate)
    {
        printf("(%d, %d) arrive because too many same_operate\n", x, y);
        absolute_x = -1;
        absolute_y = -1;
        same_operate = -1;
        absolute_distance = -1;
        if (PositionX == -1)
        {
            pos_x = x;
            pos_y = y;
        }
        return 1;
    }

    int temp_x = WhereIsColorSensor();
    int temp_y = temp_x / 1000;
    temp_x -= temp_y * 1000;
    if (PLUSMINUS(absolute_x, temp_x, wide_judge_arrived) && PLUSMINUS(absolute_y, temp_y, wide_judge_arrived))
    {
        printf("(%d, %d)に到着しました\n", absolute_x, absolute_y);
        LOG_MESSAGE("(" + to_string(absolute_x) + "," + to_string(absolute_y) + ")に到着しました", MODE_NORMAL);
        absolute_x = -1;
        absolute_y = -1;
        same_operate = -1;
        absolute_distance = -1;
        return 1;
    }

    LOG_MESSAGE("ab(" + to_string(absolute_x) + "," + to_string(absolute_y) + ")", MODE_NORMAL);
    x = absolute_x;
    y = absolute_y;
    x = x - temp_x;
    y = y - temp_y;
    LOG_MESSAGE("x, y = " + to_string(x) + ", " + to_string(y), MODE_NORMAL);
    double angle = atan2(y, x);
    angle = angle * 180 / 3.14;
    int angle_int = TO_INT(angle);
    angle_int -= 90;
    if (angle_int < 0)
    {
        angle_int += 360;
    }
    LOG_MESSAGE("angle " + to_string(angle_int), MODE_NORMAL);
    GoToAngle(angle_int, TO_INT(sqrt(x * x + y * y)));

    if (repeated_num_log + 1 == getRepeatedNum() || objects_num_log != LoadedObjects)
    {
        same_operate++;
    }
    else
    {
        same_operate = 0;
    }
    repeated_num_log = getRepeatedNum();
    return 0;
}

int AutoStrategy::isNearTheFloor(MapInfo color, int x, int y, int cm_radius)
{
    LOG_MESSAGE(FUNCNAME + "(" + to_string(TO_INT(color)) + "," + to_string(x) + "," + to_string(y) + "," + to_string(cm_radius) + "): start", MODE_VERBOSE);
    int dot_radious = (cm_radius + kCM2DotScale - 1) / kCM2DotScale;

    // xi = now_x - radious ~ now_x + radious
    for (int xi = robot_dot_positions[1][0] - dot_radious; xi <= robot_dot_positions[1][0] + dot_radious; xi++)
    {
        if (xi < 0 || xi >= kDotWidth)
        {
            continue;
        }

        // yj = now_y - radious ~ now_y + radious
        for (int yj = robot_dot_positions[1][1] - dot_radious; yj <= robot_dot_positions[1][1] + dot_radious; yj++)
        {
            if (yj < 0 || yj >= kDotHeight)
            {
                continue;
            }
            if (map[0][yj][xi] == color)
            {
                LOG_MESSAGE(FUNCNAME + "(): return 1", MODE_VERBOSE);
                return 1;
            }
        }
    }
    LOG_MESSAGE(FUNCNAME + "(): return 0", MODE_VERBOSE);
    return 0;
}

void AutoStrategy::GoToAngle(int angle, int distance)
{
    LOG_MESSAGE(FUNCNAME + "(" + to_string(angle) + "," + to_string(distance) + "): start", MODE_VERBOSE);
    angle = angle - Compass;
    angle %= 360;
    if (angle > 180)
    {
        angle -= 360;
    }
    if (angle < -180)
    {
        angle += 360;
    }

    int classification = obstacle(8, 10, 8);
    if (log_superobj_num > 0)
    {
        classification = obstacle(5, 7, 5);
    }

    // double magnification = 0.3;
    int short_front = 1; //(int)(pow(US_Front, magnification) * (5 - (WheelLeft * WheelLeft + WheelRight * WheelRight) / 8) / pow(25, magnification));
    int short_left = 1;  //(int)(pow(US_Left, magnification) * (5 - (WheelLeft * WheelLeft + WheelRight * WheelRight) / 8) / pow(25, magnification));
    int short_right = 1; //(int)(pow(US_Right, magnification) * (5 - (WheelLeft * WheelLeft + WheelRight * WheelRight) / 8) / pow(25, magnification));
    if (short_front < 0)
        short_front = 0;
    if (short_front > 5)
        short_front = 5;
    if (short_right < 0)
        short_right = 0;
    if (short_right > 5)
        short_right = 5;
    if (short_left < 0)
        short_left = 0;
    if (short_left > 5)
        short_left = 5;
    switch (classification)
    {
    case 0:
        classification = obstacle(20, 20, 20);
        if (log_superobj_num > 0)
        {
            classification = obstacle(15, 20, 15);
        }
        if (classification == 1 && angle > 0 && angle < 90)
        { //left
            motor(5, short_left);
        }
        else if (classification == 2 && abs(angle) < 30)
        { //front
            if (angle < 0)
            {
                motor(5, short_front);
            }
            else
            {
                motor(short_front, 5);
            }
        }
        else if (classification == 3 && angle > -30 && angle < 90)
        { //left & front
            motor(5, (short_left < short_front) ? (short_left) : (short_right));
        }
        else if (classification == 4 && angle < 0 && angle > -90)
        { //right
            motor(short_right, 5);
        }
        else if (classification == 5 && abs(angle) > 30)
        { //left & right
            if (abs(angle) < 150)
            {
                motor(5, 5);
            }
            else
            {
                if (angle < 0)
                {
                    motor(3, -4);
                }
                else
                {
                    motor(-4, 3);
                }
                // Duration = 5;
            }
        }
        else if (classification == 6 && angle < 30 && angle > -90)
        { //front & right
            motor((short_right < short_front) ? (short_right) : (short_right), 5);
        }
        else if (classification == 7)
        { //all
            if (angle < 0)
            {
                motor(5, short_front);
            }
            else
            {
                motor(short_front, 5);
            }
        }
        else
        {
            if (log_superobj_num > 0 && pow(pos_y - log_superobj_y[0], 2) + pow(pos_x - log_superobj_x[0], 2) < 800)
            {
                printf("log_superobj_num > 0 %d\n", angle);
                if (abs(angle) < 30)
                {
                    if (distance < 5)
                    {
                        motor(-5, -5);
                    }
                    else
                    {
                        motor(5, 5);
                    }
                }
                else if (abs(angle) < 100)
                {
                    if (angle < 0)
                    {
                        motor(3, -1);
                    }
                    else
                    {
                        motor(-1, 3);
                    }
                }
                else if (abs(angle) < 120)
                {
                    if (angle < 0)
                    {
                        motor(-3, -5);
                    }
                    else
                    {
                        motor(-5, -3);
                    }
                }
                else
                {
                    if (angle < 0)
                    {
                        motor(3, -5);
                    }
                    else
                    {
                        motor(-5, 3);
                    }
                }
            }
            else if (IsOnSwampland())
            {
                if (abs(angle) < 30)
                {
                    motor(5, 5);
                }
                else if (abs(angle) < 90)
                {
                    if (angle < 0)
                    {
                        motor(5, 1);
                    }
                    else
                    {
                        motor(1, 5);
                    }
                }
                else
                {
                    if (angle < 0)
                    {
                        motor(5, -5);
                    }
                    else
                    {
                        motor(-5, 5);
                    }
                }
                Duration = 5;
            }
            else if (isNearTheFloor(MAP_YELLOW, robot_dot_positions[1][0], robot_dot_positions[1][1], 30) || isNearTheFloor(MAP_UNKNOWN, robot_dot_positions[1][0], robot_dot_positions[1][1], 30))
            {
                LOG_MESSAGE("near yellow or unknown", MODE_DEBUG);
                if (abs(angle) < 10)
                {
                    motor(3, 3);
                }
                else if (abs(angle) < 30)
                {
                    if (angle < 0)
                    {
                        motor(4, 3);
                    }
                    else
                    {
                        motor(3, 4);
                    }
                }
                else if (abs(angle) < 80 && LoadedObjects != 6)
                {
                    if (angle < 0)
                    {
                        motor(3, 1);
                    }
                    else
                    {
                        motor(1, 3);
                    }
                }
                else if (abs(angle) < 120)
                {
                    if (angle < 0)
                    {
                        motor(2, -3);
                    }
                    else
                    {
                        motor(-3, 2);
                    }
                }
                else
                {
                    if (angle < 0)
                    {
                        motor(3, -4);
                    }
                    else
                    {
                        motor(-4, 3);
                    }
                }
            }
            // else if ((loaded_objects[BLACK_LOADED_ID] < 2 && dot[now_dot_id].black == 1) || (loaded_objects[CYAN_LOADED_ID] < 2 && dot[now_dot_id].cyan == 1) || (loaded_objects[RED_LOADED_ID] < 2 && dot[now_dot_id].red == 1))
            // {
            // 	if (abs(angle) < 10)
            // 	{
            // 		if (rand() % 4)
            // 		{
            // 			motor(4, 4);
            // 		}
            // 		else
            // 		{
            // 			motor(5, 5);
            // 		}
            // 	}
            // 	else if (abs(angle) < 80)
            // 	{
            // 		if (angle < 0)
            // 		{
            // 			motor(5, 2);
            // 		}
            // 		else
            // 		{
            // 			motor(2, 5);
            // 		}
            // 	}
            // 	else if (abs(angle) < 120)
            // 	{
            // 		if (angle < 0)
            // 		{
            // 			motor(5, 0);
            // 		}
            // 		else
            // 		{
            // 			motor(0, 5);
            // 		}
            // 	}
            // 	else
            // 	{
            // 		if (angle < 0)
            // 		{
            // 			motor(2, -3);
            // 		}
            // 		else
            // 		{
            // 			motor(-3, 2);
            // 		}
            // 	}
            // }
            else
            {
                // printf("angle = %d\n", angle);
                if (abs(angle) < 20)
                {
                    if (distance < 20)
                    {
                        if (angle < 0)
                        {
                            motor(3, 2);
                        }
                        else
                        {
                            motor(2, 3);
                        }
                    }
                    else
                    {
                        motor(5, 5);
                    }
                }
                else if (abs(angle) < 60)
                {
                    if (angle < 0)
                    {
                        motor(5, 3);
                    }
                    else
                    {
                        motor(3, 5);
                    }
                }
                else if (abs(angle) < 120)
                {
                    if (angle < 0)
                    {
                        motor(5, 2);
                    }
                    else
                    {
                        motor(2, 5);
                    }
                }
                else if (distance < 20)
                {
                    if (angle < 0)
                    {
                        motor(0, -3);
                    }
                    else
                    {
                        motor(-3, 0);
                    }
                }
                else
                {
                    if (angle < 0)
                    {
                        motor(4, -5);
                    }
                    else
                    {
                        motor(-5, 4);
                    }
                    // Duration = 3;
                }
            }
        }
        break;
    case 1: //left
        motor(-2, -3);
        break;
    case 2: //front
        if (US_Left < US_Right)
        {
            motor(-1, -3);
        }
        else
        {
            motor(-3, -1);
        }
        break;
    case 3: //left front
        motor(-2, -3);
        break;
    case 4: //right
        motor(-3, -2);
        break;
    case 5: //left right
        motor(-3, -3);
        break;
    case 6: //front right
        if (CurGame == 0)
        {
            motor(3, 3);
        }
        else
        {
            motor(-2, -3);
        }
        break;
    case 7: //left front right
        motor(-3, -3);
        break;
    default:
        break;
    }
    // if(IsOnSwampland()) {
    // 	int max_absolute_value = abs(WheelLeft);
    // 	if(max_absolute_value < abs(WheelRight)) {
    // 		max_absolute_value = abs(WheelRight);
    // 	}
    // 	if(max_absolute_value < 5) {
    // 		if(WheelLeft < 0) {
    // 			WheelLeft -= 5 - max_absolute_value;
    // 		}
    // 		else {
    // 			WheelLeft += 5 - max_absolute_value;
    // 		}
    // 		if(WheelRight < 0) {
    // 			WheelRight -= 5 - max_absolute_value;
    // 		}
    // 		else {
    // 			WheelRight += 5 - max_absolute_value;
    // 		}
    // 	}
    // }
    LOG_MESSAGE(FUNCNAME + "(): end with motor(" + to_string(WheelLeft) + "," + to_string(WheelRight) + ")", MODE_VERBOSE);
}

int AutoStrategy::GoToDot(int x, int y)
{
    static int prev_x = -1, prev_y = -1, prev_now_dot_id = -1;

    LOG_MESSAGE(FUNCNAME + "(" + to_string(x) + "," + to_string(y) + "): start", MODE_DEBUG);

    if (PositionX == -1 && (PLUSMINUS(pos_x, x * kCM2DotScale, (10 < kCM2DotScale ? kCM2DotScale : 10)) && PLUSMINUS(pos_y, y * kCM2DotScale, (10 < kCM2DotScale ? kCM2DotScale : 10))))
    {
        LOG_MESSAGE(FUNCNAME + "() end returning 1 because I am in PLA and it's near target(" + to_string(x) + ", " + to_string(y) + ")", MODE_NORMAL);
        GoToPosition(x, y, 10, 10, 5);
        return 1;
    }
    char map_data_to_show[kDotWidth * kDotHeight];
    rep(yi, kDotHeight)
    {
        rep(xj, kDotWidth)
        {
            if (map[0][yi][xj] == MAP_WALL || map[0][yi][xj] == MAP_YELLOW)
            {
                map_data_to_show[yi * kDotWidth + xj] = '#';
            }
            else if (map[0][yi][xj] == MAP_SWAMPLAND)
            {
                map_data_to_show[yi * kDotWidth + xj] = '$';
            }
            else
            {
                map_data_to_show[yi * kDotWidth + xj] = ' ';
            }
        }
    }

    //If the node I want to go will be go out
    if (x < 1 || x >= kDotWidth - 1 || y < 1 || y >= kDotHeight - 1)
    {
        ERROR_MESSAGE(FUNCNAME + "(): (x, y) is (" + to_string(x) + ", " + to_string(y) + ") and strange", MODE_NORMAL);
    }
    if (x < 0)
    {
        ERROR_MESSAGE(FUNCNAME + "() x is " + to_string(x), MODE_NORMAL);
        x = 0;
    }
    if (x >= kDotWidth)
    {
        ERROR_MESSAGE(FUNCNAME + "() x is " + to_string(x), MODE_NORMAL);
        x = kDotWidth - 1;
    }

    if (y < 0)
    {
        ERROR_MESSAGE(FUNCNAME + "() y is " + to_string(y), MODE_NORMAL);
        y = 0;
    }
    if (y >= kDotHeight)
    {
        ERROR_MESSAGE(FUNCNAME + "() y is " + to_string(y), MODE_NORMAL);
        y = kDotHeight - 1;
    }

    if (prev_now_dot_id != now_dot_id || prev_x != x || prev_y != y)
    {
        // Dijkstra();
        Astar(x, y);
    }
    prev_now_dot_id = now_dot_id;
    prev_x = x;
    prev_y = y;

    map_data_to_show[y * kDotWidth + x] = 'T';
    int i = 0;

    int temp_y = y;
    int temp_x = x;
    while ((map_from[temp_y][temp_x][0] != robot_dot_positions[1][0] || map_from[temp_y][temp_x][1] != robot_dot_positions[1][1]) && i < kDotWidth * 3)
    {
        temp_y = map_from[temp_y][temp_x][1];
        temp_x = map_from[temp_y][temp_x][0];
        map_data_to_show[temp_y * kDotWidth + temp_x] = '+';
        i++;
        if (temp_x < 0 || temp_x >= kDotWidth)
        {
            ERROR_MESSAGE(FUNCNAME + "(): temp_x (=" + to_string(temp_x) + ") is strange. I continue", MODE_NORMAL);
            GoToPosition(x * kCM2DotScale, y * kCM2DotScale, 5, 5, 5);
        }
        if (temp_y < 0 || temp_y >= kDotHeight)
        {
            ERROR_MESSAGE(FUNCNAME + "(): temp_y (=" + to_string(temp_y) + ") is strange. I continue", MODE_NORMAL);
            GoToPosition(x * kCM2DotScale, y * kCM2DotScale, 5, 5, 5);
        }
    }
    if (i == 200)
    {
        printf("\n\n\niの値が200ですByGoToNode()\n\n\n\n");
        LOG_MESSAGE(FUNCNAME + "(): iの値が200です", MODE_NORMAL);
    }

    map_data_to_show[now_dot_id] = '@';

    int next_x = temp_x, next_y = temp_y;

    int now_y = now_dot_id / kDotWidth;
    int now_x = now_dot_id - now_y * kDotWidth;

    int distance = 20;
    if (next_x < now_x)
    {
        if (next_y < now_y)
        {
            GoToAngle(135, distance);
        }
        else if (next_y == now_y)
        {
            GoToAngle(90, distance);
        }
        else
        {
            GoToAngle(45, distance);
        }
    }
    else if (next_x == now_x)
    {
        if (next_y < now_y)
        {
            GoToAngle(180, distance);
        }
        else if (next_y == now_y)
        {
            GoToPosition(pos_x - 3 + rand() % 6, pos_y - 3 + rand() % 6, 6, 6, 3);
            return 1;
        }
        else
        {
            GoToAngle(0, distance);
        }
    }
    else
    {
        if (next_y < now_y)
        {
            GoToAngle(225, distance);
        }
        else if (next_y == now_y)
        {
            GoToAngle(270, distance);
        }
        else
        {
            GoToAngle(315, distance);
        }
    }

    if (getRepeatedNum() % 5 == 0)
    {
        rep(xj, kDotWidth + 2)
        {
            printf("#");
        }
        rep(yi, kDotHeight)
        {
            printf("#");
            rep(xj, kDotWidth)
            {
                printf("%c", map_data_to_show[(kDotHeight - 1 - yi) * kDotWidth + xj]);
            }
            printf("#");
            printf("\n");
        }
        rep(xj, kDotWidth + 2)
        {
            printf("#");
        }
        printf("\n");
    }

    return 0;
}

int AutoStrategy::
    GoToDots(int x, int y, int wide_decide_x, int wide_decide_y)
{
    LOG_MESSAGE(FUNCNAME + "(" + to_string(x) + "," + to_string(y) + "," + to_string(wide_decide_x) + "," + to_string(wide_decide_y) + ")", MODE_VERBOSE);

    static int prev_x = -1;
    static int prev_y = -1;
    static int target_x = -1;
    static int target_y = -1;
    static int local_same_target = 0;
    static int same_target_border = 0;
    if (x != prev_x || y != prev_y)
    {
        LOG_MESSAGE(FUNCNAME + "(): changed dots", MODE_NORMAL);
        // Dijkstra();
        int max_value = 0;
        // int max_pos[2] = {-1, -1};
        int min_value = INT_MAX;
        int min_pos[2] = {-1, -1};
        int cost;
        // for (long yi = kDotHeight - 1; yi >= 0; --yi)
        // {
        //     rep(xj, kDotWidth)
        //     {
        //         if (min_pos[0] == xj && min_pos[1] == yi)
        //         {
        //             printf("m");
        //             continue;
        //         }
        //         if (xj == robot_dot_positions[1][0] && yi == robot_dot_positions[1][1])
        //         {
        //             printf("#");
        //             continue;
        //         }
        //         switch (map[0][yi][xj])
        //         {
        //         case MAP_YELLOW:
        //         case MAP_WALL:
        //             printf("*");
        //             break;
        //         default:
        //             printf("%d", TO_INT(map_cost[yi][xj] * 9 / max_value));
        //         }
        //     }
        //     printf("\n");
        // }
        // printf("\n");

        local_same_target = 0;
        prev_x = x;
        prev_y = y;
        //0:left bottom corner 1:right bottom corner 2:right bottom corner
        int corner_x[2], corner_y[2];
        corner_x[0] = (x - wide_decide_x) / kCM2DotScale;
        corner_y[0] = (y - wide_decide_y) / kCM2DotScale;
        corner_x[1] = (x + wide_decide_x) / kCM2DotScale;
        corner_y[1] = (y + wide_decide_y) / kCM2DotScale;

        for (int i = 0; i < 2; i++)
        {
            if (corner_x[i] < 0)
            {
                corner_x[i] = 0;
            }
            if (corner_x[i] >= kDotWidth)
            {
                corner_x[i] = kDotWidth - 1;
            }
            if (corner_y[i] < 0)
            {
                corner_y[i] = 0;
            }
            if (corner_y[i] >= kDotHeight)
            {
                corner_y[i] = kDotHeight - 1;
            }
        }

        // int min = 100000, id = -1;
        // n回に1回移動する
        // int option = rnd() % 5;
        // int option = 1;
        for (int yi = corner_y[0]; yi <= corner_y[1]; ++yi)
        {
            for (int xj = corner_x[0]; xj <= corner_x[1]; ++xj)
            {
                cost = abs(yi - robot_dot_positions[1][1]) + abs(xj - robot_dot_positions[1][0]);
                if (yi <= TO_INT((10 - 1) / kCM2DotScale) || yi >= TO_INT((kCospaceHeight - 10) / kCM2DotScale))
                {
                    continue;
                }
                if (xj <= TO_INT(10 - 1) / kCM2DotScale || xj >= TO_INT((kCospaceWidth - 10) / kCM2DotScale))
                {
                    continue;
                }
                if (map[0][yi][xj] == MAP_UNKNOWN || map[0][yi][xj] == MAP_UNKNOWN_NOT_WALL)
                {
                    cost -= 1000;
                }

                cost = map_cost[yi][xj];
                if (max_value < cost)
                {
                    max_value = cost;
                    // max_pos[0] = xj;
                    // max_pos[1] = yi;
                }
                if (cost < min_value)
                {
                    min_value = cost;
                    min_pos[0] = xj;
                    min_pos[1] = yi;
                }
                // int investigating_dot_id = j * kDotHeight + i;
                // if (i <= 0 || i >= kDotWidth - 1 || j <= 0 || j >= kDotHeight - 1)
                // {
                //     continue;
                // }

                // //yellow or wall or deposit
                // if (map[0][j][i] == MAP_YELLOW || map[0][j][i] == MAP_WALL || map[0][j][i] == MAP_DEPOSIT)
                // {
                //     continue;
                // }

                // int costs = TO_INT(map_arrived_times[j][i] * 100 + rand() % 10);
                // if (option)
                // {
                //     // 移動しないとき
                //     int k = 30;
                //     costs += TO_INT(pow(abs(i * kCM2DotScale - pos_x) - k, 2) * 100 - pow(abs(j * kCM2DotScale - pos_y) - k, 2) * 100);
                // }
                // else
                // {
                //     // 移動するとき
                //     costs -= TO_INT(pow(i * kCM2DotScale - pos_x, 2) / 100 - pow(j * kCM2DotScale - pos_y, 2) / 100);
                // }
                // if (costs < min)
                // {
                //     min = costs;
                //     id = investigating_dot_id;
                // }
            }
        }
        target_x = min_pos[0];
        target_y = min_pos[1];
        // if (id == -1)
        // {
        //     target_x = x / kCM2DotScale;
        //     target_y = y / kCM2DotScale;
        // }
        // else
        // {
        //     target_y = id / kDotWidth;
        //     target_x = id - target_y * kDotWidth;
        // }

        same_target_border = TO_INT(sqrt(pow(pos_x - target_x * kCM2DotScale, 2) + pow(pos_y - target_y * kCM2DotScale, 2)));
        same_target_border *= 2;
        same_target_border += 30;
        LOG_MESSAGE(FUNCNAME + "(): set same_target_border as " + to_string(same_target_border), MODE_VERBOSE);
    }
    local_same_target++;

    LOG_MESSAGE(FUNCNAME + "(): calculated best coordinate(" + to_string(target_x * kCM2DotScale) + "," + to_string(target_y * kCM2DotScale) + ")", MODE_DEBUG);

    if (GoToDot(target_x, target_y))
    {
        prev_x = -1;
        local_same_target = 0;
        LOG_MESSAGE(FUNCNAME + "() : GoToDot() return 1; GoToDots() return 1", MODE_DEBUG);
        return 1;
    }
    else if (local_same_target > same_target_border)
    {
        prev_x = -1;
        local_same_target = 0;
        LOG_MESSAGE(FUNCNAME + "() : local_same_target(" + to_string(local_same_target) + ") < same_target_border(" + to_string(same_target_border) + ") return 1", MODE_DEBUG);
        return 1;
    }
    else
    {
        LOG_MESSAGE(FUNCNAME + "() : return 0", MODE_DEBUG);
        //fprintf(logfile, " %d End GoToDots() with returning 0\n", getRepeatedNum());
        return 0;
    }
}

void AutoStrategy::autoSearch(float parameter)
{
    static int is_changed = 1;
    static int target_x = -1, target_y = -1;
    if (is_changed)
    {
        is_changed = 0;
        int score_area_map[kAreaHeight][kAreaWidth];
        rep(ayi, kAreaHeight)
        {
            rep(axj, kAreaWidth)
            {
                score_area_map[ayi][axj] = 0;
            }
        }
        if (parameter < 10)
        {
            int score = 0;
            int max_score = INT_MIN;
            int max_score_x = -1;
            int max_score_y = -1;
            // 候補のエリアを選ぶ
            rep(yi, kDotHeight)
            {
                rep(xj, kDotWidth)
                {
                    if (map[0][yi][xj] == MAP_WALL || map[0][yi][xj] == MAP_YELLOW)
                    {
                        continue;
                    }

                    score = kCM2AreaScale;
                    if (map[0][yi][xj] == MAP_UNKNOWN || map[0][yi][xj] == MAP_UNKNOWN_NOT_WALL)
                    {
                        score *= 2;
                    }
                    score -= map_arrived_times[yi][xj];
                    score_area_map[TO_INT(yi / kDot2AreaScale)][TO_INT(xj / kDot2AreaScale)] += score;
                }
            }

            int max_value;
            rep(ayi, kAreaHeight)
            {
                rep(axj, kAreaWidth)
                {
                    if (abs(ayi * kCM2AreaScale - pos_y) + abs(axj * kCM2AreaScale - pos_x) > max_value)
                    {
                        max_value = abs(ayi * kCM2AreaScale - pos_y) + abs(axj * kCM2AreaScale - pos_x);
                    }
                }
            }
            rep(ayi, kAreaHeight)
            {
                rep(axj, kAreaWidth)
                {
                    score_area_map[ayi][axj] += i_sigmoid(fabsf(static_cast<float>(ayi * kCM2AreaScale - pos_y)) + fabsf(static_cast<float>(axj * kCM2AreaScale - pos_x)) / static_cast<float>(max_value) * 20.0f - 10.0f, static_cast<float>(max_value) / 20.0f);
                    if (max_score < score_area_map[ayi][axj])
                    {
                        max_score = score_area_map[ayi][axj];
                        max_score_x = axj;
                        max_score_y = ayi;
                    }
                }
            }
            target_x = max_score_x;
            target_y = max_score_y;
            LOG_MESSAGE(FUNCNAME + "(): calculated best area (" + to_string(max_score_x * kCM2AreaScale + TO_INT(kCM2AreaScale / 2)) + ", " + to_string(max_score_y * kCM2AreaScale + TO_INT(kCM2AreaScale / 2)) + ")", MODE_DEBUG);
            GoToDots(max_score_x * kCM2AreaScale + TO_INT(kCM2AreaScale / 2), max_score_y * kCM2AreaScale + TO_INT(kCM2AreaScale / 2), TO_INT(kCM2AreaScale / 2), TO_INT(kCM2AreaScale / 2));
        }
    }
    else
    {
        if (GoToDots(target_x * kCM2AreaScale + TO_INT(kCM2AreaScale / 2), target_y * kCM2AreaScale + TO_INT(kCM2AreaScale / 2), TO_INT(kCM2AreaScale / 2), TO_INT(kCM2AreaScale / 2)))
        {
            is_changed = 1;
            LOG_MESSAGE(FUNCNAME + "(): is_changed true", MODE_VERBOSE);
        }
    }
}

void AutoStrategy::Dijkstra(void)
{
    // 初期化
    rep(i, kDotHeight)
    {
        rep(j, kDotWidth)
        {
            map_from[i][j][0] = -1;
            map_from[i][j][1] = -1;
            map_cost[i][j] = -1;
            map_status[i][j] = 0;
        }
    }

    if (robot_dot_positions[1][0] < 0 || robot_dot_positions[1][0] >= kDotWidth || robot_dot_positions[1][1] < 0 || robot_dot_positions[1][1] >= kDotHeight)
    {
        ERROR_MESSAGE(FUNCNAME + "(); now dot is (" + to_string(robot_dot_positions[1][0]) + ", " + to_string(robot_dot_positions[1][1]) + ")", MODE_NORMAL);
    }

    map_cost[robot_dot_positions[1][1]][robot_dot_positions[1][0]] = 0;
    map_from[robot_dot_positions[1][1]][robot_dot_positions[1][0]][0] = robot_dot_positions[1][0];
    map_from[robot_dot_positions[1][1]][robot_dot_positions[1][0]][1] = robot_dot_positions[1][1];
    map_status[robot_dot_positions[1][1]][robot_dot_positions[1][0]] = 1;

    while (true)
    {
        int investigating_dot_x = -1, investigating_dot_y = -1;

        rep(yi, kDotHeight)
        {
            rep(xj, kDotWidth)
            {
                if (map_status[yi][xj] != 1)
                {
                    continue;
                }
                if (investigating_dot_x == -1 || map_cost[yi][xj] < map_cost[investigating_dot_y][investigating_dot_x])
                {
                    investigating_dot_x = xj;
                    investigating_dot_y = yi;
                }
            }
        }

        // 新しいdotがない場合
        if (investigating_dot_x == -1)
        {
            break;
        }

        map_status[investigating_dot_y][investigating_dot_x] = 2;
        for (int y = investigating_dot_y - 1; y <= investigating_dot_y + 1; ++y)
        {
            for (int x = investigating_dot_x - 1; x <= investigating_dot_x + 1; ++x)
            {
                if (0 <= x && x < kDotWidth && 0 <= y && y < kDotHeight)
                {
                    if (map_status[y][x] == 2)
                    {
                        continue;
                    }
                    int cost = kCM2DotScale * 10000;
                    // cost += map_arrived_times[y][x] * 10;

                    if (map[0][y][x] == MAP_YELLOW || map[0][y][x] == MAP_WALL)
                    {
                        cost *= 100000;
                    }
                    if (map[0][y][x] == MAP_SWAMPLAND)
                    {
                        cost *= 1000;
                    }
                    if (map[0][y][x] == MAP_UNKNOWN || map[0][y][x] == MAP_UNKNOWN_NOT_WALL)
                    {
                        if (Time < 60)
                        {
                            cost /= 100;
                            // cost -= 100;
                        }
                        else
                        {
                            cost /= 2;
                        }
                    }
                    if (map[RED_LOADED_ID][y][x] == 1 && loaded_objects[RED_LOADED_ID] < kBorderSameObjNum)
                    {
                        cost /= 10;
                    }
                    if (map[CYAN_LOADED_ID][y][x] == 1 && loaded_objects[CYAN_LOADED_ID] < kBorderSameObjNum)
                    {
                        cost /= 10;
                    }
                    if (map[BLACK_LOADED_ID][y][x] == 1 && loaded_objects[BLACK_LOADED_ID] < kBorderSameObjNum)
                    {
                        cost /= 10;
                    }
                    if (map_status[y][x] == 0 || map_cost[investigating_dot_y][investigating_dot_x] + cost < map_cost[y][x])
                    {
                        map_cost[y][x] = map_cost[investigating_dot_y][investigating_dot_x] + cost;
                        map_from[y][x][0] = investigating_dot_x;
                        map_from[y][x][1] = investigating_dot_y;
                        map_status[y][x] = 1;
                    }
                }
            }
        }
    }
    // 出力
    int max_value = -100;
    rep(yi, kDotHeight)
    {
        rep(xj, kDotWidth)
        {
            if (map_cost[yi][xj] > max_value)
            {
                max_value = map_cost[yi][xj];
            }
        }
    }
    LOG_MESSAGE(FUNCNAME + "(): max value : " + to_string(max_value), MODE_VERBOSE);
    if (max_value == 0)
    {
        ERROR_MESSAGE(FUNCNAME + "(): max value is 0", MODE_NORMAL);
        return;
    }

    // for (long yi = kDotHeight - 1; yi >= 0; --yi)
    // {
    //     rep(xj, kDotWidth)
    //     {
    //         switch (map[0][yi][xj])
    //         {
    //         case MAP_YELLOW:
    //         case MAP_WALL:
    //             printf("*");
    //             break;
    //         default:
    //             printf("%d", TO_INT(map_cost[yi][xj] * 9 / max_value));
    //         }
    //     }
    //     printf("\n");
    // }
    // printf("\n");
}

void AutoStrategy::Astar(int goal_x, int goal_y)
{
    // 初期化
    rep(i, kDotHeight)
    {
        rep(j, kDotWidth)
        {
            map_from[i][j][0] = -1;
            map_from[i][j][1] = -1;
            map_cost[i][j] = -1;
            map_status[i][j] = 0;
            map_total_cost[i][j] = -1;
        }
    }

    if (robot_dot_positions[1][0] < 0 || robot_dot_positions[1][0] >= kDotWidth || robot_dot_positions[1][1] < 0 || robot_dot_positions[1][1] >= kDotHeight)
    {
        ERROR_MESSAGE(FUNCNAME + "(); now dot is (" + to_string(robot_dot_positions[1][0]) + ", " + to_string(robot_dot_positions[1][1]) + ")", MODE_NORMAL);
    }

    map_cost[robot_dot_positions[1][1]][robot_dot_positions[1][0]] = 0;
    map_from[robot_dot_positions[1][1]][robot_dot_positions[1][0]][0] = robot_dot_positions[1][0];
    map_from[robot_dot_positions[1][1]][robot_dot_positions[1][0]][1] = robot_dot_positions[1][1];
    map_status[robot_dot_positions[1][1]][robot_dot_positions[1][0]] = 1;

    long i = 0;
    while (true)
    {
        i++;
        int investigating_dot_x = -1, investigating_dot_y = -1;

        rep(yi, kDotHeight)
        {
            rep(xj, kDotWidth)
            {
                if (map_status[yi][xj] != 1)
                {
                    continue;
                }
                if (investigating_dot_x == -1 || map_total_cost[yi][xj] < map_total_cost[investigating_dot_y][investigating_dot_x] || (map_total_cost[yi][xj] == map_total_cost[investigating_dot_y][investigating_dot_x] && map_cost[yi][xj] < map_cost[investigating_dot_y][investigating_dot_x]))
                {
                    investigating_dot_x = xj;
                    investigating_dot_y = yi;
                }
            }
        }

        // goalに到着したとき
        if (map_from[goal_y][goal_x][0] != -1)
        {
            break;
        }

        map_status[investigating_dot_y][investigating_dot_x] = 2;
        for (int y = investigating_dot_y - 1; y <= investigating_dot_y + 1; ++y)
        {
            for (int x = investigating_dot_x - 1; x <= investigating_dot_x + 1; ++x)
            {
                if (0 <= x && x < kDotWidth && 0 <= y && y < kDotHeight)
                {
                    if (map_status[y][x] == 2)
                    {
                        continue;
                    }
                    int cost = kCM2DotScale;
                    cost += map_arrived_times[y][x] * 10;

                    if (map[0][y][x] == MAP_YELLOW || map[0][y][x] == MAP_WALL)
                    {
                        cost *= 100000;
                    }
                    if (map[0][y][x] == MAP_SWAMPLAND)
                    {
                        cost *= 1000;
                    }
                    if (map[0][y][x] == MAP_UNKNOWN || map[0][y][x] == MAP_UNKNOWN_NOT_WALL)
                    {
                        if (Time < 60)
                        {
                            cost /= 100;
                            // cost -= 100;
                        }
                        else
                        {
                            cost /= 2;
                        }
                    }
                    if (map[RED_LOADED_ID][y][x] == 1 && loaded_objects[RED_LOADED_ID] < kBorderSameObjNum)
                    {
                        cost /= 10;
                    }
                    if (map[CYAN_LOADED_ID][y][x] == 1 && loaded_objects[CYAN_LOADED_ID] < kBorderSameObjNum)
                    {
                        cost /= 10;
                    }
                    if (map[BLACK_LOADED_ID][y][x] == 1 && loaded_objects[BLACK_LOADED_ID] < kBorderSameObjNum)
                    {
                        cost /= 10;
                    }
                    if (map_status[y][x] == 0 || map_cost[investigating_dot_y][investigating_dot_x] + cost < map_cost[y][x])
                    {
                        map_cost[y][x] = map_cost[investigating_dot_y][investigating_dot_x] + cost;
                        map_total_cost[y][x] = map_cost[y][x] + (abs(goal_x - x) + abs(goal_y - y)) * kCM2DotScale;
                        map_from[y][x][0] = investigating_dot_x;
                        map_from[y][x][1] = investigating_dot_y;
                        map_status[y][x] = 1;
                    }
                }
            }
        }
    }

    cout << "num " << i << endl;
    // 出力
    int max_value = INT_MIN;
    rep(yi, kDotHeight)
    {
        rep(xj, kDotWidth)
        {
            if (map_cost[yi][xj] > max_value)
            {
                max_value = map_cost[yi][xj];
            }
        }
    }
    LOG_MESSAGE(FUNCNAME + "(): max value : " + to_string(max_value), MODE_VERBOSE);
    if (max_value == INT_MIN)
    {
        ERROR_MESSAGE(FUNCNAME + "(): max value is 0", MODE_NORMAL);
        return;
    }
    if (max_value < 0)
    {
        ERROR_MESSAGE(FUNCNAME + "(): max value is " + to_string(max_value), MODE_NORMAL);
    }

    // for (long yi = kDotHeight - 1; yi >= 0; --yi)
    // {
    //     rep(xj, kDotWidth)
    //     {
    //         switch (map[0][yi][xj])
    //         {
    //         case MAP_YELLOW:
    //         case MAP_WALL:
    //             printf("*");
    //             break;
    //         default:
    //             if (map_cost[yi][xj] < 0)
    //             {
    //                 printf("'");
    //             }
    //             else
    //             {
    //                 printf("%d", TO_INT(map_cost[yi][xj] * 9 / max_value));
    //             }
    //         }
    //     }
    //     printf("\n");
    // }
    // printf("\n");
}

float AutoStrategy::sigmoid(float gain, float value, float scale)
{
    return (1.0f / (1.0f + static_cast<float>(exp(static_cast<double>(-gain * value))))) * scale;
}
float AutoStrategy::sigmoid(float value, float scale)
{
    return sigmoid(1, value, scale);
}
int AutoStrategy::i_sigmoid(float gain, float value, float scale)
{
    return TO_INT(sigmoid(gain, value, scale));
}
int AutoStrategy::i_sigmoid(float value, float scale)
{
    return i_sigmoid(1, value, scale);
}
