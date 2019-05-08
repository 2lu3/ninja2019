#include "Hikaru.hpp"
#include "MapData.hpp"
#include <iostream>

#define POINT_BLACK 20
#define POINT_RED 10
#define POINT_CYAN 15
#define POINT_SUPERAREA 2
#define POINT_WHITE 1
#define POINT_DEPOSIT 0
#define POINT_SWAMPLAND -1
#define POINT_WALL -2
#define POINT_YELLOW -3

#define RED_LOADED_ID 0
#define CYAN_LOADED_ID 1
#define BLACK_LOADED_ID 2
#define SUPER_LOADED_ID 3

#define COSPACE_WIDTH 360
#define COSPACE_HEIGHT 270
#define SIZE 10
#define DOT_WIDTH_NUMBER (COSPACE_WIDTH / SIZE)
#define DOT_HEIGHT_NUMBER (COSPACE_HEIGHT / SIZE)
#define MAX_DOT_NUMBER (COSPACE_WIDTH * COSPACE_HEIGHT / SIZE / SIZE)
#define MAX_EDGE_NUMBER 25
#define BORDER_SAME_OBJECT_NUMBER 2
#define FIND_OBJECT_DURATION 46

#define FIND_OBJ_DURATION 46

using namespace std;

int log_superobj_num, log_superobj_x[10], log_superobj_y[10];
int absolute_x = -1, absolute_y = -1;
int now_dot_id;
int emergency_now_dot_id = 80;
int super_sameoperate = 0;
int searching_object;

struct Dot
{
	int x, y;  //dotのx(0<=x<36), y(0<=y<27)座標
	int wide;  //一辺の長さ
	int point; //ドットの種類(-3:yellow -2:wall etc.)
	int done;  //Dijkstra()
	long id;   //y * 36 + x
	int from;  //Dijkstra()
	int cost;  //Dijkstra()
	int is_opened;
	int score;
	int distance_from_start;		//Dijkstra()
	int curved_times;				//Dijkstra()
	int arrived_times;				//そこにいた回数
	int edge_num;					//そのドットに行くことのできるドットの数
	int edge_to[MAX_EDGE_NUMBER];   //
	int edge_cost[MAX_EDGE_NUMBER]; //
	int red;						//もし、Redがとれるなら、1
	int cyan;						//もし、Cyanがとれないなら0
	int black;						//もし、Blackが...
	int color;
};
struct Dot dot[MAX_DOT_NUMBER];

void GoToAngle(int angle, int distance);
int GoToPosition(int x, int y, int wide_decide_x, int wide_decide_y, int wide_judge_arrived);
void Dijkstra(void);
int GoToDot(int x, int y);
int CheckNowDot(void);
long WhereIsMotor(void);
long WhereIsColorSensor(void);
void InputDotInformation(void);
int GoToDots(int x, int y, int wide_decide_x, int wide_decide_y);
int GoInDots(int x, int y, int wide_decide_x, int wide_decide_y, int color);
int HowManyCurved(int id);
void AutoStrategy(void);

// int GoToColor(int color);
// int GoToColorArea(int num);

int IsNearYellow(int num, int x, int y);

int log_compass;
int same_target_num = 5;
int same_target = 0;
int log_x = -1, log_y = -1;

int large_process = -1;

const int border_same_obj_number = 3;

void Game0_Hikaru::setup(void)
{
	ProcessingTime pt;
	pt.start();

	UserGame0::setup();

	delErrorFile();
	delLogFile();
	InputColorInformation();

	double seconds = pt.end();
	logMessage("game0 setup() : " + to_string(seconds) + " milliseconds");
}

void Game0_Hikaru::loop(void)
{
	UserGame0::loop();
	logMessage("World1 loop start");
	if (SuperDuration > 0)
	{
		SuperDuration--;
	}
	else if (IsOnDepositArea() && LoadedObjects >= 5)
	{
		logMessage("find object box", MODE_DEBUG);
		switch (IsOnDepositArea())
		{
		case 1:
			logMessage("Left Sensor is in object_box", MODE_DEBUG);
			motor(0, 5);
			break;
		case 2:
			logMessage("Right Sensor is in object_box", MODE_DEBUG);
			motor(5, 0);
			break;
		case 3:
			logMessage("I am in object_box", MODE_DEBUG);
			setAction(DEPOSIT_OBJ);
			SuperDuration = 50;
			break;
		default:
			break;
		}
	}
	else if (EitherColorJudge(black_obj) && loaded_objects[BLACK_LOADED_ID] < border_same_obj_number && LoadedObjects < 6)
	{
		logMessage("find black obj");
		setAction(FIND_OBJ);
		loaded_objects[BLACK_LOADED_ID]++;
		SuperDuration = FIND_OBJ_DURATION;
	}
	else if (EitherColorJudge(cyan_obj) && loaded_objects[CYAN_LOADED_ID] < border_same_obj_number && LoadedObjects < 6)
	{
		logMessage("find cyan obj");
		setAction(FIND_OBJ);
		loaded_objects[CYAN_LOADED_ID]++;
		SuperDuration = FIND_OBJ_DURATION;
	}
	else if (EitherColorJudge(red_obj) && loaded_objects[RED_LOADED_ID] < border_same_obj_number && LoadedObjects < 6)
	{
		logMessage("find red obj");
		setAction(FIND_OBJ);
		loaded_objects[RED_LOADED_ID]++;
		SuperDuration = FIND_OBJ_DURATION;
	}
	else if (Duration > 0)
	{
		Duration--;
	}
	else if (IsOnYellowLine() && LoadedObjects > 0)
	{
		if (IsOnYellowLine() == 1)
		{
			motor(-1, -3);
		}
		else
		{
			motor(-3, -1);
		}
		setAction(YELLOW_AVOIDANCE);
		Duration = 3;
	}
	else if (obstacle(8, 10, 8))
	{
		motor(-2, 2);
		// int speed = 3;
		// switch(obstacle(10, 10, 10)) {
		// 	case 1:// 左のみ
		// 		motor(speed, -speed);
		// 		break;
		// 	case 2:
		// 		if(US_Left < US_Right) {
		// 			motor(speed, -speed);
		// 			break;
		// 		}
		// 		else {
		// 			motor(-speed, speed);
		// 		}
		// 		break;
		// 	case 3:
		// 		motor(speed, -speed);
		// 		break;
		// 	case 4:
		// 		motor(-speed, speed);
		// 		break;
		// 	case 5:
		// 		motor(speed, speed);
		// 		break;
		// 	case 6:
		// 		motor(-speed, speed);
		// 		break;
		// 	case 7:
		// 		motor(-speed, speed);
		// 		break;
		// 	default:
		// 		logMessage("obstacle value is " + obstacle(10, 10, 10), MODE_NORMAL);
		// }
	}
	else if (LoadedObjects >= 5)
	{
		cout << "deposit" << endl;
		if (US_Front < 14)
		{
			motor(-3, 1);
		}
		else if (US_Left < 8)
		{
			motor(-3, -1);
		}
		else if (US_Right < 8)
		{
			motor(-3, -1);
		}
		else if (US_Right < 15)
		{
			motor(3, 4);
		}
		else if (US_Right < 40)
		{
			motor(5, 3);
		}
		else if (US_Right < 80)
		{
			motor(4, 2);
		}
		else
		{
			motor(4, 2);
		}
	}
	else
	{
		// if (US_Front < 14)
		// {
		// 	motor(-3, 1);
		// }
		// else if (US_Left < 8)
		// {
		// 	motor(-3, -1);
		// }
		// else if (US_Right < 8)
		// {
		// 	motor(-3, -1);
		// }
		// else if (US_Right < 15 + rand() % 30)
		// {
		// 	motor(4, 5);
		// }
		// else if (US_Right < 40 + rand() % 30)
		// {
		// 	motor(4, 3);
		// }
		// else if (US_Right < 80)
		// {
		// 	motor(3, 1);
		// }
		// else
		// {
		// 	motor(4, 2);
		// }
		motor(3, 3);
	}

	/*if (Time > 180 && getAction() != FIND_OBJ && getAction() != DEPOSIT_OBJ && (!EitherColorJudge(object_box) && LoadedObjects < 3) || Time > 200)
	{
		logMessage("Teleport");
		setAction(TELEPORT);
	}*/

	if (Time > 180)
	{
		cout << "TELEPOR" << endl;
		setAction(TELEPORT);
	}

	switch (getAction())
	{
	case DEFINED:
		break;
	case YELLOW_AVOIDANCE:
		break;
	case FIND_OBJ:
		if (Duration == FIND_OBJ_DURATION || SuperDuration == FIND_OBJ_DURATION)
		{
			logMessage("Add 1 to LoadedObjects", MODE_VERBOSE);
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
			errorMessage("this robot is not in object_box", MODE_NORMAL);
			LoadedObjects = 6;
			Duration = 0;
			for (int i = 0; i < 3; i++)
			{
				loaded_objects[i] = 2;
			}
		}
		break;
	case TELEPORT:
		Teleport = 2;
		WheelLeft = 0;
		WheelRight = 0;
		LED_1 = 0;
		MyState = 0;
		LoadedObjects = 0;
		SuperDuration = 0;
		resetLoadedObjects();
		CurGame = 1;
		break;
	default:
		errorMessage("action is " + to_string(getAction()));
		break;
	}
	logMessage("World1 Loop End");
}

int Game0_Hikaru::shouldTeleport(void)
{
	return UserGame0::shouldTeleport();
}
void Game0_Hikaru::taskOnTeleport(void)
{
	UserGame0::taskOnTeleport();
	Teleport = 3;
}

FILE *object_file;

void Game1_Hikaru::setup(void)
{
	system("cls");
	UserGame1::setup();
	// fp = fopen("motor.txt", "a");
	InputDotInformation();
	InputColorInformation();
	system("chcp 65001");
	srand((unsigned int)time(NULL));

	log_compass = Compass;
	if (PositionX == 0 && PositionY == 0)
	{
		int y = emergency_now_dot_id / DOT_WIDTH_NUMBER;
		int x = emergency_now_dot_id - DOT_WIDTH_NUMBER * y;
		log_y = y * SIZE;
		log_x = x * SIZE;
		now_dot_id = emergency_now_dot_id;
	}
	else
	{
		log_x = PositionX;
		log_y = PositionY;
	}
}

int process = 0;

void Game1_Hikaru::loop()
{
	ProcessingTime pt;
	pt.start();
	// cout << process << endl;
	UserGame1::loop();
	// printf("serach %d\n", searching_object);
	static int same_time = 0;
	static int prev_repeated_num = 0;
	// printf("%d %d\n", log_x, log_y);
	if (PositionX != 0 || PositionY != 0)
	{
		log_x = PositionX;
		log_y = PositionY;
		log_compass = Compass;

		if (log_x < 0)
		{
			log_x = 0;
		}
		if (log_x >= COSPACE_WIDTH)
		{
			log_x = COSPACE_WIDTH - 1;
		}
		if (log_y < 0)
		{
			log_y = 0;
		}
		if (log_y >= COSPACE_HEIGHT)
		{
			log_y = COSPACE_HEIGHT - 1;
		}
		now_dot_id = CheckNowDot();
		int now_y = now_dot_id / DOT_WIDTH_NUMBER;
		int now_x = now_dot_id - now_y * DOT_WIDTH_NUMBER;
		int range = 3;
		rep(hi, range * 2 + 1)
		{
			rep(wj, range * 2 + 1)
			{
				int y = hi + now_y - range;
				int x = wj + now_x - range;
				if (x < 0 || x >= DOT_WIDTH_NUMBER || y < 0 || y >= DOT_HEIGHT_NUMBER)
				{
					continue;
				}
				dot[y * DOT_WIDTH_NUMBER + x].arrived_times++;
			}
		}
	}
	else
	{
		PositionX = -1;
		for (int wi = 0; wi < 20; wi++)
		{
			for (int hj = 0; hj < 20; hj++)
			{
				int x = log_x / SIZE + wi - 10;
				int y = log_y / SIZE + hj - 10;
				if (x < 0 || x >= DOT_WIDTH_NUMBER || y < 0 || y >= DOT_HEIGHT_NUMBER)
				{
					continue;
				}
				dot[y * DOT_WIDTH_NUMBER + x].arrived_times += 3;
			}
		}
		cout << "log x y" << log_x << " " << log_y << endl;
	}

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
			//*1
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
	else if (IsOnRedObj() && LoadedObjects < 6 && loaded_objects[0] < BORDER_SAME_OBJECT_NUMBER && !(LoadedObjects == 5 && log_superobj_num >= 1))
	{
		setAction(FIND_OBJ);
		loaded_objects[0]++;
		SuperDuration = FIND_OBJECT_DURATION;
	}
	else if (IsOnCyanObj() && LoadedObjects < 6 && loaded_objects[1] < BORDER_SAME_OBJECT_NUMBER && !(LoadedObjects == 5 && log_superobj_num >= 1))
	{
		setAction(FIND_OBJ);
		loaded_objects[1]++;
		SuperDuration = FIND_OBJECT_DURATION;
	}
	else if (IsOnBlackObj() && LoadedObjects < 6 && loaded_objects[2] < BORDER_SAME_OBJECT_NUMBER && !(LoadedObjects == 5 && log_superobj_num >= 1))
	{
		setAction(FIND_OBJ);
		loaded_objects[2]++;
		SuperDuration = FIND_OBJECT_DURATION;
	}
	else if (IsOnSuperObj() && SuperObj_Num == 0 && log_superobj_num > 0)
	{
		same_time = 0;
		setAction(FIND_OBJ);
		SuperDuration = FIND_OBJECT_DURATION;
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
	else if (IsOnDepositArea() && LoadedObjects >= 6)
	{
		process = 0;
		if (IsOnDepositArea() == 3)
		{
			setAction(DEPOSIT_OBJ);
			Duration = FIND_OBJECT_DURATION;
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
	else if (LoadedObjects >= 6)
	{
		searching_object = -1;
		GoInDots(180, 135, 180, 135, POINT_DEPOSIT);
	}
	else if (Time > 450 && LoadedObjects > 2)
	{
		LoadedObjects = 6;
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
				printf("%d There is no superobj\n", getRepeatedNum());
			}
			GoToPosition(log_superobj_x[0] - 5 + rand() % 10, log_superobj_y[0] - 5 + rand() % 10, 1, 1, 1);
			same_time++;
		}
	}
	else
	{
		if (loaded_objects[BLACK_LOADED_ID] < 1 || (loaded_objects[BLACK_LOADED_ID] < 2 && loaded_objects[SUPER_LOADED_ID] == 0))
		{
			// if (process == 0)
			// {
			// 	if (GoInDots(115, 110, 110, 35, POINT_BLACK))
			// 	{
			// 		if (rand() % 5 == 0)
			// 		{
			// 			process++;
			// 		}
			// 	}
			// }
			// else if (process == 1)
			// {
			// 	if (GoInDots(180, 135, 180, 135, POINT_BLACK))
			// 	{
			// 		if (rand() % 10 == 0)
			// 		{
			// 			process++;
			// 		}
			// 	}
			// }
			// else if (process == 2)
			// {
			// 	if (GoInDots(330, 35, 30, 35, POINT_BLACK))
			// 	{
			// 		if (rand() % 5 == 0)
			// 		{
			// 			process++;
			// 		}
			// 	}
			// }
			// else
			// {
			// 	process = 0;
			// }
			GoInDots(180, 135, 180, 135, POINT_BLACK);
			searching_object = BLACK_LOADED_ID;
		}
		else if (loaded_objects[CYAN_LOADED_ID] < 2)
		{

			GoInDots(180, 135, 180, 135, POINT_CYAN);
			/*
			if (process == 0)
			{
				if (GoInDots(115, 110, 110, 35, POINT_CYAN))
				{
					if (rand() % 5 == 0)
					{
						process++;
					}
				}
			}
			else if (process == 1)
			{
				if (GoInDots(180, 135, 180, 135, POINT_CYAN))
				{
					if (rand() % 10 == 0)
					{
						process++;
					}
				}
			}
			else if (process == 2)
			{
				if (GoInDots(330, 155, 30, 35, POINT_CYAN))
				{
					if (rand() % 5 == 0)
					{
						process++;
					}
				}
			}
			else
			{
				process = 0;
			}*/

			searching_object = CYAN_LOADED_ID;
		}
		else
		{
			if (process == 0)
			{
				if (GoInDots(180, 170, 112, 50, POINT_RED))
				{
					if (rand() % 5 == 0)
					{
						process++;
					}
				}
			}
			else if (process == 0)
			{
				if (GoInDots(180, 135, 180, 135, POINT_RED))
				{
					if (rand() % 5)
					{
						process++;
					}
				}
			}
			else
			{
				process = 0;
			}

			/*
			if (process == 0)
			{
				if (GoInDots(135, 170, 135, 70, POINT_RED))
				{
					if (rand() % 5 == 0)
					{
						process++;
					}
				}
			}
			else if (process == 1)
			{
				if (GoInDots(205, 35, 75, 35, POINT_RED))
				{
					if (rand() % 5 == 0)
					{
						process++;
					}
				}
			}
			else
			{
				process = 0;
			}*/
			// searching_object = BLACK_LOADED_ID;
			searching_object = RED_LOADED_ID;
		}
	}

	switch (getAction())
	{
	case DEFINED:
		//defined motor power by motor(int left, int right)
		break;
	case FIND_OBJ:
		if (Duration == FIND_OBJECT_DURATION || SuperDuration == FIND_OBJECT_DURATION)
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
		if (Duration == 0)
		{
			LED_1 = 0;
		}
		else
		{
			if (!IsOnDepositArea())
			{
				LoadedObjects = 6;
				Duration = 0;
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
	}
	double seconds = pt.end();
	cout << "loop time :" + to_string(seconds) + " milliseconds" << endl;
}

long WhereIsMotor(void)
{
	//fprintf(logfile, "%4d Start WhereIsMotor()\n", getRepeatedNum());
	long x, y;
	if (log_x < 0 || log_x >= COSPACE_WIDTH || log_y < 0 || log_y >= COSPACE_HEIGHT)
	{
		//fprintf(errfile, "%4d WhereIsMotor(): log_x, log_y = (%d, %d)\n", getRepeatedNum(), log_x, log_y);
		//fprintf(logfile, "%4d WhereIsMotor(): log_x, log_y = (%d, %d)\n", getRepeatedNum(), log_x, log_y);
		log_x = COSPACE_WIDTH / 2;
		log_y = COSPACE_HEIGHT / 2;
	}
	x = log_x - (long)cos((double)(Compass + 90) * 3.14 / 180) * 5;
	y = log_y - (long)sin((double)(Compass + 90) * 3.14 / 180) * 5;
	//fprintf(logfile, "%4d End WhereIsMotor() with returning %ld * 1000 + %ld = %ld\n", getRepeatedNum(), y, x, y * 1000 + x);
	return y * 1000 + x;
}
long WhereIsColorSensor(void)
{
	//fprintf(logfile, "%4d Start WhereIsColorSensor()\n", getRepeatedNum());
	long x, y;
	if (log_x < 0 || log_x >= COSPACE_WIDTH || log_y < 0 || log_y >= COSPACE_HEIGHT)
	{
		//fprintf(errfile, "%4d WhereIsColorSensor(): log_x, log_y = (%d, %d)\n", getRepeatedNum(), log_x, log_y);
		//fprintf(logfile, "%4d WhereIsColorSensor(): log_x, log_y = (%d, %d)\n", getRepeatedNum(), log_x, log_y);
		log_x = COSPACE_WIDTH / 2;
		log_y = COSPACE_HEIGHT / 2;
	}
	x = log_x + (int)(cos((double)(Compass + 90) * 3.14 / 180) * 5);
	y = log_y + (int)(sin((double)(Compass + 90) * 3.14 / 180) * 5);
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
	if (x >= COSPACE_WIDTH)
	{
		//fprintf(errfile, "%4d WhereIsColorSensor() (x, y) = (%ld, %ld)\n", getRepeatedNum(), x, y);
		x = COSPACE_WIDTH - 1;
	}
	if (y >= COSPACE_HEIGHT)
	{
		//fprintf(errfile, "%4d WhereIsColorSensor() (x, y) = (%ld, %ld)\n", getRepeatedNum(), x, y);
		y = COSPACE_HEIGHT - 1;
	}
	//fprintf(logfile, "%4d End WhereIsColorSensor() with returning %ld * 1000 + %ld = %ld\n", getRepeatedNum(), y, x, y * 1000 + x);
	return y * 1000 + x;
}

int CheckNowDot(void)
{
	//fprintf(logfile, "%4d Start CheckNowDot()\n", getRepeatedNum());
	int x = WhereIsColorSensor();
	int y = x / 1000;
	x = x - y * 1000;
	//fprintf(logfile, "%4d CheckNowDot(): color sensor is (%d, %d)\n", getRepeatedNum(), x, y);
	if (x < 0 || x >= COSPACE_WIDTH || y < 0 || y >= COSPACE_HEIGHT)
	{
		//fprintf(logfile, "%4d CheckNowDot(): (x, y) = (%d, %d)\n", getRepeatedNum(), x, y);
		//fprintf(errfile, "%4d CheckNowDot(): (x, y) = (%d, %d)\n", getRepeatedNum(), x, y);
		if (x < 0)
		{
			x = 10;
		}
		else
		{
			x = COSPACE_WIDTH - 11;
		}
		if (y < 0)
		{
			y = 10;
		}
		else
		{
			y = COSPACE_HEIGHT - 11;
		}
	}

	x = x / SIZE;
	y = y / SIZE;

	//もし、今壁の中の場合
	if (dot[y * DOT_WIDTH_NUMBER + x].point <= POINT_WALL)
	{
		// printf("%d CheckNowDot(): I am in wall or yellow\n", getRepeatedNum());
		//fprintf(logfile, " %d CheckNowDot(): I am in wall or yellow\n", getRepeatedNum());
		for (int i = 3; i <= 5 && dot[y * DOT_WIDTH_NUMBER + x].point <= POINT_WALL; i = i + 2)
		{
			for (int j = 0; j < pow(i, 2); j++)
			{
				int temp_x, temp_y;
				temp_y = j / i;
				temp_x = j - temp_y * i;
				temp_x += x - i / 2;
				temp_y += y - i / 2;
				if (temp_x < 0 || temp_x >= DOT_WIDTH_NUMBER)
				{
					continue;
				}
				if (temp_y < 0 || temp_y >= DOT_HEIGHT_NUMBER)
				{
					continue;
				}
				if (dot[temp_y * DOT_WIDTH_NUMBER + temp_x].point >= POINT_DEPOSIT)
				{
					x = temp_x;
					y = temp_y;
					break;
				}
			}
		}
		if (dot[y * DOT_WIDTH_NUMBER + x].point <= POINT_WALL)
		{
			//fprintf(errfile, "%d CheckNowDot(): I(%d, %d) and around me are in wall or yellow\n", getRepeatedNum(), x, y);
			//fprintf(logfile, " %d CheckNowDot(): I(%d, %d) and around me are in wall or yellow\n", getRepeatedNum(), x, y);
			printf("%d CheckNowDot(): I(%d, %d) and around me are in wall or yellow\n", getRepeatedNum(), x, y);
			//fprintf(logfile, "%4d End CheckNowDot() with returning %d\n", getRepeatedNum(), emergency_now_dot_id);
			return emergency_now_dot_id;
		}
	}
	//fprintf(logfile, "%4d End CheckNowDot() with returning (%d, %d) = %d\n", getRepeatedNum(), x, y, y * DOT_WIDTH_NUMBER + x);
	return y * DOT_WIDTH_NUMBER + x;
}

int IsNearYellow(int num, int x, int y)
{
	//fprintf(logfile, "%4d Start IsNearYellow(%d, %d, %d)\n", getRepeatedNum(), num, x, y);
	if (num % 2 == 0)
	{
		num++;
	}
	if (x < 0)
	{
		y = now_dot_id / DOT_WIDTH_NUMBER;
		x = now_dot_id - y * DOT_WIDTH_NUMBER;
		if (now_dot_id < 0 || now_dot_id >= MAX_DOT_NUMBER)
		{
			//fprintf(errfile, "%4d IsNearYellow(): now_dot_id is %d (%d, %d)\n", getRepeatedNum(), now_dot_id, x, y);
			//fprintf(logfile, "%4d IsNearYellow(): now_dot_id is %d (%d, %d)\n", getRepeatedNum(), now_dot_id, x, y);
			x = DOT_WIDTH_NUMBER / 2;
			y = DOT_HEIGHT_NUMBER / 2;
		}
	}

	for (int i = 0; i < num * num; i++)
	{
		if (i == num * num / 2)
		{
			continue;
		}

		int temp_x, temp_y;
		temp_y = i / num;
		temp_x = i % num;

		temp_x -= num / 2;
		temp_x += x;
		temp_y -= num / 2;
		temp_y += y;

		if (temp_x < 0 || temp_x >= DOT_WIDTH_NUMBER || temp_y < 0 || temp_y >= DOT_HEIGHT_NUMBER)
		{
			continue;
		}

		int id = temp_y * DOT_WIDTH_NUMBER + temp_x;
		if (dot[id].point == POINT_YELLOW)
		{
			//fprintf(logfile, "%4d End IsNearYellow() with returning 1\n", getRepeatedNum());
			return 1;
		}
	}
	//fprintf(logfile, "%4d End IsNearYellow() with returning 0\n", getRepeatedNum());
	return 0;
}

int GoToPosition(int x, int y, int wide_decide_x, int wide_decide_y, int wide_judge_arrived)
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
	if (x < 0 || y < 0 || x > COSPACE_WIDTH || y > COSPACE_HEIGHT || wide_decide_x < 0 || wide_decide_y < 0 || wide_judge_arrived < 0)
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
				printf("warming GoToPosition(): absolute_x, absolute_yが決まりません\n");
				printf("(x, y, wide_x, wide_y, wide_arrive) = (%d, %d, %d, %d, %d)\n", x, y, wide_decide_x, wide_decide_y, wide_judge_arrived);
				break;
			}
			absolute_x = x - wide_decide_x + (rand() + 1) % (wide_decide_x * 2 + 1);
			absolute_y = y - wide_decide_y + (rand() + 1) % (wide_decide_y * 2 + 1);
			i++;
		} while (absolute_x < 10 || absolute_x > COSPACE_WIDTH - 10 || absolute_y < 10 || absolute_y > COSPACE_HEIGHT - 10);
		//same_operate = 0;
	}

	if (absolute_distance <= -1)
	{
		absolute_distance = (int)sqrt(pow(absolute_x - log_x, 2) + pow(absolute_y - log_y, 2)) + 40;
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
			log_x = x;
			log_y = y;
		}
		return 1;
	}

	int temp_x = WhereIsColorSensor();
	int temp_y = temp_x / 1000;
	temp_x -= temp_y * 1000;
	if (PLUSMINUS(absolute_x, temp_x, wide_judge_arrived) && PLUSMINUS(absolute_y, temp_y, wide_judge_arrived))
	{
		printf("(%d, %d)に到着しました\n", absolute_x, absolute_y);
		absolute_x = -1;
		absolute_y = -1;
		same_operate = -1;
		absolute_distance = -1;
		return 1;
	}

	printf("ab(%d, %d)\n", absolute_x, absolute_y);
	x = absolute_x;
	y = absolute_y;
	x = x - temp_x;
	y = y - temp_y;
	printf("x, y = (%d , %d)\n", x, y);
	double angle = atan2(y, x);
	angle = angle * 180 / 3.14;
	printf("%3.3lf ", angle);
	int angle_int = (int)angle;
	angle_int -= 90;
	if (angle_int < 0)
	{
		angle_int += 360;
	}
	printf("angle = %d\n", angle_int);
	GoToAngle(angle_int, sqrt(x * x + y * y));

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

void InputDotInformation(void)
{
	// rep(hi, DOT_HEIGHT_NUMBER)
	// {
	// 	rep(wj, DOT_WIDTH_NUMBER)
	// 	{
	// 		cout << map_output_data[hi][wj];
	// 	}
	// 	cout << endl;
	// }
	// cout << endl;
	int map_position_color_data[DOT_WIDTH_NUMBER][DOT_HEIGHT_NUMBER];
	for (int i = 0; i < DOT_WIDTH_NUMBER; i++)
	{
		for (int j = 0; j < DOT_HEIGHT_NUMBER; j++)
		{
			switch (map_output_data[DOT_HEIGHT_NUMBER - j - 1][i])
			{
			case 0: //white
				map_position_color_data[i][j] = POINT_WHITE;
				break;
			case 1: //yellow
				map_position_color_data[i][j] = POINT_YELLOW;
				break;
			case 2: //wall
				map_position_color_data[i][j] = POINT_WALL;
				break;
			case 3: //swampland
				map_position_color_data[i][j] = POINT_SWAMPLAND;
				break;
			case 4: //deposit
				map_position_color_data[i][j] = POINT_DEPOSIT;
				break;
			case 5: // super area
				map_position_color_data[i][j] = POINT_SUPERAREA;
				break;
			default:
				map_position_color_data[i][j] = POINT_WHITE;
				break;
			}
		}
	}

	for (long i = 0; i < MAX_DOT_NUMBER; i++)
	{
		//I use id of dot
		//id = y * 36(= 360 / SIZE) + x;
		//x and y are 360 / SIZE and 270 / SIZE
		//SIZE may be 10

		int x, y;
		//DOT_WIDTH_NUMBER = 360 / SIZE
		y = i / DOT_WIDTH_NUMBER;
		x = i - y * DOT_WIDTH_NUMBER;

		dot[i].id = i;
		//x position. if x = 0, center of dot is 0 * 10(=SIZE) + 5(=SIZE / 2)
		dot[i].x = x * SIZE + SIZE / 2;
		dot[i].y = y * SIZE + SIZE / 2;
		//the wide of dot
		dot[i].wide = SIZE;
		//printf("(%d, %d, %d, %d)\n", x, y, dot[i].x, dot[i].y);

		//point means what's this dot belongs?
		//For example, this dot belongs yellow.
		//this map_position_color_data is defined at 60 lines upper(may be)
		dot[i].point = map_position_color_data[x][y];
		// dot[i].point = 1;
		dot[i].color = map_position_color_data[x][y];
		dot[i].red = red_data[DOT_HEIGHT_NUMBER - y - 1][x];
		// if (dot[i].red == 0 && dot[i].color == POINT_WHITE)
		// {
		// 	dot[i].cyan = 1;
		// 	dot[i].black = 1;
		// }
		dot[i].cyan = cyan_data[DOT_HEIGHT_NUMBER - y - 1][x];
		dot[i].black = black_data[DOT_HEIGHT_NUMBER - y - 1][x];

		//these are for dijkstra
		// dot[i].done = -1;
		// dot[i].from = -1;
		// dot[i].cost = -1;
	}

	//set values of cost
	for (long i = 0; i < MAX_DOT_NUMBER; i++)
	{
		int y = i / DOT_WIDTH_NUMBER;
		int x = i - y * DOT_WIDTH_NUMBER;
		dot[i].edge_num = 0;
		for (int j = 0; j < 9; j++)
		{
			// if (j % 2 == 0) {
			// 	continue;
			// }
			if (j == 4)
			{
				continue;
			}
			int temp_x, temp_y;
			temp_y = j / 3;
			temp_x = j - temp_y * 3;
			temp_x += x - 1;
			temp_y += y - 1;
			int target_id = temp_y * DOT_WIDTH_NUMBER + temp_x;
			if (temp_x < 0 || temp_x >= DOT_WIDTH_NUMBER || temp_y < 0 || temp_y >= DOT_HEIGHT_NUMBER)
			{
				continue;
			}
			dot[i].edge_to[dot[i].edge_num] = target_id;
			//yellow or wall
			if (dot[i].point <= POINT_WALL || dot[target_id].point <= POINT_WALL)
			{
				// dot[i].edge_cost[dot[i].edge_num] = -1;
				dot[i].edge_num--;
			}
			//swampland
			else if (dot[i].point == POINT_SWAMPLAND || dot[target_id].point == POINT_SWAMPLAND)
			{
				dot[i].edge_cost[dot[i].edge_num] = (dot[i].wide + dot[target_id].wide) * 1000 * 1000;
			}
			//マップの端
			else if (x == 0 || y == 0 || x == DOT_WIDTH_NUMBER - 1 || y == DOT_HEIGHT_NUMBER - 1 || temp_x == 0 || temp_y == 0 || temp_x == DOT_WIDTH_NUMBER - 1 || temp_y == DOT_HEIGHT_NUMBER - 1)
			{
				dot[i].edge_cost[dot[i].edge_num] = (dot[i].wide + dot[target_id].wide) * 1000 * 1000;
			}
			else
			{
				//others
				dot[i].edge_cost[dot[i].edge_num] = (dot[i].wide + dot[target_id].wide) / 2;
			}
			if (j % 2 == 0 && dot[i].edge_num >= 0 && dot[i].point != POINT_SWAMPLAND && dot[target_id].point != POINT_SWAMPLAND)
			{
				dot[i].edge_cost[dot[i].edge_num] = dot[i].edge_cost[dot[i].edge_num] * 1.4;
			}
			dot[i].edge_num++;
		}
	}
	// for (int hi = DOT_HEIGHT_NUMBER - 1; hi >= 0; hi--)
	// {
	// 	rep(wj, DOT_WIDTH_NUMBER)
	// 	{
	// 		switch (dot[hi * DOT_WIDTH_NUMBER + wj].point)
	// 		{
	// 		case POINT_YELLOW:
	// 			cout << "$";
	// 			break;
	// 		case POINT_WALL:
	// 			cout << "#";
	// 			break;
	// 		case POINT_DEPOSIT:
	// 			cout << "@";
	// 			break;
	// 		case POINT_SUPERAREA:
	// 			cout << "^";
	// 			break;
	// 		case POINT_SWAMPLAND:
	// 			cout << "|";
	// 			break;
	// 		default:
	// 			if (dot[hi * DOT_WIDTH_NUMBER + wj].black == 1)
	// 			{
	// 				cout << "B";
	// 			}
	// 			else if (dot[hi * DOT_WIDTH_NUMBER + wj].cyan == 1)
	// 			{
	// 				cout << "C";
	// 			}
	// 			else if (dot[hi * DOT_WIDTH_NUMBER + wj].red == 1)
	// 			{
	// 				cout << "R";
	// 			}
	// 			else
	// 			{
	// 				cout << " ";
	// 			}
	// 			break;
	// 		}
	// 	}
	// 	cout << endl;
	// }
	// cout << endl;
}

void Dijkstra()
{
	//fprintf(logfile, " %d Start Dijkstra()\n", getRepeatedNum());
	for (int i = 0; i < MAX_DOT_NUMBER; i++)
	{
		dot[i].cost = -1;
		// dot[i].distance_from_start = -1;
		dot[i].done = -1;
		dot[i].from = -1;
	}

	int now_node_id = now_dot_id;

	if (now_node_id < 0 || now_node_id >= MAX_DOT_NUMBER)
	{
		printf("\n\n\nDijkstra2(): now_dot_id's value is %d\n\n\n", now_node_id);
		return;
	}
	//printf("Dijsktra(): now_node_id = %d\n", now_node_id);
	dot[now_node_id].cost = 0;
	dot[now_node_id].from = now_node_id;
	//printf("nownode by dijkstra is %d and point is %d\n", temp, dot[temp].point);

	struct Dot investigating_node;

	// int number = 0;
	while (true)
	{
		// number++;
		//investigating_nodeを初期化
		investigating_node.done = 0;
		for (int i = 0; i < MAX_DOT_NUMBER; i++)
		{
			//if done is 0, it means already
			if (dot[i].done == 0 || dot[i].cost < 0)
			{
				continue;
			}

			// if (dot[i].point < -1 || (dot[i].point < 0 && option == 0)) {
			// 	continue;
			// }

			//If the dot is yellow or wall
			if (dot[i].point < POINT_SWAMPLAND)
			{
				continue;
			}

			//未代入の場合
			if (investigating_node.done == 0)
			{
				investigating_node = dot[i];
				continue;
			}

			//新しいドットのコストが小さい場合
			if (dot[i].cost < investigating_node.cost)
			{
				investigating_node = dot[i];
			}
		}

		//新しいノードの場合
		if (investigating_node.done == 0)
		{
			break;
		}

		dot[investigating_node.id].done = 0;
		// printf("%dからやるよ\n", investigating_node.id);

		for (int i = 0; i < investigating_node.edge_num; i++)
		{
			int target_id = investigating_node.edge_to[i];
			if (target_id < 0 || target_id >= MAX_DOT_NUMBER)
			{
				//fprintf(errfile, " %d DIjkstra() a edge have error dot[%d].edge_to[%d] = %d\n", getRepeatedNum(), investigating_node.id, i, target_id);
				//fprintf(logfile, " %d DIjkstra() a edge have error dot[%d].edge_to[%d] = %d\n", getRepeatedNum(), investigating_node.id, i, target_id);
				continue;
			}

			if (dot[target_id].done == 0)
			{
				continue;
			}

			//If target_dot is yellow or wall
			if (dot[target_id].point < POINT_SWAMPLAND)
			{
				continue;
			}

			int remember_from = dot[target_id].from;
			dot[target_id].from = investigating_node.id;
			int target_curved_times = HowManyCurved(target_id);
			int target_cost = investigating_node.cost + investigating_node.edge_cost[i];
			target_cost += target_curved_times * 10;
			if (LoadedObjects == 6)
			{
			}
			else
			{
				target_cost += dot[target_id].arrived_times * 10;
			}

			// if (dot[target_id].point < -1) {
			// 	// if (option == 0) {
			// 		continue;
			// 	// }
			// }
			double k = 0.3;
			if (searching_object == BLACK_LOADED_ID && dot[investigating_node.id].black == 1)
			{
				target_cost *= k;
			}
			if (searching_object == CYAN_LOADED_ID && dot[investigating_node.id].cyan == 1)
			{
				target_cost *= k;
			}
			if (searching_object == RED_LOADED_ID && dot[investigating_node.id].red == 1)
			{
				target_cost *= k;
			}

			if (target_cost <= 0)
			{
				printf("fixed\n");
				target_cost = 1;
			}

			if (dot[target_id].cost < 0 || target_cost < dot[target_id].cost)
			{
				dot[target_id].cost = target_cost;
				// dot[target_id].distance_from_start = investigating_node.distance_from_start + investigating_node.edge_cost[i];
				dot[target_id].from = investigating_node.id;
			}
			else
			{
				dot[target_id].from = remember_from;
			}
		}
	}

	/*
	for (int j = DOT_HEIGHT_NUMBER - 1; j >= 0; j--)
	{
		for (int i = 0; i < DOT_WIDTH_NUMBER; i++)
		{
			int id = j * DOT_WIDTH_NUMBER + i;
			int prev_y = dot[id].from / DOT_WIDTH_NUMBER;
			int prev_x = dot[id].from - prev_y * DOT_WIDTH_NUMBER;
			if (dot[id].point < POINT_SWAMPLAND)
			{
				printf("＃");
			}
			else if (dot[i].from == -1)
			{
				printf("＊");
			}
			else if (j - prev_y == 1)
			{
				if (i - prev_x == 1)
				{
					printf("↗");
				}
				else if (i - prev_x == 0)
				{
					printf("↑");
				}
				else if (i - prev_x == -1)
				{
					printf("↖");
				}
				else
				{
					printf("%2d", i - prev_x);
				}
			}
			else if (j - prev_y == 0)
			{
				if (i - prev_x == 1)
				{
					printf("->");
				}
				else if (i - prev_x == 0)
				{
					printf("@ ");
				}
				else if (i - prev_x == -1)
				{
					printf("<-");
				}
				else
				{
					printf("%2d", i - prev_x);
				}
			}
			else if (j - prev_y == -1)
			{
				if (i - prev_x == 1)
				{
					printf("↘");
				}
				else if (i - prev_x == 0)
				{
					printf("↓");
				}
				else if (i - prev_x == -1)
				{
					printf("↙");
				}
				else
				{
					printf("%2d", i - prev_x);
				}
			}
			else
			{
				printf("%2d", j - prev_y);
			}
		}
		printf("\n");
	}*/
}

int GoToDot(int x, int y)
{
	// printf("%d %d\n", x * SIZE, y * SIZE);
	static int prev_x = -1, prev_y = -1, prev_now_dot_id = -1;

	//fprintf(logfile, " %d Start GoToDot(%d, %d)\n", getRepeatedNum(), x, y);
	if (PositionX == -1 || (PLUSMINUS(log_x, x * SIZE, SIZE) && PLUSMINUS(log_y, y * SIZE, SIZE)))
	{
		//fprintf(logfile, " %d End GoToDot() with returning 1 because I am in PLA and it's near target(%d, %d)\n", getRepeatedNum(), x, y);
		return 1;
	}
	// char map_data_to_show[MAX_DOT_NUMBER];
	// for (int i = 0; i < MAX_DOT_NUMBER; i++)
	// {
	// 	if (dot[i].point <= POINT_WALL)
	// 	{
	// 		map_data_to_show[i] = '*';
	// 	}
	// 	else
	// 	{
	// 		map_data_to_show[i] = ' ';
	// 	}
	// }

	//If the node I want to go will be go out
	if (x < 1 || x >= DOT_WIDTH_NUMBER - 1 || y < 1 || y >= DOT_HEIGHT_NUMBER - 1)
	{
		printf("GoToDot(): (x, y) is (%d, %d) and strange\n", x, y);
		//fprintf(errfile, "%d GoToDot(): (x, y) is (%d, %d) and strange\n", getRepeatedNum(), x, y);
		//fprintf(logfile, " %d GoToDot(): (x, y) is (%d, %d) and strange\n", getRepeatedNum(), x, y);
	}

	if (prev_now_dot_id != now_dot_id || prev_x != x || prev_y != y)
	{
		Dijkstra();
	}
	prev_now_dot_id = now_dot_id;
	prev_x = x;
	prev_y = y;

	// printf("from %d %d to %d %d\n", now_dot_id - (int)(now_dot_id / DOT_WIDTH_NUMBER) * DOT_WIDTH_NUMBER, now_dot_id / DOT_WIDTH_NUMBER, x, y);

	int goal_dot = y * DOT_WIDTH_NUMBER + x;

	if (goal_dot < 0 || goal_dot >= MAX_DOT_NUMBER)
	{
		printf("strange (x,y)\n");
		return 0;
	}

	int temp = goal_dot;
	// map_data_to_show[goal_dot] = 'T';
	int i = 0;

	while (dot[temp].from != now_dot_id && i < 200)
	{
		// int go_x, go_y;
		// go_y = temp / DOT_WIDTH_NUMBER;
		// go_x = temp - (int)go_y * DOT_WIDTH_NUMBER;
		temp = dot[temp].from;
		// map_data_to_show[temp] = '#';
		// printf("%d\n", dot[temp].point);
		i++;
		if (temp < 0 || temp >= MAX_DOT_NUMBER)
		{
			printf("temp = %d is strange. I will continue\n", temp);
			GoToPosition(x * SIZE, y * SIZE, 5, 5, 5);
			return 0;
		}
	}
	if (i == 200)
	{
		printf("\n\n\niの値が200ですByGoToNode()\n\n\n\n");
		;
	}

	// map_data_to_show[now_dot_id] = '@';

	int next_x, next_y;
	next_y = temp / DOT_WIDTH_NUMBER;
	next_x = temp - next_y * DOT_WIDTH_NUMBER;

	int now_y = now_dot_id / DOT_WIDTH_NUMBER;
	int now_x = now_dot_id - now_y * DOT_WIDTH_NUMBER;

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
			GoToPosition(log_x - 3 + rand() % 6, log_y - 3 + rand() % 6, 6, 6, 3);
			return 1;
		}
		else
		{
			GoToAngle(0, distance);
		}
	}
	else
	{
		if (next_y <
			now_y)
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
	// system("cls");

	// for (int i = 0; i < DOT_WIDTH_NUMBER + 2; i++)
	// {
	// 	printf("|");
	// }
	// printf("\n");
	// for (int i = DOT_HEIGHT_NUMBER - 1; i >= 0; i--)
	// {
	// 	printf("|");
	// 	for (int j = 0; j < DOT_WIDTH_NUMBER; j++)
	// 	{
	// 		int id = i * DOT_WIDTH_NUMBER + j;
	// 		printf("%c", map_data_to_show[id]);
	// 	}
	// 	printf("|");
	// 	printf("\n");
	// }
	// for (int i = 0; i < DOT_WIDTH_NUMBER + 2; i++)
	// {
	// 	printf("|");
	// }
	// printf("\n");
	//fprintf(logfile, " %d End GoToDot()\n", getRepeatedNum());
	return 0;
}

int GoToDots(int x, int y, int wide_decide_x, int wide_decide_y)
{
	//fprintf(logfile, " %d Start GoToDots(%d, %d, %d, %d)\n", getRepeatedNum(), x, y, wide_decide_x, wide_decide_y);
	// printf("GoToDots(): %d %d %d %d\n", x, y, wide_decide_x, wide_decide_y);
	static int prev_x = -1;
	static int prev_y = -1;
	static int target_x = -1;
	static int target_y = -1;
	static int same_target = 0;
	static int same_target_border = 0;
	if (x != prev_x || y != prev_y)
	{
		printf("changed dots\n");
		same_target = 0;
		prev_x = x;
		prev_y = y;
		//0:left bottom corner 1:right bottom corner 2:right bottom corner
		int corner_x[2], corner_y[2];
		corner_x[0] = (x - wide_decide_x) / SIZE;
		corner_y[0] = (y - wide_decide_y) / SIZE;
		corner_x[1] = (x + wide_decide_x) / SIZE;
		corner_y[1] = (y + wide_decide_y) / SIZE;

		for (int i = 0; i < 2; i++)
		{
			if (corner_x[i] < 0)
			{
				//fprintf(errfile, " %d GoToDots() corner_x[%d] is %d < 0\n", getRepeatedNum(), i, corner_x[i]);
				//fprintf(logfile, " %d GoToDots() corner_x[%d] is %d < 0\n", getRepeatedNum(), i, corner_x[i]);
				corner_x[i] = 0;
			}
			if (corner_x[i] >= DOT_WIDTH_NUMBER)
			{
				//fprintf(errfile, " %d GoToDots() corner_x[%d] is %d >= %d\n", getRepeatedNum(), i, corner_x[i], DOT_WIDTH_NUMBER);
				//fprintf(logfile, " %d GoToDots() corner_x[%d] is %d >= %d\n", getRepeatedNum(), i, corner_x[i], DOT_WIDTH_NUMBER);
				corner_x[i] = DOT_WIDTH_NUMBER - 1;
			}
			if (corner_y[i] < 0)
			{
				//fprintf(errfile, " %d GoToDots() corner_y[%d] is %d < 0\n", getRepeatedNum(), i, corner_y[i]);
				//fprintf(logfile, " %d GoToDots() corner_y[%d] is %d < 0\n", getRepeatedNum(), i, corner_y[i]);
				corner_y[i] = 0;
			}
			if (corner_y[i] >= DOT_HEIGHT_NUMBER)
			{
				//fprintf(errfile, " %d GoToDots() corner_y[%d] is %d >= %d\n", getRepeatedNum(), i, corner_y[i], DOT_HEIGHT_NUMBER);
				//fprintf(logfile, " %d GoToDots() corner_y[%d] is %d >= %d\n", getRepeatedNum(), i, corner_y[i], DOT_HEIGHT_NUMBER);
				corner_y[i] = DOT_HEIGHT_NUMBER - 1;
			}
		}

		int min = 100000, id = -1;
		for (int i = corner_x[0]; i <= corner_x[1]; i++)
		{
			for (int j = corner_y[0]; j <= corner_y[1]; j++)
			{
				int investigating_dot_id = j * DOT_WIDTH_NUMBER + i;
				if (i <= 0 || i >= DOT_WIDTH_NUMBER - 1 || j <= 0 || j >= DOT_HEIGHT_NUMBER - 1)
				{
					continue;
				}

				//yellow or wall or deposit
				if (dot[investigating_dot_id].point < POINT_DEPOSIT)
				{
					continue;
				}

				int costs = dot[investigating_dot_id].arrived_times * 100 + rand() % 10 - pow(i * SIZE - log_x, 2) / 100 - pow(j * SIZE - log_y, 2) / 100;
				// for (int i = 0; i < 100000; i++) {
				// 	// for (int j = 0; j < 1000000; j++) {
				// 		// for (int k = 0; k < 100000; k++) {
				// 		// }
				// 	// }
				// }
				if (costs < min)
				{
					min = costs;
					id = investigating_dot_id;
				}
			}
		}
		if (id == -1)
		{
			//fprintf(stdout, "%d GoToDots(): There is no dot that can go target(%d %d) log(%d %d) eme %d\n", getRepeatedNum(), target_x, target_y, log_x, log_y, now_dot_id);
			// //fprintf(logfile, " %d GoToDots(): There is no dot that can go log(%d %d) eme %d\n", getRepeatedNum());
			// //fprintf(stdout, "%d GoToDots(): There is no dot that can go log(%d %d) eme %d\n", getRepeatedNum());
			target_x = x / SIZE;
			target_y = y / SIZE;
		}
		else
		{
			target_y = id / DOT_WIDTH_NUMBER;
			target_x = id - target_y * DOT_WIDTH_NUMBER;
			//fprintf(logfile, " %d decide target as (%d, %d)\n", getRepeatedNum(), target_x, target_y);
		}

		same_target_border = sqrt(pow(log_x - target_x * SIZE, 2) + pow(log_y - target_y * SIZE, 2));
		same_target_border *= 2;
		same_target_border += 30;

		// int i = 0;
		// do {
		// 	i++;
		// 	if(i > 20) {
		// 		printf("%d GoToDots(): can not decide target\n", getRepeatedNum());
		// 		//fprintf(errfile, "%d GoToDots(): Can not decide target\n", getRepeatedNum());
		// 		//fprintf(logfile, " %d GoToDots(): Can not decide target\n", getRepeatedNum());
		// 		target_x = x;
		// 		target_y = y;
		// 		target_x /= SIZE;
		// 		target_y /= SIZE;
		// 		break;
		// 	}
		// 	target_x = x - wide_decide_x + rand() % (wide_decide_x * 2 + 1);
		// 	target_y = y - wide_decide_y + rand() % (wide_decide_y * 2 + 1);
		// 	target_x /= SIZE;
		// 	target_y /= SIZE;
		// 	if(target_x <= 0) {
		// 		target_x = 1;
		// 	}
		// 	if(target_x >= DOT_WIDTH_NUMBER - 1) {
		// 		target_x = DOT_WIDTH_NUMBER - 2;
		// 	}
		// 	if(target_y <= 0) {
		// 		target_y = 1;
		// 	}
		// 	if(target_y >= DOT_HEIGHT_NUMBER - 1) {
		// 		target_y = DOT_HEIGHT_NUMBER - 2;
		// 	}
		// } while(dot[target_y * DOT_WIDTH_NUMBER + target_x].point <= POINT_WALL);
	}
	same_target++;
	// printf("%d %d\n", same_target, same_target_border);
	if (GoToDot(target_x, target_y) || same_target > same_target_border)
	{
		prev_x = -1;
		same_target = 0;
		//fprintf(logfile, " %d End GoToDots() with returning 1\n", getRepeatedNum());
		return 1;
	}
	else
	{
		//fprintf(logfile, " %d End GoToDots() with returning 0\n", getRepeatedNum());
		return 0;
	}
}

int GoInDots(int x, int y, int wide_decide_x, int wide_decide_y, int color)
{
	//fprintf(logfile, " %d Start GoToDots(%d, %d, %d, %d)\n", getRepeatedNum(), x, y, wide_decide_x, wide_decide_y);
	// printf("GoToDots(): %d %d %d %d\n", x, y, wide_decide_x, wide_decide_y);
	static int prev_x = -1;
	static int prev_y = -1;
	static int prev_color = -1000;
	static int target_x = -1;
	static int target_y = -1;
	static int same_target = 0;
	static int same_target_border = 0;
	if (x != prev_x || y != prev_y || color != prev_color)
	{
		printf("changed dots\n");
		same_target = 0;
		prev_x = x;
		prev_y = y;
		//0:left bottom corner 1:right bottom corner 2:right bottom corner
		int corner_x[2], corner_y[2];
		corner_x[0] = (x - wide_decide_x) / SIZE;
		corner_y[0] = (y - wide_decide_y) / SIZE;
		corner_x[1] = (x + wide_decide_x) / SIZE;
		corner_y[1] = (y + wide_decide_y) / SIZE;

		for (int i = 0; i < 2; i++)
		{
			if (corner_x[i] < 0)
			{
				//fprintf(errfile, " %d GoToDots() corner_x[%d] is %d < 0\n", getRepeatedNum(), i, corner_x[i]);
				//fprintf(logfile, " %d GoToDots() corner_x[%d] is %d < 0\n", getRepeatedNum(), i, corner_x[i]);
				corner_x[i] = 0;
			}
			if (corner_x[i] >= DOT_WIDTH_NUMBER)
			{
				//fprintf(errfile, " %d GoToDots() corner_x[%d] is %d >= %d\n", getRepeatedNum(), i, corner_x[i], DOT_WIDTH_NUMBER);
				//fprintf(logfile, " %d GoToDots() corner_x[%d] is %d >= %d\n", getRepeatedNum(), i, corner_x[i], DOT_WIDTH_NUMBER);
				corner_x[i] = DOT_WIDTH_NUMBER - 1;
			}
			if (corner_y[i] < 0)
			{
				//fprintf(errfile, " %d GoToDots() corner_y[%d] is %d < 0\n", getRepeatedNum(), i, corner_y[i]);
				//fprintf(logfile, " %d GoToDots() corner_y[%d] is %d < 0\n", getRepeatedNum(), i, corner_y[i]);
				corner_y[i] = 0;
			}
			if (corner_y[i] >= DOT_HEIGHT_NUMBER)
			{
				//fprintf(errfile, " %d GoToDots() corner_y[%d] is %d >= %d\n", getRepeatedNum(), i, corner_y[i], DOT_HEIGHT_NUMBER);
				//fprintf(logfile, " %d GoToDots() corner_y[%d] is %d >= %d\n", getRepeatedNum(), i, corner_y[i], DOT_HEIGHT_NUMBER);
				corner_y[i] = DOT_HEIGHT_NUMBER - 1;
			}
		}

		int min = 100000, id = -1;
		for (int i = corner_x[0]; i <= corner_x[1]; i++)
		{
			for (int j = corner_y[0]; j <= corner_y[1]; j++)
			{
				int investigating_dot_id = j * DOT_WIDTH_NUMBER + i;
				if (i <= 0 || i >= DOT_WIDTH_NUMBER - 1 || j <= 0 || j >= DOT_HEIGHT_NUMBER - 1)
				{
					continue;
				}

				//yellow or wall or deposit
				if (dot[investigating_dot_id].point < POINT_DEPOSIT)
				{
					continue;
				}

				if (color == POINT_RED)
				{
					if (dot[investigating_dot_id].red != 1)
					{
						continue;
					}
				}
				else if (color == POINT_CYAN)
				{
					if (dot[investigating_dot_id].cyan != 1)
					{
						continue;
					}
				}
				else if (color == POINT_BLACK)
				{
					if (dot[investigating_dot_id].black != 1)
					{
						continue;
					}
				}
				else if (color == POINT_DEPOSIT)
				{
					if (dot[investigating_dot_id].point != POINT_DEPOSIT)
					{
						continue;
					}
				}

				int costs = dot[investigating_dot_id].arrived_times * 100 - pow(i * SIZE - log_x, 2) / 100 - pow(j * SIZE - log_y, 2) / 100; // + abs(rnd() % 100);
				if (color == POINT_DEPOSIT)
				{
					costs = 1000 - -pow(i * SIZE - log_x, 2) - pow(j * SIZE - log_y, 2);
				}
				// cout << "position cost " << pow(i * SIZE - log_x, 2) / 100 + pow(j * SIZE - log_y, 2) / 100 << " arrived cost " << dot[investigating_dot_id].arrived_times * 100 << endl;
				// for (int i = 0; i < 100000; i++) {
				// 	// for (int j = 0; j < 1000000; j++) {
				// 		// for (int k = 0; k < 100000; k++) {
				// 		// }
				// 	// }
				// }
				if (costs < min)
				{
					min = costs;
					id = investigating_dot_id;
				}
			}
		}
		cout << "cost " << min << endl;
		if (id == -1)
		{
			//fprintf(errfile, "%d GoInDots(): There is no dot that can go\n", getRepeatedNum());
			//fprintf(logfile, " %d GoToDots(): There is no dot that can go\n", getRepeatedNum());
			//fprintf(stdout, "%d GoInDots(): There is no dot that can go\n", getRepeatedNum());
			target_x = x / SIZE;
			target_y = y / SIZE;
		}
		else
		{
			target_y = id / DOT_WIDTH_NUMBER;
			target_x = id - target_y * DOT_WIDTH_NUMBER;
			//fprintf(logfile, " %d decide target as (%d, %d)\n", getRepeatedNum(), target_x, target_y);
		}

		same_target_border = sqrt(pow(log_x - target_x * SIZE, 2) + pow(log_y - target_y * SIZE, 2));
		same_target_border *= 2;
		same_target_border += 30;

		// int i = 0;
		// do {
		// 	i++;
		// 	if(i > 20) {
		// 		printf("%d GoToDots(): can not decide target\n", getRepeatedNum());
		// 		//fprintf(errfile, "%d GoToDots(): Can not decide target\n", getRepeatedNum());
		// 		//fprintf(logfile, " %d GoToDots(): Can not decide target\n", getRepeatedNum());
		// 		target_x = x;
		// 		target_y = y;
		// 		target_x /= SIZE;
		// 		target_y /= SIZE;
		// 		break;
		// 	}
		// 	target_x = x - wide_decide_x + rand() % (wide_decide_x * 2 + 1);
		// 	target_y = y - wide_decide_y + rand() % (wide_decide_y * 2 + 1);
		// 	target_x /= SIZE;
		// 	target_y /= SIZE;
		// 	if(target_x <= 0) {
		// 		target_x = 1;
		// 	}
		// 	if(target_x >= DOT_WIDTH_NUMBER - 1) {
		// 		target_x = DOT_WIDTH_NUMBER - 2;
		// 	}
		// 	if(target_y <= 0) {
		// 		target_y = 1;
		// 	}
		// 	if(target_y >= DOT_HEIGHT_NUMBER - 1) {
		// 		target_y = DOT_HEIGHT_NUMBER - 2;
		// 	}
		// } while(dot[target_y * DOT_WIDTH_NUMBER + target_x].point <= POINT_WALL);
	}

	prev_x = x;
	prev_y = y;
	prev_color = color;

	same_target++;
	// printf("%d\n", same_target);
	// printf("%d %d\n", same_target, same_target_border);
	cout << "target_x, y " << target_x * SIZE << " " << target_y * SIZE << endl;
	if (GoToDot(target_x, target_y) || same_target > same_target_border)
	{
		prev_x = -1;
		same_target = 0;
		//fprintf(logfile, " %d End GoToDots() with returning 1\n", getRepeatedNum());
		return 1;
	}
	else
	{
		//fprintf(logfile, " %d End GoToDots() with returning 0\n", getRepeatedNum());
		return 0;
	}
}

int HowManyCurved(int id)
{
	/*
    道の長さ * 10 + 曲がった回数 * 20 + (Object < 6 のとき) Objectのとれる試算
    */
	int route[MAX_DOT_NUMBER];
	//曲がった回数
	int curved_times = 0;
	//道の長さ
	int distance_way = -1;
	route[0] = id;
	// printf("id is %d now is %d \n", id, now_dot_id);
	for (int i = 1; i < MAX_DOT_NUMBER; i++)
	{
		if (route[i - 1] < 0 || route[i - 1] > MAX_DOT_NUMBER)
		{
			//fprintf(errfile, " %d HowManyCurved() route[%d - 1] = %d is strange\n", getRepeatedNum(), i, route[i - 1]);
			distance_way = i + 1;
			break;
		}
		route[i] = dot[route[i - 1]].from;
		// printf("%d route[%d] = dot[route[%d - 1] = %d] = %d %d\n", distance_way, i, i, route[i - 1], dot[route[i - 1]].from, route[i]);
		//routeの最後の2つには-1を入れることにしている <- 嘘
		if (route[i] == now_dot_id)
		{
			distance_way = i + 1;
			break;
		}

		//dotの数を超えた場合
		if (route[i] >= MAX_DOT_NUMBER || route[i] < 0)
		{
			//fprintf(errfile, "%d HowManyCurved(): route[%d]の値が%dでおかしい\n", getRepeatedNum(), i, route[i]);
			//fprintf(logfile, "%d HowManyCurved(): route[%d]の値が%dでおかしい\n", getRepeatedNum(), i, route[i]);
			distance_way = i;
			break;
		}
	}
	// printf("distance_way = %d\n", distance_way);
	int x[MAX_DOT_NUMBER], y[MAX_DOT_NUMBER], direction[MAX_DOT_NUMBER];
	//directionは、左上=0で、右に行くごとに+1、下に行くごとに+3される
	if (distance_way >= MAX_DOT_NUMBER)
	{
		//fprintf(logfile, " %d Warming HowManyCurved(): routeの要素数が%dで MAX_DOT_NUMBER を超えている\n", getRepeatedNum(), distance_way);
		//fprintf(errfile, "%d Warming HowManyCurved(): routeの要素数が%dで MAX_DOT_NUMBER を超えている\n", getRepeatedNum(), distance_way);
		distance_way = MAX_DOT_NUMBER - 2;
	}
	y[0] = route[0] / DOT_WIDTH_NUMBER;
	x[0] = route[0] - y[0] * DOT_WIDTH_NUMBER;
	direction[0] = -1;
	for (int i = 1; i < distance_way; i++)
	{
		y[i] = route[i] / DOT_WIDTH_NUMBER;
		x[i] = route[i] - y[i] * DOT_WIDTH_NUMBER;
		switch (x[i] - x[i - 1])
		{
		case -1:
			direction[i] = 0;
			break;
		case 0:
			direction[i] = 1;
			break;
		case 1:
			direction[i] = 2;
			break;
		default:
			//fprintf(errfile, "%d HowManyCurved(): x[%d] = %d - x[%d - 1] = %dの値が%dでおかしい\n", getRepeatedNum(), i, x[i], i, x[i] - x[i - 1], x[i - 1]);
			//fprintf(logfile, " %d HowManyCurved(): x[%d] = %d - x[%d - 1] = %dの値が%dでおかしい\n", getRepeatedNum(), i, x[i], i, x[i] - x[i - 1], x[i - 1]);
			direction[i] = 5;
			break;
		}
		if (y[i] - y[i - 1] == 0)
		{
			direction[i] += 3;
		}
		else if (y[i] - y[i - 1] > 0)
		{
			//nothing
		}
		else
		{
			direction[i] += 6;
		}
		if (direction[i] != direction[i - 1])
		{
			curved_times++;
		}
	}
	return curved_times;
}

void GoToAngle(int angle, int distance)
{

	angle = angle - Compass;

	//180より大きい場合
	for (int i = 0; i < 10 && angle > 180; i++)
	{
		if (i == 9)
		{
			printf("GoToAngle(): angle's value:%d is abnormality\n", angle);
		}
		angle -= 360;
	}
	//-180より小さい場合
	for (int i = 0; i < 10 && angle < -180; i++)
	{
		if (i == 9)
		{
			printf("GoToAngle(): angle's value:%d is abnormality\n", angle);
		}
		angle += 360;
	}

	int classification = obstacle(5, 7, 5);
	// if (LoadedObjects >= 6)
	// {
	//     classification = 0;
	// }

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
		classification = obstacle(30, 40, 30);
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
				motor(4, 4);
			}
			else
			{
				if (angle < 0)
				{
					motor(3, -3);
				}
				else
				{
					motor(-3, 3);
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
			if (log_superobj_num > 0 && pow(log_y - log_superobj_y[0], 2) + pow(log_x - log_superobj_x[0], 2) < 800)
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
			else if (IsNearYellow(2, -1, -1))
			{
				printf("near yellow\n");
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
						motor(2, -2);
					}
					else
					{
						motor(-2, 2);
					}
				}
				else
				{
					if (angle < 0)
					{
						motor(3, -3);
					}
					else
					{
						motor(-3, 3);
					}
				}
			}
			// else if (LoadedObjects == 6) {
			// 	if (abs(angle) < 10) {
			// 		motor(5, 5);
			// 	}
			// 	else if (abs(angle) < 90) {
			// 		if (angle < 0) {
			// 			motor(5, 3);
			// 		}
			// 		else {
			// 			motor(3, 5);
			// 		}
			// 	}
			// 	else if (abs(angle) < 120) {
			// 		if (angle < 0) {
			// 			motor(2, -2);
			// 		}
			// 		else {
			// 			motor(-2, 2);
			// 		}
			// 	}
			// 	else {
			// 		if (angle < 0) {
			// 			motor(3, -3);
			// 		}
			// 		else {
			// 			motor(-3, 3);
			// 		}
			// 	}
			// }
			else if ((loaded_objects[BLACK_LOADED_ID] < 2 && dot[now_dot_id].black == 1) || (loaded_objects[CYAN_LOADED_ID] < 2 && dot[now_dot_id].cyan == 1) || (loaded_objects[RED_LOADED_ID] < 2 && dot[now_dot_id].red == 1))
			{
				if (abs(angle) < 10)
				{
					if (rand() % 4)
					{
						motor(4, 4);
					}
					else
					{
						motor(5, 5);
					}
				}
				else if (abs(angle) < 80)
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
				else if (abs(angle) < 120)
				{
					if (angle < 0)
					{
						motor(5, 0);
					}
					else
					{
						motor(0, 5);
					}
				}
				else
				{
					if (angle < 0)
					{
						motor(2, -2);
					}
					else
					{
						motor(-2, 2);
					}
				}
			}
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
						if (dot[now_dot_id].point == POINT_WHITE && IsOnSwampland())
						{
							if (IsOnSwampland() == 1)
							{
								motor(5, 2);
							}
							else if (IsOnSwampland() == 2)
							{
								motor(2, 5);
							}
						}
						else
						{
							motor(5, 5);
						}
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
						motor(4, -4);
					}
					else
					{
						motor(-4, 4);
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
}

void AutoStrategy(void)
{
	/*
	30 x 30のエリアを順番に行く
	*/
}
