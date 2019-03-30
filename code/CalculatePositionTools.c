#include "CalculatePositionTools.h"
#include "LogErrorTools.h"
#define REP for
#define rep(i, n) REP(int i = 0; i < n; i++)
#define PI 3.14
#define PLUSMINUS(a, b, difference) ((b) < (a) + (difference) && (a) - (difference) < (b))

#define COSPACE_WIDTH 240
#define COSPACE_HEIGHT 180
#define SIZE 2

#define MAP_WIDTH (COSPACE_WIDTH / SIZE + 2)
#define MAP_HEIGHT (COSPACE_HEIGHT / SIZE + 2)

#define NOTHING 0
#define WALL 1
#define YELLOW 2
#define DEPOSIT 3

int map_data[MAP_HEIGHT][MAP_WIDTH];
int map_possibility[MAP_HEIGHT][MAP_WIDTH];
int current_map_possibility[MAP_HEIGHT][MAP_WIDTH];

int equation[100][4];
int equation_num;

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

void setEquation(int x1, int y1, int x2, int y2)
{
	equation[equation_num][0] = x1;
	equation[equation_num][1] = y1;
	equation[equation_num][2] = x2;
	equation[equation_num][3] = y2;
	equation_num++;
}

// í∑ï˚å`Ç≈ÅAï«Çìoò^Ç∑ÇÈ
void setEquations(int x1, int y1, int x2, int y2)
{
	// ç∂ÇÃècñ_
	equation[equation_num][0] = x1;
	equation[equation_num][1] = y1;
	equation[equation_num][2] = x1;
	equation[equation_num][3] = y2;
	equation_num++;

	// â∫ÇÃâ°ñ_
	equation[equation_num][0] = x1;
	equation[equation_num][1] = y1;
	equation[equation_num][2] = x2;
	equation[equation_num][3] = y1;
	equation_num++;

	// âEÇÃècñ_
	equation[equation_num][0] = x2;
	equation[equation_num][1] = y1;
	equation[equation_num][2] = x2;
	equation[equation_num][3] = y2;
	equation_num++;

	// è„ÇÃâ°ñ_
	equation[equation_num][0] = x1;
	equation[equation_num][1] = y2;
	equation[equation_num][2] = x2;
	equation[equation_num][3] = y2;
	equation_num++;
}

// (ax, ay), (bx, by)ÇÃê¸ï™è„Ç…(cx, cy)Ç™Ç†ÇÈÇ©
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
	if (result1 * result2 < 0)
	{
		logMessageOption("reason 1\n", MODE_DEBUG);
	}
	if (result3 * result4 < 0)
	{
		logMessageOption("reason 2\n", MODE_DEBUG);
	}
	if (judgeOnLineSegmenet(x1, x2, y1, y2, equation[num][0], equation[num][1]))
	{
		logMessageOption("reason 3\n", MODE_DEBUG);
	}
	if (judgeOnLineSegmenet(x1, x2, y1, y2, equation[num][1], equation[num][2]))
	{
		logMessageOption("reason 4\n", MODE_DEBUG);
	}
	if (judgeOnLineSegmenet(equation[num][0], equation[num][1], equation[num][2], equation[num][3], x1, y1))
	{
		logMessageOption("reason 5\n", MODE_DEBUG);
	}
	if (judgeOnLineSegmenet(equation[num][0], equation[num][1], equation[num][2], equation[num][3], x2, y2))
	{
		logMessageOption("reason 6\n", MODE_DEBUG);
	}

	return result1 * result2 < 0 && result3 * result4 < 0 ||
				 judgeOnLineSegmenet(x1, x2, y1, y2, equation[num][0], equation[num][1]) ||
				 judgeOnLineSegmenet(x1, x2, y1, y2, equation[num][1], equation[num][2]) ||
				 judgeOnLineSegmenet(equation[num][0], equation[num][1], equation[num][2], equation[num][3], x1, y1) ||
				 judgeOnLineSegmenet(equation[num][0], equation[num][1], equation[num][2], equation[num][3], x2, y2);
}

void calculate2(int us_left, int us_front, int us_right, int compass)
{
	double angle[3] = {45, 0, -45};
	double distance[3] = {us_left, us_front, us_right};
	// 0 : x, 1 : y
	double coordinate[3][2];

	rep(i, 3)
	{
		angle[i] = (int)(compass + angle[i]) % 360;
		coordinate[i][0] = cos(angle[i] / 180 * PI) * distance[i];
		coordinate[i][1] = sin(angle[i] / 180 * PI) * distance[i];
	}

	// 0:è„ 1:ç∂ 2:â∫ 3:âE
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

	// +- îΩì]
	margin[1] -= margin[1];
	margin[2] -= margin[2];

	rep(i, 4)
	{
		margin[i] /= SIZE;
	}

	// è„
	for (int hi = MAP_HEIGHT - margin[0]; hi < MAP_HEIGHT; hi++)
	{
		rep(wj, MAP_WIDTH)
		{
			current_map_possibility[hi][wj] = 0;
		}
	}

	// ç∂
	rep(hi, MAP_HEIGHT)
	{
		rep(wj, margin[1])
		{
			current_map_possibility[hi][wj] = 0;
		}
	}

	// â∫
	rep(hi, margin[2])
	{
		rep(wj, MAP_WIDTH)
		{
			current_map_possibility[hi][wj] = 0;
		}
	}

	// âE
	rep(hi, MAP_HEIGHT)
	{
		for (int wj = MAP_WIDTH - margin[3]; wj < MAP_WIDTH; wj++)
		{
			current_map_possibility[hi][wj] = 0;
		}
	}

	double current_coordinate[2];
	double rate = 0.1;
	double difference = 10;
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
				current_map_possibility[hi][wj] = 1;
			}
			else
			{
				current_map_possibility[hi][wj] = 2;
			}
		}
	}

	rep(hi, MAP_HEIGHT)
	{
		rep(wj, MAP_WIDTH)
		{
			map_possibility[hi][wj] += current_map_possibility[hi][wj];
		}
	}
}

void init2()
{
	setEquations(0, 0, COSPACE_WIDTH, COSPACE_HEIGHT);
	setEquations(60, COSPACE_HEIGHT - 30, 90, COSPACE_HEIGHT);
	setEquations(90, 60, 120, 90);
}

void showMap2()
{
	FILE *fp = fopen("calculated_map.txt", "w");
	for (int hi = MAP_HEIGHT - 1; hi >= 0; hi--)
	{
		rep(wj, MAP_WIDTH)
		{
			fprintf(fp, "%d ", current_map_possibility[hi][wj]);
			// if (map_data[hi][wj] == WALL)
			// {
			// 	fprintf(fp, "Å†");
			// 	continue;
			// }
			// switch (current_map_possibility[hi][wj])
			// {
			// case 0:
			// 	fprintf(fp, "Å@");
			// 	break;
			// case -1:
			// 	fprintf(fp, "Å†");
			// 	break;
			// case 1:
			// 	fprintf(fp, "ÅH");
			// 	break;
			// case 2:
			// 	fprintf(fp, "Å{");
			// 	break;
			// default:
			// 	break;
			// }
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}
