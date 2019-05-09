#include "CalculatePositionTools.hpp"
#include "LogErrorTools.hpp"
#include "CommonTools.hpp"
#include "CospaceSettings.hpp"

using std::cout;
using std::endl;
using std::setw;
using std::string;
using std::to_string;

#define COSPACE_WIDTH 240
#define COSPACE_HEIGHT 180

#define POSSIBILITY_VALUE_MAX 10.0
#define POSSIBILITY_VALUE_MIN 0.1

#define NOTHING 0
#define WALL 2
#define YELLOW 1
#define DEPOSIT 3

CalculatePosition::CalculatePosition(void)
{
    rep(hi, kMapHeight)
    {
        rep(wj, kMapWidth)
        {
            map_possibility[wj][hi] = 0;
            current_map_possibility[wj][hi] = 0;
        }
    }
}

void CalculatePosition::setEquation(int x1, int y1, int x2, int y2)
{
    equation[equation_num][0] = x1;
    equation[equation_num][1] = y1;
    equation[equation_num][2] = x2;
    equation[equation_num][3] = y2;
    equation_num++;
}

// 長方形で、壁を登録する
void CalculatePosition::setEquations(int x1, int y1, int x2, int y2)
{
    // 左の縦棒
    equation[equation_num][0] = x1;
    equation[equation_num][1] = y1;
    equation[equation_num][2] = x1;
    equation[equation_num][3] = y2;
    equation_num++;

    // 下の横棒
    equation[equation_num][0] = x1;
    equation[equation_num][1] = y1;
    equation[equation_num][2] = x2;
    equation[equation_num][3] = y1;
    equation_num++;

    // 右の縦棒
    equation[equation_num][0] = x2;
    equation[equation_num][1] = y1;
    equation[equation_num][2] = x2;
    equation[equation_num][3] = y2;
    equation_num++;

    // 上の横棒
    equation[equation_num][0] = x1;
    equation[equation_num][1] = y2;
    equation[equation_num][2] = x2;
    equation[equation_num][3] = y2;
    equation_num++;
}

// (ax, ay), (bx, by)の線分上に(cx, cy)があるか
int CalculatePosition::judgeOnLineSegmenet(double ax, double ay, double bx, double by, double cx, double cy)
{
    double border = 0.00001;
    double l1 = (bx - ax) * (bx - ax) + (by - ay) * (by - ay);
    double l2 = (cx - ax) * (cx - ax) + (cy - ay) * (cy - ay);
    double c = (bx - ax) * (cx - ax) + (by - ay) * (cy - ay);
    return c >= 0 && PLUSMINUS(c * c, l1 * l2, border) && l1 >= l2;
}

int CalculatePosition::isCross(int num, double x1, double y1, double x2, double y2)
{
    // (x2 - x1)(y - y1) - (x - x1)(y2 - y1) = 0
    // (y2 - y1)x - (x2 - x1)y - (y2 - y1)x1 + (x2 - x1)y1 = 0
    double a1, b1, c1, a2, b2, c2;
    a1 = (y2 - y1);
    b1 = -(x2 - x1);
    c1 = (x2 - x1) * y1 - (y2 - y1) * x1;
    a2 = (equation[num][3] - equation[num][1]);
    b2 = -(equation[num][2] - equation[num][0]);
    c2 = -b2 * equation[num][1] - a2 * equation[num][0];
    double result1 = a2 * x1 + b2 * y1 + c2;
    double result2 = a2 * x2 + b2 * y2 + c2;

    double result3 = a1 * equation[num][0] + b1 * equation[num][1] + c1;
    double result4 = a1 * equation[num][2] + b1 * equation[num][3] + c1;

    return (result1 * result2 < 0 && result3 * result4 < 0) ||
           judgeOnLineSegmenet(x1, x2, y1, y2, equation[num][0], equation[num][1]) ||
           judgeOnLineSegmenet(x1, x2, y1, y2, equation[num][1], equation[num][2]) ||
           judgeOnLineSegmenet(equation[num][0], equation[num][1], equation[num][2], equation[num][3], x1, y1) ||
           judgeOnLineSegmenet(equation[num][0], equation[num][1], equation[num][2], equation[num][3], x2, y2);
}

void CalculatePosition::calculate(int us_left, int us_front, int us_right, int compass)
{
    ProcessingTime pt;
    pt.start();

    // 数学での角度に変換
    compass += 90;

    logMessage(getFuncName(__FUNCTION__) + "(" + to_string(us_left) + "," + to_string(us_front) + "," + to_string(us_right) + "," + to_string(compass) + ") start", MODE_DEBUG);

    float move_x, move_y;
    {
        int motor_speed = abs(WheelLeft) + abs(WheelRight);
        motor_speed /= 2;
        move_x = cos((double)(Compass + 90) / 180 * PI) * motor_speed;
        move_y = sin((double)(Compass + 90) / 180 * PI) * motor_speed;
        // float border = 0.7;
        // if (move_x < -border)
        // {
        //     move_x = -motor_speed;
        // }
        // else if (move_x < border)
        // {
        //     move_x = 0;
        // }
        // else
        // {
        //     move_x = 1;
        // }
        // if (move_y < -border)
        // {
        //     move_y = -1;
        // }
        // else if (move_y < border)
        // {
        //     move_y = 0;
        // }
        // else
        // {
        //     move_y = 1;
        // }
        // rotate or stop
        if (WheelLeft * WheelRight <= 0)
        {
            move_x = 0;
            move_y = 0;
        }
        // move backward
        else if (WheelLeft + WheelRight < 0)
        {
            move_x = -move_x;
            move_y = -move_y;
        }
    }

    double angle[3] = {45, 0, -45};
    int distance[3] = {us_left, us_front, us_right};
    // 0 : x, 1 : y
    double coordinate[3][2];

    rep(i, 3)
    {
        angle[i] = (int)(compass + angle[i]) % 360;
        coordinate[i][0] = cos(angle[i] / 180 * PI) * distance[i];
        coordinate[i][1] = sin(angle[i] / 180 * PI) * distance[i];
        logMessage("calculated wall position " + to_string(coordinate[i][0]) + " " + to_string(coordinate[i][1]), MODE_DEBUG);
    }

    // 0:上 1:左 2:下 3:右
    int margin[4] = {0, 0, 0, 0};
    rep(i, 3)
    {
        if (coordinate[i][1] > margin[0])
        {
            margin[0] = coordinate[i][1];
        }
        if (coordinate[i][0] < margin[1])
        {
            margin[1] = coordinate[i][0];
        }
        if (coordinate[i][1] < margin[2])
        {
            margin[2] = coordinate[i][1];
        }
        if (coordinate[i][0] > margin[3])
        {
            margin[3] = coordinate[i][0];
        }
    }

    // +- 反転
    margin[1] = -margin[1];
    margin[2] = -margin[2];

    rep(i, 4)
    {
        margin[i] -= 5;
        if (margin[i] < 0)
        {
            margin[i] = 0;
        }
    }

    string message = "calculated margin (top, left, down, right) = ";
    rep(i, 4)
    {
        message += to_string(margin[i]) + " ";
    }
    logMessage(message, MODE_DEBUG);

    // 上
    rep(wj, kMapWidth)
    {
        for (int hi = kMapHeight - margin[0]; hi < kMapHeight; ++hi)
        {
            current_map_possibility[wj][hi] = 0;
        }
    }

    // 左
    rep(wj, margin[1])
    {
        rep(hi, kMapHeight)
        {
            current_map_possibility[wj][hi] = 0;
        }
    }

    // 下
    rep(wj, kMapWidth)
    {
        rep(hi, margin[2])
        {
            current_map_possibility[wj][hi] = 0;
        }
    }

    // 右
    for (int wj = kMapWidth - margin[3]; wj < kMapWidth; wj++)
    {
        rep(hi, kMapHeight)
        {
            current_map_possibility[wj][hi] = 0;
        }
    }

    // double difference = SIZE;
    for (int wi = 0; wi < kMapWidth; ++wi)
    {
        // int x = map_wi * map_data_scale + wi;
        for (int hj = 0; hj < kMapHeight; ++hj)
        {
            int map_x = wi / map_data_scale;
            int map_y = hj / map_data_scale;
            if (map_data[map_x][map_y] == WALL)
            {
                current_map_possibility[wi][hj] = 0;
                continue;
            }
            // 上 左 下 右
            if (hj >= kMapHeight - margin[0] || wi < margin[1] || hj < margin[2] || wi > kMapWidth - margin[3])
                continue;
            int complete[3] = {0, 0, 0};
            current_map_possibility[wi][hj] = 0;

            rep(i, 3)
            {
                // ロボットから伸びる超音波の線のうち、x方向とy方向の長さを調べ、それぞれの長さを比例定数として誤差を決める
                double difference_x = 5; //fabs(coordinate[i][0]) / (fabs(coordinate[i][0]) + fabs(coordinate[i][1])) * 5;
                double difference_y = 5; //fabs(coordinate[i][1]) / (fabs(coordinate[i][0]) + fabs(coordinate[i][1])) * 5;
                if (coordinate[i][0] < 0)
                {
                    difference_x = -difference_x;
                }
                if (coordinate[i][1] < 0)
                {
                    difference_y = -difference_y;
                }
                double x_min = wi + coordinate[i][0] - difference_x;
                double y_min = hj + coordinate[i][1] - difference_y;
                double x_big = wi + coordinate[i][0] + difference_x;
                double y_big = hj + coordinate[i][1] + difference_y;
                rep(j, equation_num)
                {
                    int result1 = isCross(j, wi, hj, x_min, y_min);
                    int result2 = isCross(j, wi, hj, x_big, y_big);
                    if (complete[i] == 0 && result1 == 0 && result2 == 1)
                    {
                        complete[i] = 1;
                    }
                    else if (result1 == 1 && result2 == 1)
                    {
                        complete[i] = -1;
                        break;
                    }
                }
                if (complete[i] == -1)
                {
                    break;
                }
            }

            complete[0] += complete[1] + complete[2];

            if (complete[0] < 2)
            {
                current_map_possibility[wi][hj] = 0;
            }
            else if (complete[0] == 2)
            {
                current_map_possibility[wi][hj] = POSSIBILITY_VALUE_MAX / 2;
            }
            else
            {
                current_map_possibility[wi][hj] = POSSIBILITY_VALUE_MAX;
            }
        }
    }

    double k = 0.7;
    rep(wj, kMapWidth)
    {
        rep(hi, kMapHeight)
        {
            map_possibility[wj][hi] *= k;
            int num = 0;
            float add = 0;
            int left = wj - move_x, right = wj;
            int under = hi - move_y, top = hi;
            if (left > right)
            {
                int temp = left;
                left = right;
                right = temp;
            }
            if (under > top)
            {
                int temp = under;
                under = top;
                top = temp;
            }
            // printf("%d %d %d %d\n", left, right, under, top);
            for (int wj_range = left; wj_range <= right; wj_range++)
            {
                for (int hi_range = under; hi_range <= top; hi_range++)
                {

                    if (hi_range < 0 || hi_range >= kMapHeight || wj_range < 0 || wj_range >= kMapWidth)
                    {
                        continue;
                    }
                    num++; // 距離の二乗に反比例
                    // addの合計値に反比例
                    // 可能性が同じものが多い場合は、加える可能性を低くする
                    add += (float)current_map_possibility[wj_range][hi_range] * (1.0 - k);
                }
            }
            // add = add / num;
            map_possibility[wj][hi] += add;
            // printf("%f\n", add);
        }

        // printf("%d calculated %d %d\n", getRepeatedNum(), x * SIZE, y * SIZE);
    }
    float min = 1000000, max = 0;
    int max_id = -1;
    rep(wj, kMapWidth)
    {
        rep(hi, kMapHeight)
        {
            int map_x = wj / map_data_scale;
            int map_y = hi / map_data_scale;
            if (map_data[map_x][map_y] == WALL)
            {
                continue;
            }
            if (map_possibility[wj][hi] < min)
            {
                min = map_possibility[wj][hi];
            }
            if (map_possibility[wj][hi] > max)
            {
                max = map_possibility[wj][hi];
                max_id = hi * kMapWidth + wj;
            }
        }
    }

    // magnificationで、値の広さを調整する
    // correctionで、値の位置を調整する
    float magnification = POSSIBILITY_VALUE_MAX / (max - min);
    float correction = POSSIBILITY_VALUE_MIN - min * magnification;
    rep(wj, kMapWidth)
    {
        rep(hi, kMapHeight)
        {
            int map_x = wj / map_data_scale;
            int map_y = hi / map_data_scale;
            if (map_data[map_x][map_y] == WALL)
            {
                map_possibility[wj][hi] = 0;
                continue;
            }
            map_possibility[wj][hi] = map_possibility[wj][hi] * magnification + correction;
        }
    }

    calculated_y = max_id / kMapWidth;
    calculated_x = max_id - calculated_y * kMapWidth;

    cout << "calculated " << calculated_x << " " << calculated_y << endl;

    // rep(wi, kMapWidth)
    // {
    //     rep(hj, kMapHeight)
    //     {
    //         if (current_map_possibility[wi][hj] >= POSSIBILITY_VALUE_MAX / 2)
    //         {
    //             cout << "calculated " << wi << " " << hj << endl;
    //         }
    //     }
    // }
}

void CalculatePosition::showCurrentMapPossibility()
{
    int scale = 10;
    for (int hi = kMapHeight / scale - 1; hi >= 0; hi--)
    {
        rep(wj, kMapWidth / scale)
        {
            int map_x = wj * scale / map_data_scale;
            int map_y = hi * scale / map_data_scale;
            if (map_data[map_x][map_y] == WALL)
            {
                cout << "###";
                continue;
            }
            int sum = 0;
            rep(y, scale)
            {
                rep(x, scale)
                {
                    sum += current_map_possibility[wj * scale + x][hi * scale + y];
                }
            }
            sum /= scale;

            cout << setw(3) << sum;
        }
        cout << endl;
    }
    cout << endl;
    cout << endl;
}

void CalculatePosition::showMap()
{
    int scale = 10;
    for (int hi = kMapHeight / scale - 1; hi >= 0; hi--)
    {
        rep(wj, kMapWidth / scale)
        {
            int map_x = wj * scale / map_data_scale;
            int map_y = hi * scale / map_data_scale;
            if (map_data[map_x][map_y] == WALL)
            {
                cout << "###";
                continue;
            }
            double sum = 0;
            rep(y, scale)
            {
                rep(x, scale)
                {
                    sum += map_possibility[wj * scale + x][hi * scale + y];
                }
            }
            sum /= scale;

            cout << setw(3) << static_cast<int>(sum);
        }
        cout << endl;
    }
    cout << endl;
    cout << endl;
}
