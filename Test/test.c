#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define REP for
#define rep(i, n) REP(int i = 0; i < n; i++)
#define PI 3.14

#define COSPACE_WIDTH 240
#define COSPACE_HEIGHT 180
#define SIZE 10

#define MAP_WIDTH (COSPACE_WIDTH / SIZE + 2)
#define MAP_HEIGHT (COSPACE_HEIGHT / SIZE + 2)

#define NOTHING 0
#define WALL 1
#define YELLOW 2
#define DEPOSIT 3

int map_data[MAP_HEIGHT][MAP_WIDTH];
int map_possibility[MAP_HEIGHT][MAP_WIDTH];

void createStuff(int x1, int y1, int x2, int y2, int kind)
{
	x1 = x1 / SIZE + 1;
	y1 = y1 / SIZE + 1;
	x2 = x2 / SIZE + 1;
	y2 = y2 / SIZE + 1;
	for (int hi = y1; hi < y2; hi++)
	{
		for (int wj = x1; wj < x2; wj++)
		{
			if (map_data[hi][wj] == NOTHING)
			{
				map_data[hi][wj] = kind;
			}
		}
	}
}

void showMap()
{
	for (int hi = MAP_HEIGHT - 1; hi >= 0; hi--)
	{
		rep(wj, MAP_WIDTH)
		{
			switch (map_data[hi][wj])
			{
			case NOTHING:
				printf("�@");
				break;
			case WALL:
				printf("��");
				break;
			case YELLOW:
				printf("�[");
				break;
			case DEPOSIT:
				printf("�{");
				break;
			default:
				break;
			}
		}
		printf("\n");
	}
}

void init(void)
{
	rep(hi, MAP_HEIGHT)
	{
		rep(wj, MAP_WIDTH)
		{
			map_data[hi][wj] = NOTHING;
			map_possibility[hi][wj] = 1;
		}
	}

	rep(hi, MAP_HEIGHT)
	{
		map_data[hi][0] = WALL;
		map_data[hi][MAP_WIDTH - 1] = WALL;
	}
	rep(wi, MAP_WIDTH)
	{
		map_data[0][wi] = WALL;
		map_data[MAP_HEIGHT - 1][wi] = WALL;
	}

	createStuff(60, 150, 90, 180, WALL);
	createStuff(90, 60, 120, 90, WALL);
	createStuff(120, 60, 150, 90, DEPOSIT);
	createStuff(120, 0, 150, 60, YELLOW);
	createStuff(150, 60, 180, 90, YELLOW);
	createStuff(120, 60, 150, 90, DEPOSIT);
}

void calculate(int us_left, int us_front, int us_right, int compass)
{
	double angle[3] = {-45, 0, 45};
	double distance[3] = {us_left, us_front, us_right};
	double coordinate[3][2];

	rep(i, 3)
	{
		angle[i] = (int)(compass + angle[i]) % 360;
		coordinate[i][0] = cos(angle[i] / 180 * PI) * distance[i];
		coordinate[i][1] = sin(angle[i] / 180 * PI) * distance[i];
		// printf("%lf %lf\n", coordinate[i][0], coordinate[i][1]);
	}

	int margin[4] = {0, 0, 0, 0};
	rep(i, 3)
	{
		// ��
		if (coordinate[i][1] > margin[0])
		{
			margin[0] = coordinate[i][1];
		}
		// ��
		if (coordinate[i][0] < margin[1])
		{
			margin[1] = coordinate[i][0];
		}
		// ��
		if (coordinate[i][1] < margin[2])
		{
			margin[2] = coordinate[i][1];
		}
		// �E
		if (coordinate[i][0] > margin[3])
		{
			margin[3] = coordinate[i][0];
		}
	}

	// +- ���]
	margin[1] -= margin[1];
	margin[2] -= margin[2];

	// SIZE���킹
	rep(i, 4)
	{
		margin[i] = (margin[i] + SIZE - 1) / SIZE;
	}

	int current_map_possibility[MAP_HEIGHT][MAP_WIDTH];
	rep(hi, MAP_HEIGHT)
	{
		rep(wj, MAP_WIDTH)
		{
			if (map_data[hi][wj] == WALL || map_data[hi][wj] == YELLOW)
			{
				current_map_possibility[hi][wj] = 0;
				continue;
			}

			if (hi > MAP_HEIGHT - margin[0] || wj <= margin[1] || hi <= margin[2] || wj > MAP_HEIGHT - margin[3])
			{
				current_map_possibility[hi][wj] = 0.5;
				continue;
			}

			// �����������
		}
	}
}

int equation[100][4];
int equation_num;

void setEquation(int x1, int y1, int x2, int y2)
{
	// (x2 - x1)(y - y1) - (x - x1)(y2 - y1) = 0
	// (y2 - y1)x - (x2 - x1)y - (y2 - y1)x1 + (x2 - x1)y1 = 0
	equation[equation_num][0] = x1;
	equation[equation_num][1] = y1;
	equation[equation_num][2] = x2;
	equation[equation_num][3] = y2;
	equation_num++;
}

int isCross(int num, double x1, double y1, double x2, double y2)
{
	double a1, b1, c1, a2, b2, c2;
	a1 = (y2 - y1);
	b1 = -(x2 - x1);
	c1 = (x2 - x1) * y1 - (y2 - y1) * x1;
	a2 = (equation[num][3] - equation[num][1]);
	b2 = -(equation[num][2] - equation[num][0]);
	c2 = -b2 * equation[num][1] - a2 * equation[num][0];
	double result1 = a2 * x1 + b2 * y1 + c2;
	double result2 = a2 * x2 + b2 * y2 + c2;
	// printf("%lf %lf\n", result1, result2);
	if (result1 * result2 > 0)
	{
		return 0;
	}

	result1 = a1 * equation[num][0] + b1 * equation[num][1] + c1;
	result2 = a1 * equation[num][2] + b1 * equation[num][3] + c1;

	if (result1 * result2 > 0)
	{
		return 0;
	}

	return 1;
}

void calculate2(int us_left, int us_front, int us_right, int compass)
{
	double angle[3] = {45, 0, -45};
	double distance[3] = {us_left, us_front, us_right};
	double coordinate[3][2];

	rep(i, 3)
	{
		angle[i] = (int)(compass + angle[i]) % 360;
		coordinate[i][0] = cos(angle[i] / 180 * PI) * distance[i];
		coordinate[i][1] = sin(angle[i] / 180 * PI) * distance[i];
		printf("%lf %lf\n", coordinate[i][0], coordinate[i][1]);
	}

	double current_coordinate[2];
	rep(hi, MAP_HEIGHT)
	{
		// hi = 10;
		printf("start %d\n", hi);
		rep(wj, MAP_WIDTH)
		{
			wj = 22;
			if (map_data[hi][wj] == WALL)
			{
				map_possibility[hi][wj] = -1;
				wj = MAP_WIDTH;
				continue;
			}
			int complete[3] = {0, 0, 0};
			map_possibility[hi][wj] = 0;

			rep(i, 3)
			{
				rep(j, equation_num)
				{
					double min = 0.9;
					double max = 1.1;
					double x = (wj + 0.0) * SIZE;
					double y = (hi + 0.0) * SIZE;
					int result1 = isCross(j, x, y, x + coordinate[i][0] * min, y + coordinate[i][1] * min);
					int result2 = isCross(j, x, y, x + coordinate[i][0] * max, y + coordinate[i][1] * max);
					if (result1 == 0 && result2 == 1 && complete[i] != -1)
					{
						complete[i] = 1;
						// printf("min %lf %lf\n", x + coordinate[i][0] * min, y + coordinate[i][1] * min);
						// printf("max %lf %lf\n", x + coordinate[i][0] * max, y + coordinate[i][1] * max);
						printf("find\n");
					}
					else if (result1 == 1 && result2 == 1)
					{
						complete[i] = -1;
						printf("min %lf %lf\n", x + coordinate[i][0] * min, y + coordinate[i][1] * min);
						printf("max %lf %lf\n", x + coordinate[i][0] * max, y + coordinate[i][1] * max);
						printf("%d game over\n", j);
					}
				}
			}

			complete[0] += complete[1] + complete[2];

			if (complete[0] < 2)
			{
				map_possibility[hi][wj] = 0;
				printf("finally 0\n");
			}
			else
			{
				map_possibility[hi][wj] = 1;
				printf("finally 1\n");
			}

			wj = MAP_WIDTH;
		}
		fflush(stdout);
		// hi = MAP_HEIGHT;
	}
}

void init2()
{
	setEquation(0, 0, COSPACE_WIDTH, 0);
	setEquation(0, 0, 0, COSPACE_HEIGHT);
	setEquation(0, COSPACE_HEIGHT, COSPACE_WIDTH, COSPACE_HEIGHT);
	setEquation(COSPACE_WIDTH, 0, COSPACE_WIDTH, COSPACE_HEIGHT);
	setEquation(60, COSPACE_HEIGHT - 30, 60, COSPACE_HEIGHT);
	setEquation(60, COSPACE_HEIGHT - 30, 90, COSPACE_HEIGHT - 30);
	setEquation(90, COSPACE_HEIGHT - 30, 90, COSPACE_HEIGHT);
	setEquation(90, 60, 90, 90);
	setEquation(90, 60, 120, 60);
	setEquation(120, 60, 120, 90);
	setEquation(90, 90, 90, 120);
}

void showMap2()
{
	for (int hi = MAP_HEIGHT - 1; hi >= 0; hi--)
	{
		rep(wj, MAP_WIDTH)
		{
			switch (map_possibility[hi][wj])
			{
			case 0:
				printf("�@");
				break;
			case -1:
				printf("��");
				break;
			case 1:
				printf("�[");
				break;
			// case DEPOSIT:
			// 	printf("�{");
			// 	break;
			default:
				break;
			}
		}
		printf("\n");
	}
}

int main()
{
	init();
	init2();
	showMap();

	calculate2(57 / 2, 40 / 2, 57 / 2, 0);
	showMap2();
	// int result = isCross(3, 240, 180, 240, 180);
	// printf("%d\n", result);
	// rep(i, 4)
	// {
	// 	printf("%d ", equation[3][i]);
	// }
	// printf("\n");
}
