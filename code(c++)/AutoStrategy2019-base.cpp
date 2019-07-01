#include "AutoStrategy2019.hpp"
#include <stdio.h>

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
}

AutoStrategy::AutoStrategy()
{
    pt.start();
    // setRunMode(MODE_NORMAL);
    setRunMode(MODE_DEBUG);
    // setRunMode(MODE_VERBOSE);
    setDefaultRunMode(MODE_NORMAL);
    setIsOutputLogMessage2Console(false);
    setIsOutputErrorMessage2Console(false);

    pt.print("AutoStrategy::AutoStrategy() :");
}
void AutoStrategy::setup()
{
    pt.start();
    UserGame1::setup();
    logErrorMessage.delErrorFile();
    logErrorMessage.delLogFile();
    logErrorMessage.delOutFile("out.txt");

    system("cls");
    system("chcp 65001");
    resetLoadedObjects();

    if (PositionX <= 0 && PositionY <= 0)
    {
        LOG_MESSAGE(FUNCNAME + "(): I am in PLA. So I substituted emergencyX, Y", MODE_NORMAL);
        pos_x = kEmergencyPosX;
        pos_y = kEmergencyPosY;
    }
    else
    {
        kEmergencyPosX = PositionX;
        kEmergencyPosY = PositionY;
    }

    cout << "run mode : " << TO_INT(getRunMode()) << endl;
    pt.print("AutoStrategy::setup() :");
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
        if (cospaceMap.getMapInfo(x[i], y[i]) == cospaceMap.MAP_WALL)
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
                    if (cospaceMap.getMapInfo(temp_x, temp_y) != cospaceMap.MAP_WALL)
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
                y[i] = kEmergencyPosX / kCM2DotScale;
                x[i] = kEmergencyPosY / kCM2DotScale;
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
        LOG_MESSAGE("(" + to_string(absolute_x) + "," + to_string(absolute_y) + ")に到着しました", MODE_MATCH);
        absolute_x = -1;
        absolute_y = -1;
        same_operate = -1;
        absolute_distance = -1;
        return 1;
    }

    LOG_MESSAGE(FUNCNAME + "(): calculated ab(" + to_string(absolute_x) + "," + to_string(absolute_y) + ")", MODE_NORMAL);
    x = absolute_x;
    y = absolute_y;
    x = x - temp_x;
    y = y - temp_y;
    LOG_MESSAGE(FUNCNAME + "(): x, y = " + to_string(x) + ", " + to_string(y), MODE_NORMAL);
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

int AutoStrategy::isNearTheFloor(CospaceMap::MapInfo color, int x, int y, int cm_radius)
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
            if (cospaceMap.getMapInfo(xi, yj) == color)
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

    // double magnification = 0.5;
    // int distance_from_wall = 30;
    int big_motor = 5;
    int short_motor = 3;
    if (isNearTheFloor(cospaceMap.MAP_YELLOW, robot_dot_positions[1][0], robot_dot_positions[1][1], kCM2DotScale))
    {
        big_motor = 4;
        short_motor = 2;
    }
    // int short_front = 3; //TO_INT(pow(US_Front, magnification) * (5 - (WheelLeft * WheelLeft + WheelRight * WheelRight) / 10) / pow(distance_from_wall, magnification));
    // int short_left = 3;  //TO_INT(pow(US_Left, magnification) * (5 - (WheelLeft * WheelLeft + WheelRight * WheelRight) / 10) / pow(distance_from_wall, magnification));
    // int short_right = 3; //TO_INT(pow(US_Right, magnification) * (5 - (WheelLeft * WheelLeft + WheelRight * WheelRight) / 10) / pow(distance_from_wall, magnification));
    // if (short_front < 0)
    //     short_front = 0;
    // if (short_front > 5)
    //     short_front = 5;
    // if (short_right < 0)
    //     short_right = 0;
    // if (short_right > 5)
    //     short_right = 5;
    // if (short_left < 0)
    //     short_left = 0;
    // if (short_left > 5)
    //     short_left = 5;
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
            // motor(5, short_left);
            motor(big_motor, short_motor);
        }
        else if (classification == 2 && abs(angle) < 30)
        { //front
            if (angle < 0)
            {
                // motor(5, short_front);
                motor(big_motor, short_motor);
            }
            else
            {
                // motor(short_front, 5);
                motor(short_motor, big_motor);
            }
        }
        else if (classification == 3 && angle > -30 && angle < 90)
        { //left & front
            //motor(5, (short_left < short_front) ? (short_left) : (short_right));
            motor(big_motor, short_motor);
        }
        else if (classification == 4 && angle < 0 && angle > -90)
        { //right
            //motor(short_right, 5);
			motor(short_motor, big_motor);
        }
        else if (classification == 5 && abs(angle) > 30)
        { //left & right
            if (abs(angle) < 150)
            {
				motor(big_motor, big_motor);
                //motor(5, 5);
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
            //motor((short_right < short_front) ? (short_right) : (short_right), 5);
			motor(short_motor, big_motor);
        }
        else if (classification == 7)
        { //all
            if (angle < 0)
            {
                //motor(5, short_front);
				motor(big_motor, short_motor);
            }
            else
            {
                //motor(short_front, 5);
				motor(short_motor, big_motor);
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
                if (abs(angle) < 10)
                {
                    motor(5, 5);
                }
                else if (abs(angle) < 30)
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
            else if (isNearTheFloor(cospaceMap.MAP_YELLOW, robot_dot_positions[1][0], robot_dot_positions[1][1], 30) || isNearTheFloor(cospaceMap.MAP_UNKNOWN, robot_dot_positions[1][0], robot_dot_positions[1][1], 30))
            {
                LOG_MESSAGE("near yellow or unknown", MODE_DEBUG);
                if (abs(angle) < 5)
                {
                    motor(3, 3);
                }
                else if (abs(angle) < 20)
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
                else if (abs(angle) < 70 && LoadedObjects != 6)
                {
                    if (angle < 0)
                    {
                        motor(4, 2);
                    }
                    else
                    {
                        motor(2, 4);
                    }
                }
                else if (abs(angle) < 90)
                {
                    if (angle < 0)
                    {
                        motor(4, 1);
                    }
                    else
                    {
                        motor(1, 4);
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

void AutoStrategy::Dijkstra(void)
{
    // 初期化
    rep(yi, kDotHeight)
    {
        rep(xj, kDotWidth)
        {
            cospaceMap.setMapFrom(xj, yi, -1, -1);
            cospaceMap.setMapStatus(xj, yi, 0);
        }
    }

    if (robot_dot_positions[1][0] < 0 || robot_dot_positions[1][0] >= kDotWidth || robot_dot_positions[1][1] < 0 || robot_dot_positions[1][1] >= kDotHeight)
    {
        ERROR_MESSAGE(FUNCNAME + "(); now dot is (" + to_string(robot_dot_positions[1][0]) + ", " + to_string(robot_dot_positions[1][1]) + ")", MODE_NORMAL);
    }

    cospaceMap.setMapCost(robot_dot_positions[1][0], robot_dot_positions[1][1], 0);
    cospaceMap.setMapFrom(robot_dot_positions[1][0], robot_dot_positions[1][1], robot_dot_positions[1][0], robot_dot_positions[1][1]);
    cospaceMap.setMapStatus(robot_dot_positions[1][0], robot_dot_positions[1][1], 1);

    while (true)
    {
        int investigating_dot_x = -1, investigating_dot_y = -1;

        rep(yi, kDotHeight)
        {
            rep(xj, kDotWidth)
            {
                if (cospaceMap.getMapStatus(xj, yi) != 1)
                {
                    continue;
                }
                if (investigating_dot_x == -1 || cospaceMap.getMapCost(xj, yi) < cospaceMap.getMapCost(investigating_dot_x, investigating_dot_y))
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

        cospaceMap.setMapStatus(investigating_dot_x, investigating_dot_y, 2);
        for (int y = investigating_dot_y - 1; y <= investigating_dot_y + 1; ++y)
        {
            for (int x = investigating_dot_x - 1; x <= investigating_dot_x + 1; ++x)
            {
                if (0 <= x && x < kDotWidth && 0 <= y && y < kDotHeight)
                {
                    if (cospaceMap.getMapStatus(x, y) == 2)
                    {
                        continue;
                    }
                    int cost = kCM2DotScale * 10000;
                    // cost += map_arrived_times[y][x] * 10;

                    if (cospaceMap.getMapInfo(x, y) == cospaceMap.MAP_YELLOW || cospaceMap.getMapInfo(x, y) == cospaceMap.MAP_WALL)
                    {
                        cost *= 100000;
                    }
                    if (cospaceMap.getMapInfo(x, y) == cospaceMap.MAP_SWAMPLAND)
                    {
                        cost *= 1000;
                    }
                    if (cospaceMap.getMapInfo(x, y) == cospaceMap.MAP_UNKNOWN)
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
                    if (cospaceMap.getMapObjInfo(x, y, RED_LOADED_ID) > 0 && loaded_objects[RED_LOADED_ID] < kBorderSameObjNum)
                    {
                        cost /= 10;
                    }
                    if (cospaceMap.getMapObjInfo(x, y, CYAN_LOADED_ID) > 0 && loaded_objects[CYAN_LOADED_ID] < kBorderSameObjNum)
                    {
                        cost /= 10;
                    }
                    if (cospaceMap.getMapObjInfo(x, y, BLACK_LOADED_ID) > 0 && loaded_objects[BLACK_LOADED_ID] < kBorderSameObjNum)
                    {
                        cost /= 10;
                    }
                    if (cospaceMap.getMapStatus(x, y) == 0 || cospaceMap.getMapCost(investigating_dot_x, investigating_dot_y) + cost < cospaceMap.getMapCost(x, y))
                    {
                        cospaceMap.setMapCost(x, y, cospaceMap.getMapInfo(investigating_dot_x, investigating_dot_y) + cost);
                        cospaceMap.setMapFrom(x, y, investigating_dot_x, investigating_dot_y);
                        cospaceMap.setMapStatus(x, y, 1);
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
            if (cospaceMap.getMapCost(xj, yi) > max_value)
            {
                max_value = cospaceMap.getMapCost(xj, yi);
            }
        }
    }
    LOG_MESSAGE(FUNCNAME + "(): max value : " + to_string(max_value), MODE_VERBOSE);
    if (max_value <= 0)
    {
        ERROR_MESSAGE(FUNCNAME + "(): max value is " + to_string(max_value), MODE_NORMAL);
        return;
    }

    // for (long yi = kDotHeight - 1; yi >= 0; --yi)
    // {
    //     rep(xj, kDotWidth)
    //     {
    //         switch (map[0][yi][xj])
    //         {
    //         case cospaceMap.MAP_YELLOW:
    //         case cospaceMap.MAP_WALL:
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

AutoStrategy::CospaceMap::CospaceMap()
{
    rep(i, TO_INT((extent<decltype(map), 0>::value)))
    {
        rep(j, kDotHeight)
        {
            rep(k, kDotWidth)
            {
                map[i][j][k] = 0;
            }
        }
    }
    rep(i, kDotHeight)
    {
        rep(j, kDotWidth)
        {
            map_curved_times[i][j] = 0;
        }
    }
}
int AutoStrategy::CospaceMap::getMapCurvedTimes(int from_x, int from_y, int target_x, int target_y)
{
    LOG_MESSAGE(FUNCNAME + "(" + to_string(from_x) + ", " + to_string(from_y) + ", " + to_string(target_x) + "," + to_string(target_y) + "): start", MODE_DEBUG);

    if (from_x < 0 || from_x >= kDotWidth || from_y < 0 || from_y >= kDotHeight)
    {
        ERROR_MESSAGE(FUNCNAME + "(): from(" + to_string(from_x) + ", " + to_string(from_y) + "); target(" + to_string(target_x) + "," + to_string(target_y) + ")", MODE_NORMAL);
    }

    if (map_from[from_y][from_x][0] == -1)
    {
        ERROR_MESSAGE(FUNCNAME + "(): there is no map_from value in (" + to_string(from_x) + ", " + to_string(from_y) + ")", MODE_NORMAL);
    }

    // (previous_ x, y) -> (from_ x, y) -> (target_ x, y)
    int previous_x = map_from[from_y][from_x][0];
    int previous_y = map_from[from_y][from_x][1];
    if (abs(previous_x - from_x) > 1 || abs(previous_y - from_y) > 1)
    {
        ERROR_MESSAGE(FUNCNAME + "(): the difference between previous_(x or y) and from(x or y) > 1", MODE_NORMAL);
    }
    int previous_direction = (previous_y - from_y) * 3 + previous_x - from_x;
    int now_direction = (from_y - target_y) * 3 + from_x - target_x;
    if (previous_direction == now_direction)
    {
        return map_curved_times[from_y][from_x];
    }
    else
    {
        return map_curved_times[from_y][from_x] + 1;
    }
}

void AutoStrategy::saveColorInfo(void)
{
    if (ColorJudgeLeft(object_box))
    {
        cospaceMap.setMapInfo(robot_dot_positions[0][0], robot_dot_positions[0][1], cospaceMap.MAP_DEPOSIT);
        LOG_MESSAGE("deposit " + to_string(robot_dot_positions[0][0] * kCM2DotScale) + " " + to_string(robot_dot_positions[0][1] * kCM2DotScale), MODE_VERBOSE);
    }
    else if (ColorJudgeLeft(trap_line))
    {
        cospaceMap.setMapInfo(robot_dot_positions[0][0], robot_dot_positions[0][1], cospaceMap.MAP_YELLOW);
        LOG_MESSAGE("yellow " + to_string(robot_dot_positions[0][0] * kCM2DotScale) + " " + to_string(robot_dot_positions[0][1] * kCM2DotScale), MODE_VERBOSE);
    }
    else if (ColorJudgeLeft(gray_zone))
    {
        for (int yi = robot_dot_positions[0][1] - TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); yi <= robot_dot_positions[0][1] + TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); ++yi)
        {
            if (yi < 0 || yi >= kDotHeight)
            {
                continue;
            }
            for (int xj = robot_dot_positions[0][0] - TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); xj <= robot_dot_positions[0][0] + TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); ++xj)
            {
                if (xj < 0 || xj >= kDotWidth)
                {
                    continue;
                }
                if (cospaceMap.getMapInfo(xj, yi) == cospaceMap.MAP_UNKNOWN)
                {
                    cospaceMap.setMapInfo(robot_dot_positions[0][0], robot_dot_positions[0][1], cospaceMap.MAP_SWAMPLAND);
                    cospaceMap.setMapInfo(xj, yi, cospaceMap.MAP_SWAMPLAND);
                    LOG_MESSAGE("swampland " + to_string(xj) + " " + to_string(yi), MODE_VERBOSE);
                }
            }
        }
    }
    else if (ColorJudgeLeft(blue_zone))
    {
        cospaceMap.setMapInfo(robot_dot_positions[0][0], robot_dot_positions[0][1], cospaceMap.MAP_SUPER_AREA);
        LOG_MESSAGE("blue floor " + to_string(robot_dot_positions[0][0]) + " " + to_string(robot_dot_positions[0][1]), MODE_VERBOSE);
    }
    else if (ColorJudgeLeft(black_obj))
    {
        for (int yi = robot_dot_positions[0][1] - TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); yi <= robot_dot_positions[0][1] + TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); ++yi)
        {
            if (yi < 0 || yi >= kDotHeight)
            {
                continue;
            }
            for (int xj = robot_dot_positions[0][0] - TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); xj <= robot_dot_positions[0][0] + TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); ++xj)
            {
                if (xj < 0 || xj >= kDotWidth)
                {
                    continue;
                }

                cospaceMap.setMapObjInfo(robot_dot_positions[0][0], robot_dot_positions[0][1], BLACK_LOADED_ID);
                LOG_MESSAGE("black obj " + to_string(robot_dot_positions[0][0]) + " " + to_string(robot_dot_positions[0][1]), MODE_VERBOSE);
            }
        }
    }
    else if (ColorJudgeLeft(cyan_obj))
    {
        for (int yi = robot_dot_positions[0][1] - TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); yi <= robot_dot_positions[0][1] + TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); ++yi)
        {
            if (yi < 0 || yi >= kDotHeight)
            {
                continue;
            }
            for (int xj = robot_dot_positions[0][0] - TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); xj <= robot_dot_positions[0][0] + TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); ++xj)
            {
                if (xj < 0 || xj >= kDotWidth)
                {
                    continue;
                }
                cospaceMap.setMapObjInfo(robot_dot_positions[2][0], robot_dot_positions[2][1], CYAN_LOADED_ID);
                LOG_MESSAGE("cyan obj " + to_string(robot_dot_positions[2][0]) + " " + to_string(robot_dot_positions[2][1]), MODE_VERBOSE);
            }
        }
    }
    else if (ColorJudgeLeft(red_obj))
    {
        for (int yi = robot_dot_positions[0][1] - TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); yi <= robot_dot_positions[0][1] + TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); ++yi)
        {
            if (yi < 0 || yi >= kDotHeight)
            {
                continue;
            }
            for (int xj = robot_dot_positions[0][0] - TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); xj <= robot_dot_positions[0][0] + TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); ++xj)
            {
                if (xj < 0 || xj >= kDotWidth)
                {
                    continue;
                }
                cospaceMap.setMapObjInfo(robot_dot_positions[2][0], robot_dot_positions[2][1], RED_LOADED_ID);
                LOG_MESSAGE("red obj " + to_string(robot_dot_positions[2][0]) + " " + to_string(robot_dot_positions[2][1]), MODE_VERBOSE);
            }
        }
    }
    else if (ColorJudgeLeft(white_zone))
    {
        cospaceMap.setMapInfo(robot_dot_positions[0][0], robot_dot_positions[0][1], cospaceMap.MAP_WHITE);
        LOG_MESSAGE("white area " + to_string(robot_dot_positions[0][0]) + " " + to_string(robot_dot_positions[0][1]), MODE_VERBOSE);
    }
    else
    {
        cospaceMap.setMapInfo(robot_dot_positions[0][0], robot_dot_positions[0][1], cospaceMap.MAP_WHITE);
        LOG_MESSAGE("white area(maye be) " + to_string(robot_dot_positions[0][0]) + " " + to_string(robot_dot_positions[0][1]), MODE_VERBOSE);
    }

    if (ColorJudgeRight(object_box))
    {
        cospaceMap.setMapInfo(robot_dot_positions[2][0], robot_dot_positions[2][1], cospaceMap.MAP_DEPOSIT);
        LOG_MESSAGE("deposit " + to_string(robot_dot_positions[2][0]) + " " + to_string(robot_dot_positions[2][1]), MODE_VERBOSE);
    }
    else if (ColorJudgeRight(trap_line))
    {
        cospaceMap.setMapInfo(robot_dot_positions[2][0], robot_dot_positions[2][1], cospaceMap.MAP_YELLOW);
        LOG_MESSAGE(to_string(robot_dot_positions[2][0]) + " " + to_string(robot_dot_positions[2][1]), MODE_VERBOSE);
    }
    else if (ColorJudgeRight(gray_zone))
    {
        for (int yi = robot_dot_positions[2][1] - TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); yi <= robot_dot_positions[2][1] + TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); ++yi)
        {
            if (yi < 0 || yi >= kDotHeight)
            {
                continue;
            }
            for (int xj = robot_dot_positions[2][0] - TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); xj <= robot_dot_positions[2][0] + TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); ++xj)
            {
                if (xj < 0 || xj >= kDotWidth)
                {
                    continue;
                }
                if (cospaceMap.getMapInfo(xj, yi) == cospaceMap.MAP_UNKNOWN)
                {
                    cospaceMap.setMapInfo(xj, yi, cospaceMap.MAP_SWAMPLAND);
                    LOG_MESSAGE("swampland " + to_string(xj) + " " + to_string(yi), MODE_VERBOSE);
                }
            }
        }
    }
    else if (ColorJudgeRight(blue_zone))
    {
        cospaceMap.setMapInfo(robot_dot_positions[2][0], robot_dot_positions[2][1], cospaceMap.MAP_SUPER_AREA);
        LOG_MESSAGE("blue floor " + to_string(robot_dot_positions[2][0]) + " " + to_string(robot_dot_positions[2][1]), MODE_VERBOSE);
    }
    else if (ColorJudgeRight(black_obj))
    {
        for (int yi = robot_dot_positions[2][1] - TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); yi <= robot_dot_positions[2][1] + TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); ++yi)
        {
            if (yi < 0 || yi >= kDotHeight)
            {
                continue;
            }
            for (int xj = robot_dot_positions[2][0] - TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); xj <= robot_dot_positions[2][0] + TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); ++xj)
            {
                if (xj < 0 || xj >= kDotWidth)
                {
                    continue;
                }
                cospaceMap.setMapObjInfo(robot_dot_positions[2][0], robot_dot_positions[2][1], BLACK_LOADED_ID);
                LOG_MESSAGE("black obj " + to_string(robot_dot_positions[2][0]) + " " + to_string(robot_dot_positions[2][1]), MODE_VERBOSE);
            }
        }
    }
    else if (ColorJudgeRight(cyan_obj))
    {
        for (int yi = robot_dot_positions[2][1] - TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); yi <= robot_dot_positions[2][1] + TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); ++yi)
        {
            if (yi < 0 || yi >= kDotHeight)
            {
                continue;
            }
            for (int xj = robot_dot_positions[2][0] - TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); xj <= robot_dot_positions[2][0] + TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); ++xj)
            {
                if (xj < 0 || xj >= kDotWidth)
                {
                    continue;
                }
                cospaceMap.setMapObjInfo(robot_dot_positions[2][0], robot_dot_positions[2][1], CYAN_LOADED_ID);
                LOG_MESSAGE("cyan obj " + to_string(robot_dot_positions[2][0]) + " " + to_string(robot_dot_positions[2][1]), MODE_VERBOSE);
            }
        }
    }
    else if (ColorJudgeRight(red_obj))
    {
        for (int yi = robot_dot_positions[2][1] - TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); yi <= robot_dot_positions[2][1] + TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); ++yi)
        {
            if (yi < 0 || yi >= kDotHeight)
            {
                continue;
            }
            for (int xj = robot_dot_positions[2][0] - TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); xj <= robot_dot_positions[2][0] + TO_INT(cospaceMap.kGuessedMapSize / kCM2DotScale); ++xj)
            {
                if (xj < 0 || xj >= kDotWidth)
                {
                    continue;
                }
                cospaceMap.setMapObjInfo(robot_dot_positions[2][0], robot_dot_positions[2][1], RED_LOADED_ID);
                LOG_MESSAGE("red obj " + to_string(robot_dot_positions[2][0]) + " " + to_string(robot_dot_positions[2][1]), MODE_VERBOSE);
            }
        }
    }
    else if (ColorJudgeRight(white_zone))
    {
        cospaceMap.setMapInfo(robot_dot_positions[2][0], robot_dot_positions[2][1], cospaceMap.MAP_WHITE);
        LOG_MESSAGE("white area " + to_string(robot_dot_positions[2][0]) + " " + to_string(robot_dot_positions[2][1]), MODE_VERBOSE);
    }
    else
    {
        cospaceMap.setMapInfo(robot_dot_positions[2][0], robot_dot_positions[2][1], cospaceMap.MAP_WHITE);
        LOG_MESSAGE("white area(may be) " + to_string(robot_dot_positions[2][0]) + " " + to_string(robot_dot_positions[2][1]), MODE_VERBOSE);
    }
}

void AutoStrategy::calculateWallPosition(void)
{

    if (PositionX != -1)
    {
        LOG_MESSAGE(FUNCNAME + "():" + "壁の位置の計算を開始", MODE_DEBUG);

        // 0: left & right 1: front
        int difference_us_position[2] = {9, 9};
        int us_sensors[3] = {US_Left, US_Front, US_Right};
        LOG_MESSAGE(FUNCNAME + "(): " + "US " + to_string(US_Left) + " " + to_string(US_Front) + " " + to_string(US_Right) + " Compass: " + to_string(Compass), MODE_DEBUG);
        string us_names[3] = {"Left", "Front", "Right"};
        int angles[3] = {40, 0, -40};
        int calculated_relative_coordinate[3][2];
        int calculated_absolute_dot_position[3][2];
        for (int i = 0; i < 3; ++i)
        {

            angles[i] += Compass + 90;
            angles[i] %= 360;
            LOG_MESSAGE(FUNCNAME + "(): US(" + us_names[i] + "): " + to_string(us_sensors[i]) + " Compass: " + to_string(angles[i]), MODE_VERBOSE);
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
            LOG_MESSAGE(FUNCNAME + "(): calculated wall position us: " + us_names[i] + " pos: " + to_string(pos_x + calculated_relative_coordinate[i][0]) + "," + to_string(pos_y + calculated_relative_coordinate[i][1]) + " registered pos:" + to_string(calculated_absolute_dot_position[i][0] * kCM2DotScale) + "," + to_string(calculated_absolute_dot_position[i][1] * kCM2DotScale), MODE_VERBOSE);
            LOG_MESSAGE(FUNCNAME + "(): calculated positions (" + to_string(robot_dot_positions[1][0]) + ", " + to_string(robot_dot_positions[1][1]) + ") -> (" + to_string(calculated_absolute_dot_position[i][0]) + ", " + to_string(calculated_absolute_dot_position[i][1]) + ")", MODE_VERBOSE);
            if (0 <= calculated_absolute_dot_position[i][0] && calculated_absolute_dot_position[i][0] < kDotWidth && 0 <= calculated_absolute_dot_position[i][1] && calculated_absolute_dot_position[i][1] < kDotHeight)
            {
                // 壁はないときは、MAP_WALLを登録する必要がない
                if (us_sensors[i] < kUSLimit + difference_us_position[i % 2])
                {
                    // if (map[0][calculated_absolute_dot_position[i][1]][calculated_absolute_dot_position[i][0]] == cospaceMap.MAP_UNKNOWN || map[0][calculated_absolute_dot_position[i][1]][calculated_absolute_dot_position[i][0]] == MAP_UNKNOWN_NOT_WALL)
                    {
                        cospaceMap.addMapInfo(calculated_absolute_dot_position[i][0], calculated_absolute_dot_position[i][1], cospaceMap.MAP_WALL, 2);
                        LOG_MESSAGE(FUNCNAME + "(): set here as Wall; pos: " + to_string(calculated_absolute_dot_position[i][0] * kCM2DotScale) + "," + to_string(calculated_absolute_dot_position[i][1] * kCM2DotScale), MODE_VERBOSE);
                    }
                }
            }

            // x[0], y[0] -> x[1], y[1]までMAP_WALLをcospaceMap.MAP_WHITEに変更する
            // 壁の位置(壁から多少離れた位置)とロボットそれぞれの絶対座標
            // 基本的に、実際の壁との距離から0.7倍程度にするが、kCM2DotScaleが最低2つはあけないといけない
            // 1cm先に壁がある場合、cospaceMap.MAP_WHITEは登録しない
            const int kRange4Wall = 20;

            if (us_sensors[i] < kRange4Wall + difference_us_position[i % 2])
            {
                // cospaceMap.MAP_WHITEは登録しない
                LOG_MESSAGE(FUNCNAME + "(): cospaceMap.MAP_WHITEは、壁との距離が非常に近いため設定しません", MODE_VERBOSE)
                continue;
            }
            if (us_sensors[i] * 0.3 < kRange4Wall)
            {
                LOG_MESSAGE(FUNCNAME + "(): us_sensors[i](" + to_string(us_sensors[i]) + ") * 0.3 < kRange4Wall(" + to_string(kRange4Wall) + ")", MODE_VERBOSE);
                calculated_relative_coordinate[i][0] = TO_INT(cos(angles[i] * M_PI / 180) * (us_sensors[i] - kRange4Wall));
                calculated_relative_coordinate[i][1] = TO_INT(sin(angles[i] * M_PI / 180) * (us_sensors[i] - kRange4Wall));
            }
            else
            {
                LOG_MESSAGE(FUNCNAME + "(): us_sensors[i](" + to_string(us_sensors[i]) + ") * 0.3 >= kRange4Wall(" + to_string(kRange4Wall) + ")", MODE_VERBOSE);
                calculated_relative_coordinate[i][0] = TO_INT(cos(angles[i] * M_PI / 180) * us_sensors[i] * 0.7);
                calculated_relative_coordinate[i][1] = TO_INT(sin(angles[i] * M_PI / 180) * us_sensors[i] * 0.7);
            }

            const int x[2] = {robot_dot_positions[1][0], TO_INT((pos_x + calculated_relative_coordinate[i][0] + kCM2DotScale / 2) / kCM2DotScale)};
            const int y[2] = {robot_dot_positions[1][1], TO_INT((pos_y + calculated_relative_coordinate[i][1] + kCM2DotScale / 2) / kCM2DotScale)};

            LOG_MESSAGE(FUNCNAME + "(): Set MAP_UNKNOWN (" + to_string(x[0]) + ", " + to_string(y[0]) + ") -> (" + to_string(x[1]) + ", " + to_string(y[1]) + ")", MODE_VERBOSE);

            // (x[0], y[0]) -> (x[1], y[1])まで、MAP_WALLをMAP_UNKNOWN_NOT_WALLに変更する
            if (x[0] == x[1]) // 縦方向の直線の場合
            {
                if (0 <= x[0] && x[0] < kDotWidth)
                {
                    int y_start = y[0], y_end = y[1];
                    if (y_end == calculated_absolute_dot_position[i][1])
                    {
                        if (y_start < y_end)
                        {
                            --y_end;
                        }
                        else if (y_start > y_end)
                        {
                            ++y_end;
                        }
                        else
                        {
                            LOG_MESSAGE(FUNCNAME + "(): 壁近くのドットを保護した結果、MAP_WHITEは設定できません", MODE_VERBOSE);
                            continue;
                        }
                    }
                    // 上から変更しようと、下から変更しようと変わらない
                    if (y[0] > y[1])
                    {
                        int temp = y_start;
                        y_start = y_end;
                        y_end = temp;
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
                        if (cospaceMap.getMapInfo(x[0], yi) == cospaceMap.MAP_WALL)
                        {
                            cospaceMap.addMapInfo(x[0], yi, cospaceMap.MAP_WALL, -1);
                            LOG_MESSAGE(FUNCNAME + "(): decrease the possibility of wall pos (" + to_string(x[0] * kCM2DotScale) + ", " + to_string(yi * kCM2DotScale), MODE_VERBOSE);
                        }
                        LOG_MESSAGE(FUNCNAME + "(): (" + to_string(x[0] * kCM2DotScale) + ", " + to_string(yi * kCM2DotScale) + "): here is not wall; wall(" + to_string(calculated_absolute_dot_position[i][0]) + ", " + to_string(calculated_absolute_dot_position[i][1]) + ")", MODE_VERBOSE);
                    }
                }
            }
            else
            {
                double tilt = static_cast<double>(y[1] - y[0]) / static_cast<double>(x[1] - x[0]);
                int x_start = x[0], x_end = x[1];
                if (x_end == calculated_absolute_dot_position[i][0])
                {
                    if (x_start < x_end)
                    {
                        --x_end;
                    }
                    else if (x_start > x_end)
                    {
                        ++x_end;
                    }
                    else
                    {
                        LOG_MESSAGE(FUNCNAME + "(): 壁近くのドットを保護した結果、MAP_WHITEは設定できません", MODE_VERBOSE);
                        continue;
                    }
                }
                if (x_start > x_end)
                {
                    int temp = x_start;
                    x_start = x_end;
                    x_end = temp;
                    tilt = -tilt;
                }
                LOG_MESSAGE(FUNCNAME + "(): tilt is " + to_string(tilt), MODE_VERBOSE);
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
                    int y_start = y[0] + TO_INT(tilt * static_cast<double>(xi - x_start));
                    int y_end = y[0] + TO_INT(floor(tilt * (static_cast<double>(xi + 1 - x_start))));
                    if ((y_start - calculated_absolute_dot_position[i][1]) * (y_end - calculated_absolute_dot_position[i][1]) <= 0)
                    {
                        LOG_MESSAGE(FUNCNAME + "(): 壁近くのドットを保護した結果、MAP_WHITEは設定できません", MODE_VERBOSE);
                        continue;
                    }
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
                        if (cospaceMap.getMapInfo(xi, yj) == cospaceMap.MAP_WALL)
                        {
                            cospaceMap.addMapInfo(xi, yj, cospaceMap.MAP_WALL, -1);
                            LOG_MESSAGE(FUNCNAME + "(): decrease the possibility of wall pos (" + to_string(xi * kCM2DotScale) + ", " + to_string(yj * kCM2DotScale), MODE_VERBOSE);
                        }
                        LOG_MESSAGE(FUNCNAME + "(): (" + to_string(xi * kCM2DotScale) + ", " + to_string(yj * kCM2DotScale) + "): here is not wall; wall(" + to_string(calculated_absolute_dot_position[i][0]) + ", " + to_string(calculated_absolute_dot_position[i][1]) + ")", MODE_VERBOSE);
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

        // // x[0] -> x[1]まで、順番にyの値を調べ、それぞれのドットにcospaceMap.MAP_WHITEを代入していく
        // // ただし、x[0]とx[1]はcospaceMap.MAP_WHITEを代入しない
        // // x[0]かx[1]のうちどちらかは壁である
        // // map[0][y][x] = cospaceMap.MAP_WALLできるのは、map[0][y][x] == MAP_UNKNOWNのときだけ
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
        //         if (map[0][yj][xi] == cospaceMap.MAP_WALL)
        //         {
        //             map[0][yj][xi] = MAP_UNKNOWN_NOT_WALL;
        //             LOG_MESSAGE(FUNCNAME + "(): set here as unknow space; pos:" + to_string(xi * kCM2DotScale) + "," + to_string(yj * kCM2DotScale), MODE_VERBOSE);
        //         }
        //     }
        // }

        // }
    }
}
