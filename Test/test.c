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
double map_possibility[MAP_HEIGHT][MAP_WIDTH];

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
				printf("Å@");
				break;
			case WALL:
				printf("Å†");
				break;
			case YELLOW:
				printf("Å[");
				break;
			case DEPOSIT:
				printf("Å{");
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
		// è„
		if (coordinate[i][1] > margin[0])
		{
			margin[0] = coordinate[i][1];
		}
		// ç∂
		if (coordinate[i][0] < margin[1])
		{
			margin[1] = coordinate[i][0];
		}
		// â∫
		if (coordinate[i][1] < margin[2])
		{
			margin[2] = coordinate[i][1];
		}
		// âE
		if (coordinate[i][0] > margin[3])
		{
			margin[3] = coordinate[i][0];
		}
	}

	// +- îΩì]
	margin[1] -= margin[1];
	margin[2] -= margin[2];

	// SIZEçáÇÌÇπ
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

			// ï˚íˆéÆÇçÏÇÈ
		}
	}
}

double equation[100][3];
int equation_num;

void setEquation(int x1, int y1, int x2, int y2)
{
	// (x2 - x1)(y - y1) - (x - x1)(y2 - y1) = 0
	// (x2 - x1)y - (y2 - y1)x + (y2 - y1)x1 - (x2 - x1)y1 = 0
	equation[equation_num][0] = -(y2 - y1);
	equation[equation_num][1] = (x2 - x1);
	equation[equation_num][2] = (y2 - y1) * x1 - (y2 - y1) * y1;
	equation_num++;
}

void calculate2()
{
}

int main()
{
	init();
	showMap();

	calculate(10, 50, 20, 0);
}
