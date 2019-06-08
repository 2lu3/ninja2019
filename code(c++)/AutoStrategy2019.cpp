#include "AutoStrategy2019.hpp"

using namespace std;

AutoStrategy::~AutoStrategy()
{
}

AutoStrategy::AutoStrategy()
{
    pt.start();
    setRunMode(MODE_DEBUG);
    setDefaultRunMode(MODE_NORMAL);
    setIsOutputLogMessage2Console(false);
    setIsOutputErrorMessage2Console(true);
    pt.print("AutoStrategy::AutoStrategy() :");
}

void AutoStrategy::setup()
{
    pt.start();
    UserGame1::setup();
    logErrorMessage.delErrorFile();
    logErrorMessage.delLogFile();
    cout << "run mode : " << static_cast<int>(getRunMode()) << endl;
    pt.print("AutoStrategy::setup() :");
}

void AutoStrategy::loop()
{
    pt.start();
    UserGame1::loop();
    logErrorMessage.logMessage("Auto Strategy loop start", MODE_DEBUG);

    // static int same_time = 0;
    // static int prev_repeated_num = 0;
    if (PositionX != 0 || PositionY != 0)
    {
        logErrorMessage.logMessage(FUNCNAME + "(): not in PLA and (x, y) is (" + to_string(PositionX) + ", " + to_string(PositionY) + ")", MODE_DEBUG);
        log_x = PositionX;
        log_y = PositionY;
        log_compass = Compass;

        if (log_x < 0)
            log_x = 0;
        if (log_x >= kCospaceWidth)
            log_x = kCospaceWidth - 1;
        if (log_y < 0)
            log_y = 0;
        if (log_y >= kCospaceHeight)
            log_y = kCospaceHeight - 1;
        CheckNowDot();
        int range = 0;
        rep(hi, range * 2 + 1)
        {
            rep(wj, range * 2 + 1)
            {
                // int y = hi + robot_dot_positions[1][1] - range;
                // int x = wj + robot_dot_positions[0][0] - range;
                // if (x < 0 || x >= kDotWidth || y < 0 || y >= kDotHeight)
                // {
                //     continue;
                // }
                // // dot[y * kDotWidthNum + x].arrived_times++;
            }
        }
    }
    else
    {
        PositionX = -1;
        int range = 2;
        for (int wi = 0; wi < range * 2; wi++)
        {
            for (int hj = 0; hj < range * 2; hj++)
            {
                int x = log_x / kCM2DotScale + wi - range;
                int y = log_y / kCM2DotScale + hj - range;
                if (x < 0 || x >= kDotWidth || y < 0 || y >= kDotHeight)
                {
                    continue;
                }
                // dot[y * kDotWidth + x].arrived_times += 2;
            }
        }
    }

    /*
    // 色のデータを保存
    if (IsOnDepositArea())
    {
        cout << "deposit" << endl;
        if (ColorJudgeLeft(object_box))
        {
            map[0][robot_dot_positions[0][1]][robot_dot_positions[0][0]] = MAP_DEPOSIT;
            logMessage(to_string(robot_dot_positions[0][0]) + " " + to_string(robot_dot_positions[0][1]), MODE_VERBOSE);
        }
        if (ColorJudgeRight(object_box))
        {
            map[0][robot_dot_positions[2][1]][robot_dot_positions[2][0]] = MAP_DEPOSIT;
            logMessage(to_string(robot_dot_positions[2][0]) + " " + to_string(robot_dot_positions[2][1]), MODE_VERBOSE);
        }
    }
    if (IsOnYellowLine())
    {
        cout << "yellow" << endl;
        logMessage(to_string(robot_dot_positions[0][0]) + " " + to_string(robot_dot_positions[0][1]), MODE_VERBOSE);
        if (ColorJudgeLeft(trap_line))
        {
            map[0][robot_dot_positions[0][1]][robot_dot_positions[0][0]] = MAP_YELLOW;
            logMessage(to_string(robot_dot_positions[0][0]) + " " + to_string(robot_dot_positions[0][1]), MODE_VERBOSE);
        }
        if (ColorJudgeRight(trap_line))
        {
            map[0][robot_dot_positions[2][1]][robot_dot_positions[2][0]] = MAP_YELLOW;
            logMessage(to_string(robot_dot_positions[2][0]) + " " + to_string(robot_dot_positions[2][1]), MODE_VERBOSE);
        }
    }
    if (IsOnSwampland())
    {
        cout << "swampland" << endl;
        if (ColorJudgeLeft(gray_zone))
        {
            map[0][robot_dot_positions[0][1]][robot_dot_positions[0][0]] = MAP_SWAMPLAND;
            logMessage(to_string(robot_dot_positions[0][0]) + " " + to_string(robot_dot_positions[0][1]), MODE_VERBOSE);
        }
        if (ColorJudgeRight(gray_zone))
        {
            map[0][robot_dot_positions[2][1]][robot_dot_positions[2][0]] = MAP_SWAMPLAND;
            logMessage(to_string(robot_dot_positions[2][0]) + " " + to_string(robot_dot_positions[2][1]), MODE_VERBOSE);
        }
    }
    if (IsOnBlueFloor())
    {
        cout << "blue floor" << endl;
        if (ColorJudgeLeft(blue_zone))
        {
            map[0][robot_dot_positions[0][1]][robot_dot_positions[0][0]] = MAP_SUPER_AREA;
        }
        if (ColorJudgeRight(blue_zone))
        {
            map[0][robot_dot_positions[2][1]][robot_dot_positions[2][0]] = MAP_SUPER_AREA;
        }
    }*/

    // 壁の距離を計算

    {
        int us_sensors[3] = {US_Left, US_Front, US_Right};
        int angles[3] = {-45, 0, 45};
        int calculated_position[3][2];
        rep(i, 3)
        {
            angles[i] += Compass + 90;
            if (us_sensors[i] > 180)
            {
                calculated_position[i][0] = -1;
                calculated_position[i][1] = -1;
                cout << i << " pass; value = " << us_sensors[i] << endl;
                continue;
            }
            // 壁の位置とロボットの相対座標
            calculated_position[i][0] = static_cast<int>(cos(angles[i] * M_PI / 180) * us_sensors[i]);
            calculated_position[i][1] = static_cast<int>(sin(angles[i] * M_PI / 180) * us_sensors[i]);

            // 壁の位置とロボットそれぞれの絶対座標
            int x[2] = {log_x, calculated_position[i][0] + log_x};
            int y[2] = {log_y, calculated_position[i][1] + log_y};
            cout << i << " " << x[1] << " " << y[1] << endl;

            // 0 < 1にする
            if (x[0] > x[1])
            {
                // x[0]とx[1]を入れ替え
                int temp = x[0];
                x[0] = x[1];
                x[1] = temp;
            }
            if (y[0] > y[1])
            {
                // y[0]とy[1]を入れ替え
                int temp = y[0];
                y[0] = y[1];
                y[1] = temp;
            }

            // 傾き
            float tilt;
            if (calculated_position[i][0] == 0)
            {
                tilt = 1000000000;
            }
            else
            {
                tilt = fabs(static_cast<float>(calculated_position[i][1] / calculated_position[i][0]));
            }

            // x[0] -> x[1]まで、順番にyの値を調べ、それぞれのドットにMAP_WHITEを代入していく
            // ただし、x[0]とx[1]はMAP_WHITEを代入しない
            // x[0]かx[1]のうちどちらかは壁である
            // map[0][y][x] = MAP_WALLできるのは、map[0][y][x] == MAP_UNKNOWNのときだけ
            for (int xi = x[0] + 1; xi < x[1]; xi++)
            {
                if (xi < 0 || xi >= kDotWidth)
                {
                    continue;
                }
                for (int yj = static_cast<int>(static_cast<float>(xi - x[0]) * tilt) + y[0]; static_cast<float>(yj - y[0]) <= static_cast<float>(xi - x[0] + 1) * tilt; yj++)
                {
                    if (yj < 0 || yj >= kDotHeight)
                    {
                        continue;
                    }
                    if (map[0][yj][xi] == MAP_UNKNOWN || map[0][yj][xi] == MAP_WALL)
                    {
                        map[0][yj][xi] = MAP_WHITE;
                    }
                }
            }
            calculated_position[i][0] += log_x;
            calculated_position[i][1] += log_y;

            if (calculated_position[i][0] < 0 || calculated_position[i][0] >= kDotWidth || calculated_position[i][1] < 0 || calculated_position[i][1] >= kDotHeight)
            {
                continue;
            }
            if (map[0][calculated_position[i][1]][calculated_position[i][0]] == MAP_UNKNOWN)
            {
                map[0][calculated_position[i][1]][calculated_position[i][0]] = MAP_WALL;
            }
        }
    }
    /*
    if (getRepeatedNum() % 1000 == 0)
    {
        cout << "output" << endl;
        delOutFile("output_data.txt");
        string temp;
        for (long hi = kDotHeight - 1; hi >= 0; --hi)
        {
            rep(wj, kDotWidth)
            {
                // temp += map[0][hi][wj];
                // if (hi % k == k - 1 && wj % k == k - 1)
                // {
                //     if (temp == 0)
                //     {
                //         outputData("output_data.txt", "0", MODE_MATCH);
                //     }
                //     else
                //     {
                //         outputData("output_data.txt", "1", MODE_MATCH);
                //     }
                // }
                switch (map[0][hi][wj])
                {
                case MAP_WALL:
                    temp = '1';
                    break;
                case MAP_WHITE:
                case MAP_UNKNOWN:
                case MAP_SUPER_AREA:
                default:
                    temp = '0';
                    break;
                }
                outputData("output_data.txt", temp, MODE_MATCH);
            }
            outputData("output_data.txt", "\n", MODE_MATCH);
        }
        outputData("output_data.txt", "\n", MODE_MATCH);
        cout << "output finished" << endl;
    }*/

    motor(3, 5);

    /*
    if (SuperObj_Num != 0)
    {
        if (log_superobj_num <= 0 && (log_superobj_x[0] != SuperObj_X || log_superobj_y[0] != SuperObj_Y))
        {
            //I separate this from *1 because log_superobj_x[0 - 1] makes error
            log_superobj_x[0] = SuperObj_X;
            log_superobj_y[0] = SuperObj_Y;
            log_superobj_num++;
        }
        else
        {
            //This avoid record same data many times
            // if the new SuperObject is undefined
            // hint : if log_superobj_num == 0, this doesn't work. so I separate this case
            if (log_superobj_x[log_superobj_num - 1] != SuperObj_X || log_superobj_y[log_superobj_num - 1] != SuperObj_Y)
            {
                log_superobj_x[log_superobj_num] = SuperObj_X;
                log_superobj_y[log_superobj_num] = SuperObj_Y;
                log_superobj_num++;
            }
        }
    }

    if (SuperDuration > 0)
    {
        SuperDuration--;
    }
    else if (IsOnRedObj() && LoadedObjects < 6 && loaded_objects[0] < kBorderSameObjNum && !(LoadedObjects == 5 && log_superobj_num >= 1))
    {
        setAction(FIND_OBJ);
        loaded_objects[0]++;
        SuperDuration = kFindObjDuration;
    }
    else if (IsOnCyanObj() && LoadedObjects < 6 && loaded_objects[1] < kBorderSameObjNum && !(LoadedObjects == 5 && log_superobj_num >= 1))
    {
        setAction(FIND_OBJ);
        loaded_objects[1]++;
        SuperDuration = kFindObjDuration;
    }
    else if (IsOnBlackObj() && LoadedObjects < 6 && loaded_objects[2] < kBorderSameObjNum && !(LoadedObjects == 5 && log_superobj_num >= 1))
    {
        setAction(FIND_OBJ);
        loaded_objects[2]++;
        SuperDuration = kFindObjDuration;
    }
    else if (IsOnSuperObj() && SuperObj_Num == 0 && log_superobj_num > 0 && !(IsOnRedObj() || IsOnBlackObj() || IsOnCyanObj()))
    {
        same_time = 0;
        setAction(FIND_OBJ);
        SuperDuration = kFindObjDuration;
        int min = 10000;
        int num = 0;
        for (int i = 0; i < log_superobj_num; i++)
        {
            if (pow(log_superobj_x[i] - log_x, 2) + pow(log_superobj_y[i] - log_y, 2) < min)
            {
                min = pow(log_superobj_x[i] - log_x, 2) + pow(log_superobj_y[i] - log_y, 2);
                num = i;
            }
        }

        if (min < 1200)
        {
            //I delete log_superobj_x[2], I have to move the data of log_superobj_x[3] to [2].
            for (int i = num + 1; i < log_superobj_num; i++)
            {
                log_superobj_x[i] = log_superobj_x[i - 1];
                log_superobj_y[i] = log_superobj_y[i - 1];
            }
            loaded_objects[3]++;
            log_superobj_num--;
        }
        else
        {
            motor(3, 3);
        }
    }
    else if (Duration > 0)
    {
        Duration--;
    }
    else if (IsOnYellowLine() && LoadedObjects > 0)
    {
        motor(-5, -3);
        Duration = 3;
    }
    else if (IsOnDepositArea() && (LoadedObjects >= 6 || (LoadedObjects > 0 && Time > 270)))
    {
        process = 0;
        if (IsOnDepositArea() == 3)
        {
            setAction(DEPOSIT_OBJ);
            Duration = kFindObjDuration;
        }
        else if (IsOnDepositArea() == 1)
        {
            motor(0, 5);
        }
        else
        {
            motor(5, 0);
        }
    }
    else if (LoadedObjects >= 6 || (Time > 270 && log_superobj_num == 0 && (LoadedObjects > 2 || loaded_objects[SUPER_LOADED_ID] > 0)))
    {
        searching_object = -1;
        GoInDots(180, 135, 180, 135, POINT_DEPOSIT);
    }
    else if (log_superobj_num > 0)
    {
        searching_object = SUPER_LOADED_ID;
        if (GoToDots(log_superobj_x[0], log_superobj_y[0], 0, 0))
        {
            if (prev_repeated_num + 40 < getRepeatedNum())
            {
                same_time = 0;
            }
            if (same_time > 10)
            {
                log_superobj_num = 0;
                logMessage("There is no superobj", MODE_NORMAL);
            }
            GoToPosition(log_superobj_x[0] - 5 + rand() % 10, log_superobj_y[0] - 5 + rand() % 10, 1, 1, 1);
            same_time++;
        }
    }
    else
    {
        if (loaded_objects[BLACK_LOADED_ID] < 2)
        {
            GoInDots(180, 135, 180, 135, POINT_BLACK);
            searching_object = BLACK_LOADED_ID;
        }
        else if (loaded_objects[CYAN_LOADED_ID] < 2)
        {
            GoInDots(180, 135, 180, 135, POINT_CYAN);
            searching_object = CYAN_LOADED_ID;
        }
        else
        {
            GoInDots(180, 135, 180, 135, POINT_RED);
            searching_object = RED_LOADED_ID;
        }
    }*/
    /*
    switch (getAction())
    {
    case DEFINED:
        //defined motor power by motor(int left, int right)
        break;
    case FIND_OBJ:
        if (Duration == kFindObjDuration || SuperDuration == kFindObjDuration)
        {
            LoadedObjects++;
        }
        LED_1 = 1;
        MyState = 0;
        WheelLeft = 0;
        WheelRight = 0;
        if (Duration == 0 && SuperDuration == 0)
        {
            LED_1 = 0;
        }
        break;
    case DEPOSIT_OBJ:
        WheelLeft = 0;
        WheelRight = 0;
        LED_1 = 2;
        MyState = 0;
        LoadedObjects = 0;
        //initialize all value of loaded_objects
        for (int i = 0; i < 4; i++)
        {
            loaded_objects[i] = 0;
        }
        if (Duration == 0 && SuperDuration == 0)
        {
            LED_1 = 0;
        }
        else
        {
            if (!IsOnDepositArea())
            {
                LoadedObjects = 6;
                Duration = 0;
                SuperDuration = 0;
                for (int i = 0; i < 3; i++)
                {
                    loaded_objects[i] = 2;
                }
            }
        }
        break;
    default:
        printf("World2System(): action's value is ubnormal\n");
        break;
    }

    //This is for superobj
    //When there is no superobj, I give up to search superobj
    super_sameoperate++;
    if (log_superobj_num == 0)
    {
        super_sameoperate = 0;
    }
    if (super_sameoperate > 800)
    {
        log_superobj_num = 0;
        super_sameoperate = 0;
    }*/
    double seconds = pt.end();
    logErrorMessage.logMessage("AutoStrategy loop time :" + to_string(seconds) + " ms", MODE_NORMAL);
}

void AutoStrategy::CheckNowDot()
{
    // 左のカラーセンサ、右のカラーセンサ、座標の中心点をそれぞれA,B,Cとすると、
    // 三角形ABCは、一辺5-6の正三角形
    // x,yはそれぞれ左のカラーセンサ、座標センサの場所、右のカラーセンサの場所のx,y座標
    int side = 6;
    int x[3] = {
        static_cast<int>(log_x + cos((Compass + 90 + 30) * M_PI / 180) * side),
        log_x,
        static_cast<int>(log_x + cos((Compass + 90 - 30) * M_PI / 180) * side)};
    int y[3] = {
        static_cast<int>(log_y + sin((Compass + 90 + 30) * M_PI / 180) * side),
        log_y,
        static_cast<int>(log_y + sin((Compass + 90 - 30) * M_PI / 180) * side)};

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
}
