#include "CalculatePositionTools.hpp"
#include "LogErrorTools.hpp"
#include "CommonTools.hpp"
#include "CospaceSettings.hpp"

using namespace std;

#define COSPACE_WIDTH 240
#define COSPACE_HEIGHT 180
#define SIZE 1

#define MAP_WIDTH (COSPACE_WIDTH / SIZE)
#define MAP_HEIGHT (COSPACE_HEIGHT / SIZE)

#define POSSIBILITY_VALUE_MAX 10.0
#define POSSIBILITY_VALUE_MIN 0.1

#define NOTHING 0
#define WALL 2
#define YELLOW 3
#define DEPOSIT 4

CalculatePosition::CalculatePosition(void)
{
    rep(hi, MAP_HEIGHT)
    {
        rep(wj, MAP_WIDTH)
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

    // debug
    if (getRunMode() >= MODE_DEBUG)
    {
        if (result1 * result2 < 0)
        {
            logMessage("reason 1\n", MODE_DEBUG);
        }
        if (result3 * result4 < 0)
        {
            logMessage("reason 2\n", MODE_DEBUG);
        }
        if (judgeOnLineSegmenet(x1, x2, y1, y2, equation[num][0], equation[num][1]))
        {
            logMessage("reason 3\n", MODE_DEBUG);
        }
        if (judgeOnLineSegmenet(x1, x2, y1, y2, equation[num][1], equation[num][2]))
        {
            logMessage("reason 4\n", MODE_DEBUG);
        }
        if (judgeOnLineSegmenet(equation[num][0], equation[num][1], equation[num][2], equation[num][3], x1, y1))
        {
            logMessage("reason 5\n", MODE_DEBUG);
        }
        if (judgeOnLineSegmenet(equation[num][0], equation[num][1], equation[num][2], equation[num][3], x2, y2))
        {
            logMessage("reason 6\n", MODE_DEBUG);
        }
    }

    return (result1 * result2 < 0 && result3 * result4 < 0) ||
           judgeOnLineSegmenet(x1, x2, y1, y2, equation[num][0], equation[num][1]) ||
           judgeOnLineSegmenet(x1, x2, y1, y2, equation[num][1], equation[num][2]) ||
           judgeOnLineSegmenet(equation[num][0], equation[num][1], equation[num][2], equation[num][3], x1, y1) ||
           judgeOnLineSegmenet(equation[num][0], equation[num][1], equation[num][2], equation[num][3], x2, y2);
}

void CalculatePosition::calculate(int us_left, int us_front, int us_right, int compass)
{
    float move_x, move_y;
    move_x = cos((double)(Compass + 90) / 180 * PI);
    move_y = sin((double)(Compass + 90) / 180 * PI);
    float border = 0.7;
    if (move_x < -border)
    {
        move_x = -1;
    }
    else if (move_x < border)
    {
        move_x = 0;
    }
    else
    {
        move_x = 1;
    }
    if (move_y < -border)
    {
        move_y = -1;
    }
    else if (move_y < border)
    {
        move_y = 0;
    }
    else
    {
        move_y = 1;
    }

    // rotate
    if (WheelLeft * WheelRight < 0)
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

    double angle[3] = {45, 0, -45};
    int distance[3] = {us_left, us_front, us_right};
    // 0 : x, 1 : y
    double coordinate[3][2];

    rep(i, 3)
    {
        angle[i] = (int)(compass + angle[i]) % 360;
        coordinate[i][0] = cos(angle[i] / 180 * PI) * distance[i];
        coordinate[i][1] = sin(angle[i] / 180 * PI) * distance[i];
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
    margin[1] -= margin[1];
    margin[2] -= margin[2];

    rep(i, 4)
    {
        margin[i] /= SIZE;
    }

    // 上
    rep(wj, MAP_WIDTH)
    {
        for (int hi = MAP_HEIGHT - margin[0]; hi < MAP_HEIGHT; hi++)
        {
            current_map_possibility[wj][hi] = 0;
        }
    }

    // 左

    rep(wj, margin[1])
    {
        rep(hi, MAP_HEIGHT)
        {
            current_map_possibility[wj][hi] = 0;
        }
    }

    // 下
    rep(wj, MAP_WIDTH)
    {
        rep(hi, margin[2])
        {
            current_map_possibility[wj][hi] = 0;
        }
    }

    // 右
    for (int wj = MAP_WIDTH - margin[3]; wj < MAP_WIDTH; wj++)
    {
        rep(hi, MAP_HEIGHT)
        {
            current_map_possibility[wj][hi] = 0;
        }
    }

    double difference = SIZE;
    for (int wj = margin[1]; wj < MAP_WIDTH - margin[3]; wj++)
    {
        double x = (wj - 0.5) * SIZE;
        for (int hi = margin[2]; hi < MAP_HEIGHT - margin[0]; hi++)
        {
            double y = (hi - 0.5) * SIZE;
            if (map_data[wj][hi] == WALL)
            {
                current_map_possibility[wj][hi] = 0;
                continue;
            }

            int complete[3] = {0, 0, 0};
            current_map_possibility[wj][hi] = 0;

            rep(i, 3)
            {
                double difference1 = difference * fabs(coordinate[i][0]) / (fabs(coordinate[i][0]) + fabs(coordinate[i][1]));
                double difference2 = difference * fabs(coordinate[i][1]) / (fabs(coordinate[i][0]) + fabs(coordinate[i][1]));
                if (coordinate[i][0] < 0)
                {
                    difference1 = -difference1;
                }
                if (coordinate[i][1] < 0)
                {
                    difference2 = -difference2;
                }
                double x_min = x + coordinate[i][0] - difference1;
                double y_min = y + coordinate[i][1] - difference2;
                double x_big = x + coordinate[i][0] + difference1;
                double y_big = y + coordinate[i][1] + difference2;
                rep(j, equation_num)
                {
                    int result1 = isCross(j, x, y, x_min, y_min);
                    int result2 = isCross(j, x, y, x_big, y_big);
                    if (result1 == 0 && result2 == 1)
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
                current_map_possibility[wj][hi] = 0;
            }
            else if (complete[0] == 2)
            {
                current_map_possibility[wj][hi] = POSSIBILITY_VALUE_MAX / 2;
            }
            else
            {
                current_map_possibility[wj][hi] = POSSIBILITY_VALUE_MAX;
            }
        }
    }

    float k = 0.9;
    rep(wj, MAP_WIDTH)
    {
        rep(hi, MAP_HEIGHT)
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

                    if (hi_range < 0 || hi_range >= MAP_HEIGHT || wj_range < 0 || wj_range >= MAP_WIDTH)
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

        float min = 1000000, max = 0;
        int max_id = -1;
        rep(wj, MAP_WIDTH)
        {
            rep(hi, MAP_HEIGHT)
            {
                if (map_data[wj][hi] == WALL)
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
                    max_id = hi * MAP_WIDTH + wj;
                }
            }
        }

        // magnificationで、値の広さを調整する
        // correctionで、値の位置を調整する
        float magnification = POSSIBILITY_VALUE_MAX / (max - min);
        float correction = POSSIBILITY_VALUE_MIN - min * magnification;
        rep(wj, MAP_WIDTH)
        {
            rep(hi, MAP_HEIGHT)
            {
                if (map_data[wj][hi] == WALL)
                {
                    map_possibility[wj][hi] = 0;
                    continue;
                }
                map_possibility[wj][hi] = map_possibility[wj][hi] * magnification + correction;
            }
        }

        calculated_y = max_id / MAP_WIDTH;
        calculated_x = max_id - calculated_y * MAP_WIDTH;
        // printf("%d calculated %d %d\n", getRepeatedNum(), x * SIZE, y * SIZE);
    }
}

void CalculatePosition::showCurrentMapPossibility()
{
    for (int hi = MAP_HEIGHT - 1; hi >= 0; hi--)
    {
        rep(wj, MAP_WIDTH)
        {
            if (map_data[wj][hi] == WALL)
            {
                cout << "□";
                continue;
            }

            cout << setw(2) << current_map_possibility[wj][hi];
        }
        cout << endl;
    }
    cout << endl;
    cout << endl;
}

void CalculatePosition::showMap2()
{
    showCurrentMapPossibility();
    return;
    for (int hi = MAP_HEIGHT - 1; hi >= 0; hi--)
    {
        rep(wj, MAP_WIDTH)
        {
            if (map_data[wj][hi] == WALL)
            {
                cout << "*";
                continue;
            }

            if (hi == calculated_y && wj == calculated_x)
            {
                cout << "#";
            }
            else if (map_possibility[wj][hi] > 0.8 * POSSIBILITY_VALUE_MAX)
            {
                cout << "+";
            }
            else if (map_possibility[wj][hi] > 0.6 * POSSIBILITY_VALUE_MAX)
            {
                cout << "-";
            }
            else
            {
                cout << " ";
            }

            // printf("%2.0f", map_possibility[hi][wj]);
            // printf("%2d", current_map_possibility[hi][wj]);
            // if (map_possibility[hi][wj] < 0.4)
            // {
            // 	printf("　");
            // }
            // else if (map_possibility[hi][wj] < 0.6)
            // {
            // 	printf("？");
            // }
            // else if (map_possibility[hi][wj] < 0.7)
            // {
            // 	printf("＋");
            // }
            // else
            // {
            // 	printf("☆");
            // }
            // if (map_possibility[hi][wj] < (POSSIBILITY_VALUE_MAX - POSSIBILITY_VALUE_MIN) / 5 * 4 + POSSIBILITY_VALUE_MIN)
            // {
            // 	printf("　");
            // }
            // else if (map_possibility[hi][wj] < (POSSIBILITY_VALUE_MAX - POSSIBILITY_VALUE_MIN) / 4 * 2 + POSSIBILITY_VALUE_MIN)
            // {
            // 	printf("？");
            // }
            // else if (map_possibility[hi][wj] < (POSSIBILITY_VALUE_MAX - POSSIBILITY_VALUE_MIN) / 4 * 3 + POSSIBILITY_VALUE_MIN)
            // {
            // 	printf("＋");
            // }
            // else
            // {
            // 	printf("☆");
            // }
            // switch (current_map_possibility[hi][wj])
            // {
            // case 0:
            // 	printf("　");
            // 	break;
            // case -1:
            // 	printf("□");
            // 	break;
            // case 1:
            // 	printf("？");
            // 	break;
            // case 2:
            // 	printf("＋");
            // 	break;
            // default:
            // 	break;
            // }
        }
        cout << "\n";
    }
    cout << endl;
    // showCurrentMapPossibility();
}
