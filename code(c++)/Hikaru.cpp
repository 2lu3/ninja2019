#include "Hikaru.hpp"

#define FIND_OBJ_DURATION 50
#define ACCURACY 5
#define REP(i, n) for (int i = 0; i < n; i++)

using namespace std;

#define POINT_WHITE 1
#define POINT_DEPOSIT 0
#define POINT_SWAMPLAND -1
#define POINT_WALL -2
#define POINT_YELLOW -3

#define COSPACE_WIDTH 360
#define COSPACE_HEIGHT 270
#define SIZE 10
#define DOT_WIDTH_NUMBER (COSPACE_WIDTH / SIZE)
#define DOT_HEIGHT_NUMBER (COSPACE_HEIGHT / SIZE)
#define MAX_DOT_NUMBER (COSPACE_WIDTH * COSPACE_HEIGHT / SIZE / SIZE)
#define MAX_EDGE_NUMBER 25
#define BORDER_SAME_OBJECT_NUMBER 2
#define FIND_OBJECT_DURATION 50
#define PLUSMINUS(a, b, c) ((a) <= (b) + (c) && (a) >= (b) - (c))

struct Dot
{
	int x, y;	//coordinate of dot
	int wide;	//the lengh of edge of dot
	int point; //the kind of dot(yellow, wall, deposit ...)
	int done;	//Dijkstra()
	int id;		 //y * 36 + x
	int from;	//Dijkstra()
	int cost;	//Dijkstra()
	int edge_num;
	int edge_to[MAX_EDGE_NUMBER];
	int edge_cost[MAX_EDGE_NUMBER];
};
struct Dot dot[MAX_DOT_NUMBER];

int log_x, log_y, log_compass;
int emergency_now_dot_id = 447;
int now_dot_id = -1;
int log_superobj_num = 0, log_superobj_x[10], log_superobj_y[10];
int process = 0;
int super_sameoperate = 0;

void Game0_Hikaru::setup(void)
{
	ProcessingTime pt;
	pt.start();

	delErrorFile();
	delLogFile();
	InputColorInformation();

	double seconds = pt.end();
	logMessage("game0 setup() : " + to_string(seconds) + " milliseconds", 2);
}
void Game0_Hikaru::loop(void)
{
	logMessage("World1 loop start");
	if (SuperDuration > 0)
	{
		SuperDuration--;
	}
	else if (IsOnDepositArea() && LoadedObjects >= 4)
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
	else if (EitherColorJudge(black_obj) && loaded_objects[BLACK_LOADED_ID] < 4 && LoadedObjects < 6)
	{
		logMessage("find black obj");
		setAction(FIND_OBJ);
		loaded_objects[BLACK_LOADED_ID]++;
		SuperDuration = FIND_OBJ_DURATION;
	}
	else if (EitherColorJudge(cyan_obj) && loaded_objects[CYAN_LOADED_ID] < 4 && LoadedObjects < 6)
	{
		logMessage("find cyan obj");
		setAction(FIND_OBJ);
		loaded_objects[CYAN_LOADED_ID]++;
		SuperDuration = FIND_OBJ_DURATION;
	}
	else if (EitherColorJudge(red_obj) && loaded_objects[RED_LOADED_ID] < 4 && LoadedObjects < 6)
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
		if (LoadedObjects >= 4)
		{
			motor(-5, -1);
		}
		else
		{
			switch (IsOnYellowLine())
			{
			case 1:
				motor(1, -3);
				break;
			case 2:
				motor(-3, 1);
				break;
			case 3:
				motor(-3, 1);
				break;
			default:
				break;
			}
		}
		Duration = 1;
	}
	else if (LoadedObjects >= 4)
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
			motor(2, 3);
		}
		else if (US_Right < 40)
		{
			motor(4, 2);
		}
		else if (US_Right < 80)
		{
			motor(3, 1);
		}
		else
		{
			motor(3, 1);
		}
	}
	else
	{
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
		else if (US_Right < 15 + rand() % 30)
		{
			motor(3, 4);
		}
		else if (US_Right < 40 + rand() % 30)
		{
			motor(3, 2);
		}
		else if (US_Right < 80)
		{
			motor(3, 1);
		}
		else
		{
			motor(3, 2);
		}
	}

	/*if (Time > 180 && getAction() != FIND_OBJ && getAction() != DEPOSIT_OBJ && (!EitherColorJudge(object_box) && LoadedObjects < 3) || Time > 200)
	{
		logMessage("Teleport");
		setAction(TELEPORT);
	}*/

	if (Time > 298)
	{
		setAction(TELEPORT);
	}

	switch (getAction())
	{
	case DEFINED:
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
		for (int i = 0; i < sizeof(loaded_objects) / sizeof(*loaded_objects); i++)
		{
			loaded_objects[i] = 0;
		}

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
		for (int i = 0; i < sizeof(loaded_objects) / sizeof(*loaded_objects); i++)
		{
			loaded_objects[i] = 0;
		}
		CurGame = 1;
		break;
	default:
		errorMessage("action is " + to_string(getAction()));
		break;
	}
	logMessage("World1 Loop End");
}

int CheckNowDot(void);
void InputDotInformation(void);
int GoToDots(int x, int y, int wide_decide_x, int wide_decide_y);

void Game1_Hikaru::setup(void)
{
	setRunMode(MODE_NORMAL);
	setDefaultRunMode(MODE_NORMAL);
	srand((unsigned int)time(NULL));
	InputDotInformation();
	LoadedObjects = 0;
	delErrorFile();
	delLogFile();
	REP(i, 4)
	{
		loaded_objects[i] = 0;
	}

	if (PositionX == 0)
	{
		log_y = emergency_now_dot_id / DOT_WIDTH_NUMBER;
		log_x = emergency_now_dot_id - emergency_now_dot_id * DOT_WIDTH_NUMBER;
		log_x *= SIZE;
		log_y *= SIZE;

		now_dot_id = emergency_now_dot_id;
	}
}

void Game1_Hikaru::loop(void)
{
	logMessage("World 1 start" + process);
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
	}
	else
	{
		PositionX = -1;
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
	else if (EitherColorJudge(red_obj) && LoadedObjects < 6)
	{
		setAction(FIND_OBJ);
		loaded_objects[0]++;
		SuperDuration = FIND_OBJECT_DURATION;
	}
	else if (EitherColorJudge(cyan_obj) && LoadedObjects < 6)
	{
		setAction(FIND_OBJ);
		loaded_objects[1]++;
		SuperDuration = FIND_OBJECT_DURATION;
	}
	else if (EitherColorJudge(black_obj) && LoadedObjects < 6)
	{
		setAction(FIND_OBJ);
		loaded_objects[2]++;
		SuperDuration = FIND_OBJECT_DURATION;
	}
	else if (EitherColorJudge(sp_obj) && SuperObj_Num == 0 && log_superobj_num > 0)
	{
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

		//I delete log_superobj_x[2], I have to move the data of log_superobj_x[3] to [2].
		for (int i = num + 1; i < log_superobj_num; i++)
		{
			log_superobj_x[i] = log_superobj_x[i - 1];
			log_superobj_y[i] = log_superobj_y[i - 1];
		}
		loaded_objects[3]++;
		log_superobj_num--;
	}
	else if (EitherColorJudge(trap_line) && LoadedObjects > 0)
	{
		motor(-5, -3);
		SuperDuration = 3;
	}
	else if (Duration > 0)
	{
		Duration--;
	}
	else if (EitherColorJudge(object_box) && LoadedObjects >= 4)
	{
		process = 0;
		if (BothColorJudge(object_box))
		{
			setAction(DEPOSIT_OBJ);
		}
		else if (ColorJudgeLeft(object_box))
		{
			motor(0, 5);
		}
		else
		{
			motor(5, 0);
		}
	}
	else
	{
		if (LoadedObjects >= 6)
		{
			GoToDots(260, 60, 3, 3);
		}
		else if (process == 0)
		{
			if (GoToDots(200, 50, 20, 20))
			{
				process++;
			}
		}
		else if (process == 1)
		{
			if (GoToDots(200, 210, 20, 20))
			{
				process++;
			}
		}
		else if (process == 2)
		{
			if (GoToDots(75, 140, 20, 20))
			{
				process = 0;
			}
		}
		else
		{
			motor(3, 3);
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
			if (!BothColorJudge(object_box))
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
	if (super_sameoperate > 500)
	{
		log_superobj_num = 0;
		super_sameoperate = 0;
	}

	logMessage("World 1 end");
}

long WhereIsmotor(void)
{
	long x, y;
	if (log_x < 0 || log_x >= COSPACE_WIDTH || log_y < 0 || log_y >= COSPACE_HEIGHT)
	{
		errorMessage("WhereIsmotor(): log_x, log_y = (" + to_string(log_x) + "," + to_string(log_y) + ")", MODE_NORMAL);
		log_x = COSPACE_WIDTH / 2;
		log_y = COSPACE_HEIGHT / 2;
	}
	x = log_x - (long)cos((double)(Compass + 90) * 3.14 / 180) * 5;
	y = log_y - (long)sin((double)(Compass + 90) * 3.14 / 180) * 5;
	if (x < 0)
	{
		x = 0;
	}
	if (y < 0)
	{
		y = 0;
	}
	if (x >= COSPACE_WIDTH)
	{
		x = COSPACE_WIDTH - 1;
	}
	if (y >= COSPACE_HEIGHT)
	{
		y = COSPACE_HEIGHT - 1;
	}
	return y * 1000 + x;
}
long WhereIsColorSensor(void)
{
	long x, y;
	if (log_x < 0 || log_x >= COSPACE_WIDTH || log_y < 0 || log_y >= COSPACE_HEIGHT)
	{
		errorMessage("WhereIsColorSensor(): log_x, log_y = (" + to_string(log_x) + ", " + to_string(log_y) + ") ", MODE_NORMAL);
		log_x = COSPACE_WIDTH / 2;
		log_y = COSPACE_HEIGHT / 2;
	}
	x = log_x + (int)(cos((double)(Compass + 90) * 3.14 / 180) * 5);
	y = log_y + (int)(sin((double)(Compass + 90) * 3.14 / 180) * 5);
	if (x < 0)
	{
		x = 0;
	}
	if (y < 0)
	{
		y = 0;
	}
	if (x >= COSPACE_WIDTH)
	{
		x = COSPACE_WIDTH - 1;
	}
	if (y >= COSPACE_HEIGHT)
	{
		y = COSPACE_HEIGHT - 1;
	}
	return y * 1000 + x;
}

int CheckNowDot(void)
{
	int x = WhereIsColorSensor();
	int y = x / 1000;
	x = x - y * 1000;
	if (x < 0 || x >= COSPACE_WIDTH || y < 0 || y >= COSPACE_HEIGHT)
	{
		errorMessage("CheckNowDot(): (x, y) = (" + to_string(x) + ", " + to_string(y) + ") ", MODE_NORMAL);
		if (x < 0)
		{
			x = SIZE;
		}
		else
		{
			x = COSPACE_WIDTH - SIZE - 1;
		}
		if (y < 0)
		{
			y = SIZE;
		}
		else
		{
			y = COSPACE_HEIGHT - SIZE;
		}
	}

	x = x / SIZE;
	y = y / SIZE;

	// if the coordinate is in wall
	if (dot[y * DOT_WIDTH_NUMBER + x].point <= POINT_WALL)
	{
		errorMessage("CheckNowDot(): I am in wall or yellow", MODE_NORMAL);
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
			errorMessage("CheckNowDot(): I( " + to_string(x) + ", " + to_string(y) + ") and around me are in wall or yellow", MODE_NORMAL);
			return emergency_now_dot_id;
		}
	}
	return y * DOT_WIDTH_NUMBER + x;
}

int IsNearYellow(int num, int x, int y)
{
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
			errorMessage("IsNearYellow(): now_dot_id is " + to_string(now_dot_id) + " (" + to_string(x) + ", " + to_string(y) + ")", MODE_NORMAL);
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
			return 1;
		}
	}
	return 0;
}

void GoToAngle(int angle, int distance)
{
	if (distance < 0)
	{
		errorMessage("GoToAngle(): distance's value is " + to_string(distance) + " < 0", MODE_NORMAL);
	}

	angle = angle - Compass;

	angle %= 360;

	if (angle > 180)
	{
		angle -= 360;
	}
	else if (angle < -180)
	{
		angle += 360;
	}

	int classification = obstacle(5, 7, 5);
	if (LoadedObjects >= 6)
	{
		classification = 0;
	}

	// double magnification = 0.3;
	int short_front = 1; //(int)(pow(US_Front, magnification) * (5 - (WheelLeft * WheelLeft + WheelRight * WheelRight) / 8) / pow(25, magnification));
	int short_left = 2;	//(int)(pow(US_Left, magnification) * (5 - (WheelLeft * WheelLeft + WheelRight * WheelRight) / 8) / pow(25, magnification));
	int short_right = 2; //(int)(pow(US_Right, magnification) * (5 - (WheelLeft * WheelLeft + WheelRight * WheelRight) / 8) / pow(25, magnification));
	int max_value = 4;
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
		classification = obstacle(20, 35, 20);
		if (LoadedObjects >= 6)
		{
			classification = 0;
		}
		if (classification == 1 && angle > 0 && angle < 90)
		{ //left
			motor(max_value, short_left);
		}
		else if (classification == 2 && abs(angle) < 30)
		{ //front
			if (angle < 0)
			{
				motor(max_value, short_front);
			}
			else
			{
				motor(short_front, max_value);
			}
		}
		else if (classification == 3 && angle > -30 && angle < 90)
		{ //left & front
			motor(max_value, (short_left < short_front) ? (short_left) : (short_right));
		}
		else if (classification == 4 && angle < 0 && angle > -90)
		{ //right
			motor(short_right, max_value);
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
			motor((short_right < short_front) ? (short_right) : (short_right), max_value);
		}
		else if (classification == 7)
		{ //all
			if (angle < 0)
			{
				motor(max_value, short_front);
			}
			else
			{
				motor(short_front, max_value);
			}
		}
		else
		{
			if (log_superobj_num > 0 && pow(log_y - log_superobj_y[0], 2) + pow(log_x - log_superobj_x[0], 2) < 800)
			{
				printf("log_superobj_num > 0 %d\n", angle);
				if (abs(angle) < 30)
				{
					if (distance < 7)
					{
						motor(-1, -1);
					}
					else
					{
						motor(max_value, max_value);
					}
				}
				else if (abs(angle) < 100)
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
						motor(0, -2);
					}
					else
					{
						motor(-2, 0);
					}
				}
				else
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
			}
			else if (EitherColorJudge(gray_zone))
			{
				if (abs(angle) < 30)
				{
					motor(max_value, max_value);
				}
				else if (abs(angle) < 90)
				{
					if (angle < 0)
					{
						motor(max_value, 3);
					}
					else
					{
						motor(3, max_value);
					}
				}
				else
				{
					if (angle < 0)
					{
						motor(max_value, -max_value);
					}
					else
					{
						motor(-max_value, max_value);
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
			else
			{
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
						if (dot[now_dot_id].point == POINT_WHITE && EitherColorJudge(gray_zone))
						{
							if (ColorJudgeLeft(gray_zone))
							{
								motor(max_value, 2);
							}
							else if (ColorJudgeRight(gray_zone))
							{
								motor(2, max_value);
							}
						}
						else
						{
							motor(max_value, max_value);
						}
					}
				}
				else if (abs(angle) < 60)
				{
					if (angle < 0)
					{
						motor(max_value, 3);
					}
					else
					{
						motor(3, max_value);
					}
				}
				else if (abs(angle) < 120)
				{
					if (angle < 0)
					{
						motor(max_value, 2);
					}
					else
					{
						motor(2, max_value);
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
		motor(-2, -3);
		break;
	case 7: //left front right
		motor(-3, -3);
		break;
	default:
		break;
	}
}

int GoToPosition(int x, int y, int wide_decide_x, int wide_decide_y, int wide_judge_arrived)
{
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

		errorMessage("GoToPosition(): arguments value range error", MODE_MATCH);
		return 0;
	}

	//if absolute_x and y is not in the range of x and y
	if (absolute_x == -1 || !(PLUSMINUS(absolute_x, x, wide_decide_x) && PLUSMINUS(absolute_y, y, wide_decide_y)))
	{
		int i = 0;
		do
		{
			if (i > 5)
			{
				absolute_x = COSPACE_WIDTH / 2;
				absolute_y = COSPACE_HEIGHT / 2;
				errorMessage("warming GoToPosition(): absolute_x, absolute_yが決まりません\n", MODE_MATCH);
				break;
			}
			absolute_x = x - wide_decide_x + rand() % (wide_decide_x * 2 + 1);
			absolute_y = y - wide_decide_y + rand() % (wide_decide_y * 2 + 1);
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
		logMessage("(" + to_string(x) + ", " + to_string(y) + ") arrive because too many same_operate", MODE_NORMAL);
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
		logMessage("(" + to_string(absolute_x) + ", " + to_string(absolute_y) + ")に到着しました", MODE_NORMAL);
		absolute_x = -1;
		absolute_y = -1;
		same_operate = -1;
		absolute_distance = -1;
		return 1;
	}

	logMessage("absolute x, y (" + to_string(absolute_x) + ", " + to_string(absolute_y) + ")", MODE_DEBUG);
	x = absolute_x;
	y = absolute_y;
	x = x - temp_x;
	y = y - temp_y;
	logMessage("x, y = (" + to_string(x) + ", " + to_string(y) + ")", MODE_DEBUG);
	double angle = atan2(y, x);
	angle = angle * 180 / 3.14;

	int angle_int = (int)angle;
	angle_int -= 90;
	if (angle_int < 0)
	{
		angle_int += 360;
	}
	GoToAngle(angle_int, sqrt(x * x + y * y));

	if (repeated_num_log + 1 == getRepeatedNum())
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
	int map_position_data[36][27] = {
			{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -2, -2, -2, 1, -3, -3, -3, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -2, -2, -2, 1, -3, -3, -3, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, -3, -3, -3, 1, -1, -1, -1, -1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, -3, -3, -3, -1, -1, -1, -1, -1, 1, 1, 1}, {1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 0, 0, 0, 0, 0, -3, -3, -3, -1, -1, -1, -1, -2, -2, 1, 1}, {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, -3, -1, -1, -1, -1, -1, -2, -2, 1, 1}, {-1, -1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, 1, 1, 1, -3, -3, -3, -3, -2, -2, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, 1, 1, 1, -3, -3, -3, -3, -3, -3, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1, -3, -3, 0, 0, -3, -3, -3, -3, -3, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -3, -3, -3, 0, 0, 0, -3, -3, -3, -3, 1, 1}, {1, 1, -2, 1, 1, 1, 1, 1, 1, 1, 1, -1, -1, 1, -3, -3, -3, -3, 0, 0, 0, 0, 1, 1, 1, 1, 1}, {1, -2, -2, -2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -3, -3, -3, -3, 0, 0, 0, 0, 1, 1, 1, 1, 1}, {1, -2, -2, -2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -3, -3, -3, -3, 1, 0, 0, 0, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -3, -3, -3, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -2, -2, -2, -2, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -2, -2, -2, -2, -2, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -2, -2, -2, -2, -2, -2, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -2, -2, -2, -2, -2, -2, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, -3, -3, 1, 1, 1, 1, -2, -2, -2, -2, -2, -2, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, -3, -3, -3, 1, 1, -1, -1, -2, -2, -2, -2, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, -3, -3, -3, 1, -1, -1, -1, -1, 1, 1, 1, 1, 1, 1, 1, 1, -2, -2, 1}, {1, 1, 1, 0, 0, 0, 0, 0, -3, -3, -3, 1, -1, -1, -1, -1, 1, 1, 1, 1, 1, 1, 1, 1, -2, -2, 1}, {1, 1, 1, 0, 0, 0, 0, 0, -3, -3, -3, -1, -1, -1, -1, -1, 1, 1, 1, 1, 1, 1, 1, 1, -2, -2, 1}, {1, 1, 1, 1, 0, 0, 0, 0, 0, -3, -3, -1, -1, -1, -1, -3, -3, -3, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 0, -1, -1, 1, 1, 1, -1, -3, -3, -3, -3, -3, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 1, -3, -3, -3, -3, -3, -3, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {-1, -1, -1, -1, -1, -1, -1, -1, -1, -3, -3, 0, 0, -3, -3, -3, -3, -3, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, -1, -1, -1, -1, -1, -3, -3, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, -1, -1, -1, -1, -3, -3, -3, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, -1, -1, -1, 1, -3, -3, -3, -3, 0, 0, 0, 0, 1, 1, 1, -2, -2, -2, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, -3, -3, -3, -3, 0, 0, 0, 0, 1, 1, 1, -2, -2, -2, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, -3, -3, -3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, -3, -3, -3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

	short map_position_color_data[DOT_WIDTH_NUMBER][DOT_HEIGHT_NUMBER];
	for (int i = 0; i < DOT_WIDTH_NUMBER; i++)
	{
		for (int j = 0; j < DOT_HEIGHT_NUMBER; j++)
		{
			// map_position_color_data[i][j] = POINT_WHITE;
			map_position_color_data[i][j] = map_position_data[i][j];
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
	}

	//set values of cost
	for (long i = 0; i < MAX_DOT_NUMBER; i++)
	{
		int y = i / DOT_WIDTH_NUMBER;
		int x = i - y * DOT_WIDTH_NUMBER;
		dot[i].edge_num = 0;
		for (int j = 0; j < 9; j++)
		{
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
				dot[i].edge_cost[dot[i].edge_num] = (dot[i].wide + dot[target_id].wide) * 1000;
			}
			//マップの端
			else if (x == 0 || y == 0 || x == DOT_WIDTH_NUMBER - 1 || y == DOT_HEIGHT_NUMBER - 1 || temp_x == 0 || temp_y == 0 || temp_x == DOT_WIDTH_NUMBER - 1 || temp_y == DOT_HEIGHT_NUMBER - 1)
			{
				dot[i].edge_cost[dot[i].edge_num] = (dot[i].wide + dot[target_id].wide) * 1000;
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
}

void Dijkstra(int option)
{
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

	// if(dot[now_node_id].point < 0) {
	option = 1;
	// }

	struct Dot investigating_node;

	// int number = 0;
	while (true)
	{
		// number++;
		//init investigating_node
		investigating_node.done = 0;
		for (int i = 0; i < MAX_DOT_NUMBER; i++)
		{
			//if done is 0, it means already
			if (dot[i].done == 0 || dot[i].cost < 0)
			{
				continue;
			}

			//If the dot is yellow or wall
			if (dot[i].point < POINT_SWAMPLAND)
			{
				continue;
			}

			// if it is not entered
			if (investigating_node.done == 0)
			{
				investigating_node = dot[i];
				continue;
			}

			//if the new dot's cost is not big
			if (dot[i].cost < investigating_node.cost)
			{
				investigating_node = dot[i];
			}
		}

		// if it is new dot
		if (investigating_node.done == 0)
		{
			break;
		}

		dot[investigating_node.id].done = 0;

		for (int i = 0; i < investigating_node.edge_num; i++)
		{
			int target_id = investigating_node.edge_to[i];
			if (target_id < 0 || target_id >= MAX_DOT_NUMBER)
			{
				printf(" %d DIjkstra() a edge have error dot[%d].edge_to[%d] = %d\n", getRepeatedNum(), investigating_node.id, i, target_id);
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
			int target_cost = investigating_node.cost + investigating_node.edge_cost[i];

			if (dot[target_id].cost < 0 || target_cost < dot[target_id].cost)
			{
				dot[target_id].cost = target_cost;
				dot[target_id].from = investigating_node.id;
			}
			else
			{
				dot[target_id].from = remember_from;
			}
		}
	}
}

int IsThereobstacle(int x, int y, int target_x, int target_y)
{
	return 0;
}

int GoToDot(int x, int y)
{
	static int prev_x = -1, prev_y = -1, prev_now_dot_id = -1;

	if (PositionX == -1 && PLUSMINUS(log_x / SIZE, x, 2) && PLUSMINUS(log_y / SIZE, y, 2))
	{
		errorMessage("GoToDot() : End GoToDot() with returning 1 because I am in PLA and it's near target( + " + to_string(x) + ", " + to_string(y) + ")", MODE_NORMAL);
		return 1;
	}

	char map_data_to_show[MAX_DOT_NUMBER];
	for (int i = 0; i < MAX_DOT_NUMBER; i++)
	{
		if (dot[i].point <= POINT_WALL)
		{
			map_data_to_show[i] = '*';
		}
		else
		{
			map_data_to_show[i] = ' ';
		}
	}

	//If the node I want to go will be go out
	if (x < 1 || x >= DOT_WIDTH_NUMBER - 1 || y < 1 || y >= DOT_HEIGHT_NUMBER - 1)
	{
		errorMessage("GoToDot(): (x, y) is (" + to_string(x) + "," + to_string(y) + ") and strange", MODE_MATCH);
		if (x < 0 || y >= DOT_WIDTH_NUMBER || y < 0 || y >= DOT_HEIGHT_NUMBER)
		{
			errorMessage("GoToDot() : (x, y) is not in cospace area", MODE_MATCH);
			if (getRunMode() >= MODE_MATCH)
			{
				y = emergency_now_dot_id / DOT_WIDTH_NUMBER;
				x = emergency_now_dot_id - y * DOT_WIDTH_NUMBER;
			}
			else
			{
				return 1;
			}
		}
	}

	if (prev_now_dot_id != now_dot_id || prev_x != x || prev_y != y)
	{
		Dijkstra(0);
	}
	prev_now_dot_id = now_dot_id;
	prev_x = x;
	prev_y = y;

	int goal_dot = y * DOT_WIDTH_NUMBER + x;

	if (goal_dot < 0 || goal_dot >= MAX_DOT_NUMBER)
	{
		errorMessage("strange (x,y)", MODE_NORMAL);
		return 0;
	}

	int temp = goal_dot;
	map_data_to_show[goal_dot] = 'T';
	int i = 0;

	while (dot[temp].from != now_dot_id && i < 200)
	{
		// int go_x, go_y;
		// go_y = temp / DOT_WIDTH_NUMBER;
		// go_x = temp - (int)go_y * DOT_WIDTH_NUMBER;
		temp = dot[temp].from;
		map_data_to_show[temp] = '#';
		// printf("%d\n", dot[temp].point);
		i++;
		if (temp < 0 || temp >= MAX_DOT_NUMBER)
		{
			errorMessage("GoToDot() temp = " + to_string(temp) + " is srange. I will continue", MODE_MATCH);
			GoToPosition(x * SIZE, y * SIZE, 5, 5, 5);
			return 0;
		}
	}
	if (i == 200)
	{
		errorMessage("GoToDot() : iの値が200です", MODE_NORMAL);
	}

	map_data_to_show[now_dot_id] = '@';

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
	system("cls");
	for (int i = 0; i < DOT_WIDTH_NUMBER + 2; i++)
	{
		printf("|");
	}
	printf("\n");
	for (int i = DOT_HEIGHT_NUMBER - 1; i >= 0; i--)
	{
		printf("|");
		for (int j = 0; j < DOT_WIDTH_NUMBER; j++)
		{
			int id = i * DOT_WIDTH_NUMBER + j;
			printf("%c", map_data_to_show[id]);
		}
		printf("|");
		printf("\n");
	}
	for (int i = 0; i < DOT_WIDTH_NUMBER + 2; i++)
	{
		printf("|");
	}
	printf("\n");
	return 0;
}

int GoToDots(int x, int y, int wide_decide_x, int wide_decide_y)
{
	// printf("GoToDots(): %d %d %d %d\n", x, y, wide_decide_x, wide_decide_y);
	static int prev_x = -1;
	static int prev_y = -1;
	static int target_x = -1;
	static int target_y = -1;
	static int same_target = 0;
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
				printf(" %d GoToDots() corner_x[%d] is %d < 0\n", getRepeatedNum(), i, corner_x[i]);
				corner_x[i] = 0;
			}
			if (corner_x[i] >= DOT_WIDTH_NUMBER)
			{
				printf(" %d GoToDots() corner_x[%d] is %d >= %d\n", getRepeatedNum(), i, corner_x[i], DOT_WIDTH_NUMBER);
				corner_x[i] = DOT_WIDTH_NUMBER - 1;
			}
			if (corner_y[i] < 0)
			{
				printf(" %d GoToDots() corner_y[%d] is %d < 0\n", getRepeatedNum(), i, corner_y[i]);
				corner_y[i] = 0;
			}
			if (corner_y[i] >= DOT_HEIGHT_NUMBER)
			{
				printf(" %d GoToDots() corner_y[%d] is %d >= %d\n", getRepeatedNum(), i, corner_y[i], DOT_HEIGHT_NUMBER);
				corner_y[i] = DOT_HEIGHT_NUMBER - 1;
			}
		}

		int min = 100000, id = -1;
		for (int i = corner_x[0]; i <= corner_x[1]; i++)
		{
			for (int j = corner_y[0]; j <= corner_y[1]; j++)
			{
				string message = "";
				message += i;
				message += ' ';
				message += j;
				message += ' ';
				int investigating_dot_id = j * DOT_WIDTH_NUMBER + i;
				if (i <= 0 || i >= DOT_WIDTH_NUMBER - 1 || j <= 0 || j >= DOT_HEIGHT_NUMBER - 1)
				{
					message += "rejection of range";
					continue;
				}

				//yellow or wall or deposit
				if (dot[investigating_dot_id].point <= POINT_DEPOSIT)
				{
					message += "refection of point";
					continue;
				}

				int costs = rand() % 10 - pow(i * SIZE - log_x, 2) / 100 - pow(j * SIZE - log_y, 2) / 100;

				if (costs < min)
				{
					min = costs;
					id = investigating_dot_id;
					message += "cost updated";
				}
				else
				{
					message += "cost unupdated";
				}
				logMessage(message, MODE_VERBOSE);
			}
		}
		if (id == -1)
		{
			printf("%d GoToDots(): There is no dot that can go\n", getRepeatedNum());
			return 0;
		}
		else
		{
			target_y = id / DOT_WIDTH_NUMBER;
			target_x = id - target_y * DOT_WIDTH_NUMBER;
		}
	}

	same_target++;
	printf("%d\n", same_target);
	if (GoToDot(target_x, target_y) || same_target > 100)
	{
		prev_x = -1;
		same_target = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}

int GoInDots(int x, int y, int wide_decide_x, int wide_decide_y, int color)
{
	static int prev_x = -1;
	static int prev_y = -1;
	static int target_x = -1;
	static int target_y = -1;
	static int same_target = 0;
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
				printf(" %d GoToDots() corner_x[%d] is %d < 0\n", getRepeatedNum(), i, corner_x[i]);
				corner_x[i] = 0;
			}
			if (corner_x[i] >= DOT_WIDTH_NUMBER)
			{
				printf(" %d GoToDots() corner_x[%d] is %d >= %d\n", getRepeatedNum(), i, corner_x[i], DOT_WIDTH_NUMBER);
				corner_x[i] = DOT_WIDTH_NUMBER - 1;
			}
			if (corner_y[i] < 0)
			{
				printf(" %d GoToDots() corner_y[%d] is %d < 0\n", getRepeatedNum(), i, corner_y[i]);
				corner_y[i] = 0;
			}
			if (corner_y[i] >= DOT_HEIGHT_NUMBER)
			{
				printf(" %d GoToDots() corner_y[%d] is %d >= %d\n", getRepeatedNum(), i, corner_y[i], DOT_HEIGHT_NUMBER);
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
				if (dot[investigating_dot_id].point <= POINT_DEPOSIT)
				{
					continue;
				}

				int costs = rand() % 10 - pow(i * SIZE - log_x, 2) / 25 - pow(j * SIZE - log_y, 2) / 25;
				if (costs < min)
				{
					min = costs;
					id = investigating_dot_id;
				}
			}
		}
		if (id == -1)
		{
			printf("%d GoToDots(): There is no dot that can go\n", getRepeatedNum());
			target_x = x / SIZE;
			target_y = y / SIZE;
		}
		else
		{
			target_y = id / DOT_WIDTH_NUMBER;
			target_x = id - target_y * DOT_WIDTH_NUMBER;
		}
	}

	same_target++;
	printf("%d\n", same_target);
	if (PLUSMINUS(log_x, x, wide_decide_x) && PLUSMINUS(log_y, y, wide_decide_y))
	{
		motor(5, 5);
	}
	if (GoToDot(target_x, target_y) || same_target > 100)
	{
		prev_x = -1;
		same_target = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}
