#include "CalculatePositionTools.hpp"
#include "LogErrorTools.hpp"
#include "CommonTools.hpp"
#include "CospaceSettings.hpp"

using namespace std;

#define REP for
#define rep(i, n) REP(int i = 0; i < n; i++)
#define PLUSMINUS(a, b, difference) ((b) < (a) + (difference) && (a) - (difference) < (b))

#define COSPACE_WIDTH 240
#define COSPACE_HEIGHT 180
#define SIZE 10

#define MAP_WIDTH (COSPACE_WIDTH / SIZE + 2)
#define MAP_HEIGHT (COSPACE_HEIGHT / SIZE + 2)

#define POSSIBILITY_VALUE_MAX 10.0
#define POSSIBILITY_VALUE_MIN 0.1

#define NOTHING 0
#define WALL 1
#define YELLOW 2
#define DEPOSIT 3

int map_data[24][18] = {
    {0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 2, 2, 2},
    {0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 1, 2, 2, 2},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 3, 3, 1, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 3, 3, 3, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 3, 3, 3, 3, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 1, 1, 1, 0, 0},
    {0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},
    {0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0},
    {0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0},
    {0, 2, 2, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2},
    {0, 0, 0, 0, 0, 0, 3, 1, 1, 1, 1, 0, 0, 0, 0, 0, 2, 2},
    {0, 1, 1, 1, 1, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 3, 3, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 3, 3, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 2, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0},
    {2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

float map_possibility[MAP_HEIGHT][MAP_WIDTH];
int current_map_possibility[MAP_HEIGHT][MAP_WIDTH];

int equation[100][4] = {
    {0, 0, 0, 180},
    {0, 0, 240, 0},
    {0, 180, 240, 180},
    {240, 0, 240, 180},
    {0, 10, 30, 10},
    {6, 10, 120, 10},
    {15, 10, 180, 10},
    {0, 20, 30, 20},
    {4, 20, 50, 20},
    {9, 20, 120, 20},
    {4, 30, 50, 30},
    {0, 60, 30, 60},
    {18, 60, 210, 60},
    {0, 70, 30, 70},
    {18, 70, 240, 70},
    {14, 80, 180, 80},
    {21, 80, 240, 80},
    {8, 90, 120, 90},
    {14, 90, 180, 90},
    {19, 90, 210, 90},
    {3, 100, 60, 100},
    {8, 100, 120, 100},
    {13, 100, 150, 100},
    {19, 100, 210, 100},
    {3, 110, 60, 110},
    {13, 110, 150, 110},
    {21, 110, 240, 110},
    {4, 120, 60, 120},
    {21, 120, 240, 120},
    {22, 130, 240, 130},
    {16, 140, 180, 140},
    {22, 140, 240, 140},
    {16, 150, 200, 150},
    {12, 160, 150, 160},
    {18, 160, 200, 160},
    {6, 170, 90, 170},
    {12, 170, 150, 170},
    {10, 10, 10, 20},
    {10, 60, 10, 70},
    {20, 10, 20, 20},
    {20, 60, 20, 70},
    {30, 10, 30, 20},
    {30, 60, 30, 70},
    {30, 100, 30, 110},
    {40, 20, 40, 30},
    {40, 100, 40, 120},
    {50, 20, 50, 30},
    {50, 100, 50, 120},
    {60, 0, 60, 10},
    {60, 100, 60, 120},
    {60, 170, 60, 180},
    {70, 0, 70, 10},
    {70, 170, 70, 180},
    {80, 0, 80, 10},
    {80, 90, 80, 100},
    {80, 170, 80, 180},
    {90, 0, 90, 20},
    {90, 90, 90, 100},
    {90, 170, 90, 180},
    {100, 10, 100, 20},
    {100, 90, 100, 100},
    {110, 10, 110, 20},
    {110, 90, 110, 100},
    {120, 10, 120, 20},
    {120, 90, 120, 100},
    {120, 160, 120, 170},
    {130, 100, 130, 110},
    {130, 160, 130, 170},
    {140, 80, 140, 90},
    {140, 100, 140, 110},
    {140, 160, 140, 170},
    {150, 0, 150, 10},
    {150, 80, 150, 90},
    {150, 100, 150, 110},
    {150, 160, 150, 170},
    {160, 0, 160, 10},
    {160, 80, 160, 90},
    {160, 140, 160, 150},
    {170, 0, 170, 10},
    {170, 80, 170, 90},
    {170, 140, 170, 150},
    {180, 0, 180, 10},
    {180, 60, 180, 70},
    {180, 80, 180, 90},
    {180, 140, 180, 160},
    {190, 60, 190, 70},
    {190, 90, 190, 100},
    {190, 150, 190, 160},
    {200, 60, 200, 70},
    {200, 90, 200, 100},
    {200, 150, 200, 160},
    {210, 60, 210, 80},
    {210, 90, 210, 100},
    {210, 110, 210, 120},
    {220, 70, 220, 80},
    {220, 110, 220, 120},
    {220, 130, 220, 140},
    {230, 70, 230, 80},
    {230, 110, 230, 120},
    {230, 130, 230, 140}};

int equation_num;

int calculated_x, calculated_y;

void init(void)
{
    rep(hi, MAP_HEIGHT)
    {
        rep(wj, MAP_WIDTH)
        {
            map_possibility[hi][wj] = 0;
        }
    }
}

void setEquation(int x1, int y1, int x2, int y2)
{
    equation[equation_num][0] = x1;
    equation[equation_num][1] = y1;
    equation[equation_num][2] = x2;
    equation[equation_num][3] = y2;
    equation_num++;
}

// 長方形で、壁を登録する
void setEquations(int x1, int y1, int x2, int y2)
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
int judgeOnLineSegmenet(double ax, double ay, double bx, double by, double cx, double cy)
{
    double border = 0.00001;
    double l1 = (bx - ax) * (bx - ax) + (by - ay) * (by - ay);
    double l2 = (cx - ax) * (cx - ax) + (cy - ay) * (cy - ay);
    double c = (bx - ax) * (cx - ax) + (by - ay) * (cy - ay);
    return c >= 0 && PLUSMINUS(c * c, l1 * l2, border) && l1 >= l2;
}

int isCross(int num, double x1, double y1, double x2, double y2)
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

void calculate2(int us_left, int us_front, int us_right, int compass)
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
    for (int hi = MAP_HEIGHT - margin[0]; hi < MAP_HEIGHT; hi++)
    {
        rep(wj, MAP_WIDTH)
        {
            current_map_possibility[hi][wj] = 0;
        }
    }

    // 左
    rep(hi, MAP_HEIGHT)
    {
        rep(wj, margin[1])
        {
            current_map_possibility[hi][wj] = 0;
        }
    }

    // 下
    rep(hi, margin[2])
    {
        rep(wj, MAP_WIDTH)
        {
            current_map_possibility[hi][wj] = 0;
        }
    }

    // 右
    rep(hi, MAP_HEIGHT)
    {
        for (int wj = MAP_WIDTH - margin[3]; wj < MAP_WIDTH; wj++)
        {
            current_map_possibility[hi][wj] = 0;
        }
    }

    double difference = SIZE;
    for (int hi = margin[2]; hi < MAP_HEIGHT - margin[0]; hi++)
    {
        double y = (hi - 0.5) * SIZE;
        for (int wj = margin[1]; wj < MAP_WIDTH - margin[3]; wj++)
        {
            if (map_data[hi][wj] == WALL)
            {
                current_map_possibility[hi][wj] = 0;
                continue;
            }

            int complete[3] = {0, 0, 0};
            current_map_possibility[hi][wj] = 0;

            double x = (wj - 0.5) * SIZE;

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
                current_map_possibility[hi][wj] = 0;
            }
            else if (complete[0] == 2)
            {
                current_map_possibility[hi][wj] = POSSIBILITY_VALUE_MAX / 2;
            }
            else
            {
                current_map_possibility[hi][wj] = POSSIBILITY_VALUE_MAX;
            }
        }
    }

    float k = 0.9;
    rep(hi, MAP_HEIGHT)
    {
        rep(wj, MAP_WIDTH)
        {
            map_possibility[hi][wj] *= k;
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
            for (int hi_range = under; hi_range <= top; hi_range++)
            {
                for (int wj_range = left; wj_range <= right; wj_range++)
                {
                    if (hi_range < 0 || hi_range >= MAP_HEIGHT || wj_range < 0 || wj_range >= MAP_WIDTH)
                    {
                        continue;
                    }
                    num++; // 距離の二乗に反比例
                    // addの合計値に反比例
                    // 可能性が同じものが多い場合は、加える可能性を低くする
                    add += (float)current_map_possibility[hi_range][wj_range] * (1.0 - k);
                }
            }
            // add = add / num;
            map_possibility[hi][wj] += add;
            // printf("%f\n", add);
        }

        float min = 1000000, max = 0;
        int max_id = -1;
        rep(hi, MAP_HEIGHT)
        {
            rep(wj, MAP_WIDTH)
            {
                if (map_data[hi][wj] == WALL)
                {
                    continue;
                }
                if (map_possibility[hi][wj] < min)
                {
                    min = map_possibility[hi][wj];
                }
                if (map_possibility[hi][wj] > max)
                {
                    max = map_possibility[hi][wj];
                    max_id = hi * MAP_WIDTH + wj;
                }
            }
        }

        // magnificationで、値の広さを調整する
        // correctionで、値の位置を調整する
        float magnification = POSSIBILITY_VALUE_MAX / (max - min);
        float correction = POSSIBILITY_VALUE_MIN - min * magnification;
        rep(hi, MAP_HEIGHT)
        {
            rep(wj, MAP_WIDTH)
            {
                if (map_data[hi][wj] == WALL)
                {
                    map_possibility[hi][wj] = 0;
                    continue;
                }
                map_possibility[hi][wj] = map_possibility[hi][wj] * magnification + correction;
            }
        }

        calculated_y = max_id / MAP_WIDTH;
        calculated_x = max_id - calculated_y * MAP_WIDTH;
        // printf("%d calculated %d %d\n", getRepeatedNum(), x * SIZE, y * SIZE);
    }
}

void init2()
{
    setEquations(0, 0, COSPACE_WIDTH, COSPACE_HEIGHT);
    setEquations(60, COSPACE_HEIGHT - 30, 90, COSPACE_HEIGHT);
    setEquations(90, 60, 120, 90);
}

void showCurrentMapPossibility()
{
    for (int hi = MAP_HEIGHT - 1; hi >= 0; hi--)
    {
        rep(wj, MAP_WIDTH)
        {
            if (map_data[hi][wj] == WALL)
            {
                cout << "□";
                continue;
            }

            cout << setw(2) << current_map_possibility[hi][wj];
        }
        cout << endl;
    }
    cout << endl;
    cout << endl;
}

void showMap2()
{
    showCurrentMapPossibility();
    return;
    for (int hi = MAP_HEIGHT - 1; hi >= 0; hi--)
    {
        rep(wj, MAP_WIDTH)
        {
            if (map_data[hi][wj] == WALL)
            {
                cout << "*";
                continue;
            }

            if (hi == calculated_y && wj == calculated_x)
            {
                cout << "#";
            }
            else if (map_possibility[hi][wj] > 0.8 * POSSIBILITY_VALUE_MAX)
            {
                cout << "+";
            }
            else if (map_possibility[hi][wj] > 0.6 * POSSIBILITY_VALUE_MAX)
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
