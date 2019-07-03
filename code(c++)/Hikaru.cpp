/*
todo : 回転するときに、カラーセンサが沼地に入ってしまう問題を解決
オドメトリを用いて、PLAでTrapに入る問題を解決
360 x 270にすること
*/
#include "Hikaru.hpp"
#include "MapData.hpp"
#include <iostream>

#define IF if
#define LOG_MESSAGE(MESSAGE, OPTION) \
	IF((OPTION) <= getRunMode()) { logErrorMessage.logMessage((MESSAGE), (OPTION)); }
#define ERROR_MESSAGE(MESSAGE, OPTION) \
	IF((OPTION) <= getRunMode()) { logErrorMessage.errorMessage((MESSAGE), (OPTION)); }

#define POINT_BLACK 20
#define POINT_RED 10
#define POINT_CYAN 15
#define POINT_SUPERAREA 2
#define POINT_WHITE 1
#define POINT_DEPOSIT 0
#define POINT_SWAMPLAND -1
#define POINT_WALL -2
#define POINT_YELLOW -3

#define FUNC_NAME getFuncName(__FUNCTION__)

using namespace std;

void Game0_Hikaru::setup(void)
{
	pt.start();

	UserGame0::setup();

	logErrorMessage.delErrorFile();
	logErrorMessage.delLogFile();
	InputColorInformation();

	double seconds = pt.end();
	LOG_MESSAGE("game0 setup() : " + to_string(seconds) + " milliseconds", MODE_NORMAL);
}

void Game0_Hikaru::loop(void)
{
	UserGame0::loop();
	pt.start();
	LOG_MESSAGE("World1 loop start", MODE_NORMAL);
	if (SuperDuration > 0)
	{
		SuperDuration--;
	}
	else if (IsOnDepositArea() && LoadedObjects >= 5)
	{
		LOG_MESSAGE("find object box", MODE_DEBUG);
		switch (IsOnDepositArea())
		{
		case 1:
			LOG_MESSAGE("Left Sensor is in object_box", MODE_DEBUG);
			motor(0, 5);
			break;
		case 2:
			LOG_MESSAGE("Right Sensor is in object_box", MODE_DEBUG);
			motor(5, 0);
			break;
		case 3:
			LOG_MESSAGE("I am in object_box", MODE_DEBUG);
			setAction(DEPOSIT_OBJ);
			SuperDuration = 50;
			break;
		default:
			break;
		}
	}
	else if (EitherColorJudge(black_obj) && loaded_objects[BLACK_LOADED_ID] < kBorderSameObjNum && LoadedObjects < 6)
	{
		LOG_MESSAGE("find black obj", MODE_NORMAL);
		setAction(FIND_OBJ);
		loaded_objects[BLACK_LOADED_ID]++;
		SuperDuration = kFindObjDuration;
	}
	else if (EitherColorJudge(cyan_obj) && loaded_objects[CYAN_LOADED_ID] < kBorderSameObjNum && LoadedObjects < 6)
	{
		LOG_MESSAGE("find cyan obj", MODE_NORMAL);
		setAction(FIND_OBJ);
		loaded_objects[CYAN_LOADED_ID]++;
		SuperDuration = kFindObjDuration;
	}
	else if (EitherColorJudge(red_obj) && loaded_objects[RED_LOADED_ID] < kBorderSameObjNum && LoadedObjects < 6)
	{
		LOG_MESSAGE("find red obj", MODE_NORMAL);
		setAction(FIND_OBJ);
		loaded_objects[RED_LOADED_ID]++;
		SuperDuration = kFindObjDuration;
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
		motor(-3, 2);
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
		// 		LOG_MESSAGE("obstacle value is " + obstacle(10, 10, 10), MODE_NORMAL);
		// }
	}
	else if (LoadedObjects >= 5)
	{
		LOG_MESSAGE("deposit", MODE_NORMAL);
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
		LOG_MESSAGE("Teleport");
		setAction(TELEPORT);
	}*/

	switch (getAction())
	{
	case DEFINED:
		break;
	case YELLOW_AVOIDANCE:
		break;
	case FIND_OBJ:
		if (Duration == kFindObjDuration || SuperDuration == kFindObjDuration)
		{
			LOG_MESSAGE("Add 1 to LoadedObjects", MODE_VERBOSE);
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
			ERROR_MESSAGE("this robot is not in object_box", MODE_NORMAL);
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
	case TO_DEPOSIT: // world 1 only
	default:
		ERROR_MESSAGE("action is " + to_string(static_cast<int>(getAction())), MODE_NORMAL);
		break;
	}
	LOG_MESSAGE("World1 Loop End", MODE_NORMAL);
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

void Game1_Hikaru::setup(void)
{
	system("cls");
	UserGame1::setup();
	InputDotInformation();
	InputColorInformation();
	system("chcp 65001");
	srand(static_cast<unsigned int>(time(NULL)));

	log_compass = Compass;
	if (PositionX == 0 && PositionY == 0)
	{
		int y = emergency_now_dot_id / kDotWidthNum;
		int x = emergency_now_dot_id - kDotWidthNum * y;
		log_y = y * kSize;
		log_x = x * kSize;
		now_dot_id = emergency_now_dot_id;
	}
	else
	{
		log_x = PositionX;
		log_y = PositionY;
	}

	setRunMode(MODE_MATCH);
}

void Game1_Hikaru::loop()
{
	ProcessingTime pt;
	pt.start();

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
		if (log_x >= kCospaceWidth)
		{
			log_x = kCospaceWidth - 1;
		}
		if (log_y < 0)
		{
			log_y = 0;
		}
		if (log_y >= kCospaceHeight)
		{
			log_y = kCospaceHeight - 1;
		}
		now_dot_id = CheckNowDot();
		int now_y = now_dot_id / kDotWidthNum;
		int now_x = now_dot_id - now_y * kDotWidthNum;
		int range = 0;
		rep(hi, range * 2 + 1)
		{
			rep(wj, range * 2 + 1)
			{
				int y = hi + now_y - range;
				int x = wj + now_x - range;
				if (x < 0 || x >= kDotWidthNum || y < 0 || y >= kDotHeightNum)
				{
					continue;
				}
				dot[y * kDotWidthNum + x].arrived_times++;
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
				int x = log_x / kSize + wi - range;
				int y = log_y / kSize + hj - range;
				if (x < 0 || x >= kDotWidthNum || y < 0 || y >= kDotHeightNum)
				{
					continue;
				}
				dot[y * kDotWidthNum + x].arrived_times += 2;
			}
		}
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
				min = static_cast<int>(pow(log_superobj_x[i] - log_x, 2) + pow(log_superobj_y[i] - log_y, 2));
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
				LOG_MESSAGE("There is no superobj", MODE_NORMAL);
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
	}

	switch (static_cast<int>(getAction()))
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
	}
	double seconds = pt.end();
	LOG_MESSAGE("loop time :" + to_string(seconds) + " ms", MODE_NORMAL);
}

long Game1_Hikaru::WhereIsMotor(void)
{
	//fprintf(logfile, "%4d Start WhereIsMotor()\n", getRepeatedNum());
	long x, y;
	if (log_x < 0 || log_x >= kCospaceWidth || log_y < 0 || log_y >= kCospaceHeight)
	{
		//fprintf(errfile, "%4d WhereIsMotor(): log_x, log_y = (%d, %d)\n", getRepeatedNum(), log_x, log_y);
		//fprintf(logfile, "%4d WhereIsMotor(): log_x, log_y = (%d, %d)\n", getRepeatedNum(), log_x, log_y);
		log_x = kCospaceWidth / 2;
		log_y = kCospaceHeight / 2;
	}
	x = log_x - static_cast<long>(cos((Compass + 90) * 3.14 / 180)) * 5;
	y = log_y - static_cast<long>(sin((Compass + 90) * 3.14 / 180)) * 5;
	//fprintf(logfile, "%4d End WhereIsMotor() with returning %ld * 1000 + %ld = %ld\n", getRepeatedNum(), y, x, y * 1000 + x);
	return y * 1000 + x;
}
long Game1_Hikaru::WhereIsColorSensor(void)
{
	//fprintf(logfile, "%4d Start WhereIsColorSensor()\n", getRepeatedNum());
	long x, y;
	if (log_x < 0 || log_x >= kCospaceWidth || log_y < 0 || log_y >= kCospaceHeight)
	{
		//fprintf(errfile, "%4d WhereIsColorSensor(): log_x, log_y = (%d, %d)\n", getRepeatedNum(), log_x, log_y);
		//fprintf(logfile, "%4d WhereIsColorSensor(): log_x, log_y = (%d, %d)\n", getRepeatedNum(), log_x, log_y);
		log_x = kCospaceWidth / 2;
		log_y = kCospaceHeight / 2;
	}
	x = log_x + static_cast<long>(cos((Compass + 90) * 3.14 / 180) * 5);
	y = log_y + static_cast<long>(sin((Compass + 90) * 3.14 / 180) * 5);
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
	//fprintf(logfile, "%4d End WhereIsColorSensor() with returning %ld * 1000 + %ld = %ld\n", getRepeatedNum(), y, x, y * 1000 + x);
	return y * 1000 + x;
}

int Game1_Hikaru::CheckNowDot(void)
{
	//fprintf(logfile, "%4d Start CheckNowDot()\n", getRepeatedNum());
	int x = WhereIsColorSensor();
	int y = x / 1000;
	x = x - y * 1000;
	//fprintf(logfile, "%4d CheckNowDot(): color sensor is (%d, %d)\n", getRepeatedNum(), x, y);
	if (x < 0 || x >= kCospaceWidth || y < 0 || y >= kCospaceHeight)
	{
		//fprintf(logfile, "%4d CheckNowDot(): (x, y) = (%d, %d)\n", getRepeatedNum(), x, y);
		//fprintf(errfile, "%4d CheckNowDot(): (x, y) = (%d, %d)\n", getRepeatedNum(), x, y);
		if (x < 0)
		{
			x = 10;
		}
		else
		{
			x = kCospaceWidth - 11;
		}
		if (y < 0)
		{
			y = 10;
		}
		else
		{
			y = kCospaceHeight - 11;
		}
	}

	x = x / kSize;
	y = y / kSize;

	//もし、今壁の中の場合
	if (dot[y * kDotWidthNum + x].point <= POINT_WALL)
	{
		// printf("%d CheckNowDot(): I am in wall or yellow\n", getRepeatedNum());
		//fprintf(logfile, " %d CheckNowDot(): I am in wall or yellow\n", getRepeatedNum());
		for (int i = 3; i <= 5 && dot[y * kDotWidthNum + x].point <= POINT_WALL; i = i + 2)
		{
			for (int j = 0; j < pow(i, 2); j++)
			{
				int temp_x, temp_y;
				temp_y = j / i;
				temp_x = j - temp_y * i;
				temp_x += x - i / 2;
				temp_y += y - i / 2;
				if (temp_x < 0 || temp_x >= kDotWidthNum)
				{
					continue;
				}
				if (temp_y < 0 || temp_y >= kDotHeightNum)
				{
					continue;
				}
				if (dot[temp_y * kDotWidthNum + temp_x].point >= POINT_DEPOSIT)
				{
					x = temp_x;
					y = temp_y;
					break;
				}
			}
		}
		if (dot[y * kDotWidthNum + x].point <= POINT_WALL)
		{
			LOG_MESSAGE("CheckNowDot(): I(" + to_string(x) + "," + to_string(y) + ") and around me are in wall or yellow", MODE_NORMAL);
			//fprintf(logfile, "%4d End CheckNowDot() with returning %d\n", getRepeatedNum(), emergency_now_dot_id);
			return emergency_now_dot_id;
		}
	}
	//fprintf(logfile, "%4d End CheckNowDot() with returning (%d, %d) = %d\n", getRepeatedNum(), x, y, y * kDotWidthNum + x);
	return y * kDotWidthNum + x;
}

int Game1_Hikaru::IsNearYellow(int num, int x, int y)
{
	//fprintf(logfile, "%4d Start IsNearYellow(%d, %d, %d)\n", getRepeatedNum(), num, x, y);
	if (num % 2 == 0)
	{
		num++;
	}
	if (x < 0)
	{
		y = now_dot_id / kDotWidthNum;
		x = now_dot_id - y * kDotWidthNum;
		if (now_dot_id < 0 || now_dot_id >= kMaxDotNum)
		{
			//fprintf(errfile, "%4d IsNearYellow(): now_dot_id is %d (%d, %d)\n", getRepeatedNum(), now_dot_id, x, y);
			//fprintf(logfile, "%4d IsNearYellow(): now_dot_id is %d (%d, %d)\n", getRepeatedNum(), now_dot_id, x, y);
			x = kDotWidthNum / 2;
			y = kDotHeightNum / 2;
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

		if (temp_x < 0 || temp_x >= kDotWidthNum || temp_y < 0 || temp_y >= kDotHeightNum)
		{
			continue;
		}

		int id = temp_y * kDotWidthNum + temp_x;
		if (dot[id].point == POINT_YELLOW)
		{
			//fprintf(logfile, "%4d End IsNearYellow() with returning 1\n", getRepeatedNum());
			return 1;
		}
	}
	//fprintf(logfile, "%4d End IsNearYellow() with returning 0\n", getRepeatedNum());
	return 0;
}

int Game1_Hikaru::GoToPosition(int x, int y, int wide_decide_x, int wide_decide_y, int wide_judge_arrived)
{
	LOG_MESSAGE(FUNCNAME + "(" + to_string(x) + ", " + to_string(y) + ", " + to_string(wide_decide_x) + ", " + to_string(wide_decide_y) + ", " + to_string(wide_judge_arrived) + "): start", MODE_DEBUG);
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
		return 0;
	}

	//absolute_x,yの値が、x, yの値からずれている場合
	if (absolute_x == -1 || !(PLUSMINUS(absolute_x, x, wide_decide_x) && PLUSMINUS(absolute_y, y, wide_decide_y)))
	{
		int i = 0;
		do
		{
			if (i > 20)
			{
				absolute_x = x;
				absolute_y = y;
				ERROR_MESSAGE("warming GoToPosition(): absolute_x, absolute_yが決まりません; (" + to_string(x) + ", " + to_string(y) + ", " + to_string(wide_decide_x) + ", " + to_string(wide_decide_y) + ", " + to_string(wide_judge_arrived) + ")", MODE_NORMAL);
				break;
			}
			absolute_x = x - wide_decide_x + (rnd() + 1) % (wide_decide_x * 2 + 1);
			absolute_y = y - wide_decide_y + (rnd() + 1) % (wide_decide_y * 2 + 1);
			i++;
		} while (absolute_x < 10 || absolute_x > kCospaceWidth - 10 || absolute_y < 10 || absolute_y > kCospaceHeight - 10);
		//same_operate = 0;
	}

	if (absolute_distance <= -1)
	{
		absolute_distance = static_cast<int>(sqrt(pow(absolute_x - log_x, 2) + pow(absolute_y - log_y, 2))) + 40;
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
		LOG_MESSAGE("(" + to_string(absolute_x) + "," + to_string(absolute_y) + ")に到着しました", MODE_NORMAL);
		absolute_x = -1;
		absolute_y = -1;
		same_operate = -1;
		absolute_distance = -1;
		return 1;
	}

	LOG_MESSAGE("ab(" + to_string(absolute_x) + "," + to_string(absolute_y) + ")", MODE_NORMAL);
	x = absolute_x;
	y = absolute_y;
	x = x - temp_x;
	y = y - temp_y;
	LOG_MESSAGE("x, y = " + to_string(x) + ", " + to_string(y), MODE_NORMAL);
	double angle = atan2(y, x);
	angle = angle * 180 / 3.14;
	int angle_int = static_cast<int>(angle);
	angle_int -= 90;
	if (angle_int < 0)
	{
		angle_int += 360;
	}
	LOG_MESSAGE("angle " + to_string(angle_int), MODE_NORMAL);
	GoToAngle(angle_int, static_cast<int>(sqrt(x * x + y * y)));

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

void Game1_Hikaru::InputDotInformation(void)
{
	// rep(hi, kDotHeightNum)
	// {
	// 	rep(wj, kDotWidthNum)
	// 	{
	// 		cout << map_output_data[hi][wj];
	// 	}
	// 	cout << endl;
	// }
	// cout << endl;
	int map_position_color_data[kDotWidthNum][kDotHeightNum];
	for (int i = 0; i < kDotWidthNum; i++)
	{
		for (int j = 0; j < kDotHeightNum; j++)
		{
			switch (map_output_data[kDotHeightNum - j - 1][i])
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

	for (long i = 0; i < kMaxDotNum; i++)
	{
		//I use id of dot
		//id = y * 36(= 360 / kSize) + x;
		//x and y are 360 / kSize and 270 / kSize
		//kSize may be 10

		int x, y;
		//kDotWidthNum = 360 / kSize
		y = i / kDotWidthNum;
		x = i - y * kDotWidthNum;

		dot[i].id = i;
		//x position. if x = 0, center of dot is 0 * 10(=kSize) + 5(=kSize / 2)
		dot[i].x = x * kSize + kSize / 2;
		dot[i].y = y * kSize + kSize / 2;
		//the wide of dot
		dot[i].wide = kSize;
		//printf("(%d, %d, %d, %d)\n", x, y, dot[i].x, dot[i].y);

		//point means what's this dot belongs?
		//For example, this dot belongs yellow.
		//this map_position_color_data is defined at 60 lines upper(may be)
		dot[i].point = map_position_color_data[x][y];
		// dot[i].point = 1;
		dot[i].color = map_position_color_data[x][y];
		dot[i].red = red_data[kDotHeightNum - y - 1][x];
		// if (dot[i].red == 0 && dot[i].color == POINT_WHITE)
		// {
		// 	dot[i].cyan = 1;
		// 	dot[i].black = 1;
		// }
		dot[i].cyan = cyan_data[kDotHeightNum - y - 1][x];
		dot[i].black = black_data[kDotHeightNum - y - 1][x];

		//these are for dijkstra
		// dot[i].done = -1;
		// dot[i].from = -1;
		// dot[i].cost = -1;
	}

	//set values of cost
	for (long i = 0; i < kMaxDotNum; i++)
	{
		int y = i / kDotWidthNum;
		int x = i - y * kDotWidthNum;
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
			int target_id = temp_y * kDotWidthNum + temp_x;
			if (temp_x < 0 || temp_x >= kDotWidthNum || temp_y < 0 || temp_y >= kDotHeightNum)
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
			else if (x == 0 || y == 0 || x == kDotWidthNum - 1 || y == kDotHeightNum - 1 || temp_x == 0 || temp_y == 0 || temp_x == kDotWidthNum - 1 || temp_y == kDotHeightNum - 1)
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
				dot[i].edge_cost[dot[i].edge_num] = static_cast<int>(dot[i].edge_cost[dot[i].edge_num] * 1.4);
			}
			dot[i].edge_num++;
		}
	}
	// for (int hi = kDotHeightNum - 1; hi >= 0; hi--)
	// {
	// 	rep(wj, kDotWidthNum)
	// 	{
	// 		switch (dot[hi * kDotWidthNum + wj].point)
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
	// 			if (dot[hi * kDotWidthNum + wj].black == 1)
	// 			{
	// 				cout << "B";
	// 			}
	// 			else if (dot[hi * kDotWidthNum + wj].cyan == 1)
	// 			{
	// 				cout << "C";
	// 			}
	// 			else if (dot[hi * kDotWidthNum + wj].red == 1)
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

void Game1_Hikaru::Dijkstra()
{
	//fprintf(logfile, " %d Start Dijkstra()\n", getRepeatedNum());
	for (int i = 0; i < kMaxDotNum; i++)
	{
		dot[i].id = i;
		dot[i].cost = -1;
		// dot[i].distance_from_start = -1;
		dot[i].done = -1;
		dot[i].from = -1;
	}

	int now_node_id = now_dot_id;

	if (now_node_id < 0 || now_node_id >= kMaxDotNum)
	{
		ERROR_MESSAGE(FUNC_NAME + "(); now dot id value is " + to_string(now_dot_id), MODE_NORMAL);
		return;
	}
	dot[now_node_id].cost = 0;
	dot[now_node_id].from = now_node_id;

	struct Dot investigating_node;

	// int number = 0;
	while (true)
	{
		// number++;
		//investigating_nodeを初期化
		investigating_node.done = 0;
		for (int i = 0; i < kMaxDotNum; i++)
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

		for (int i = 0; i < investigating_node.edge_num; i++)
		{
			int target_id = investigating_node.edge_to[i];
			if (target_id < 0 || target_id >= kMaxDotNum)
			{
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
				target_cost += static_cast<int>(dot[target_id].arrived_times * 10);
			}

			// if (dot[target_id].point < -1) {
			// 	// if (option == 0) {
			// 		continue;
			// 	// }
			// }
			double k = 0.5;
			if (searching_object == BLACK_LOADED_ID && dot[investigating_node.id].black == 1)
			{
				target_cost = static_cast<int>(k * target_cost);
			}
			if (searching_object == CYAN_LOADED_ID && dot[investigating_node.id].cyan == 1)
			{
				target_cost = static_cast<int>(k * target_cost);
			}
			if (searching_object == RED_LOADED_ID && dot[investigating_node.id].red == 1)
			{
				target_cost = static_cast<int>(k * target_cost);
			}

			if (target_cost <= 0)
			{
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
}

int Game1_Hikaru::GoToDot(int x, int y)
{
	// printf("%d %d\n", x * kSize, y * kSize);
	static int prev_x = -1, prev_y = -1, prev_now_dot_id = -1;

	//fprintf(logfile, " %d Start GoToDot(%d, %d)\n", getRepeatedNum(), x, y);
	if (PositionX == -1 && (PLUSMINUS(log_x, x * kSize, kSize) && PLUSMINUS(log_y, y * kSize, kSize)))
	{
		//fprintf(logfile, " %d End GoToDot() with returning 1 because I am in PLA and it's near target(%d, %d)\n", getRepeatedNum(), x, y);
		LOG_MESSAGE(FUNC_NAME + "() end returning 1 because I am in PLA and it's near target(" + to_string(x) + ", " + to_string(y) + ")", MODE_NORMAL);
		GoToPosition(x, y, 10, 10, 5);
		return 1;
	}
	// char map_data_to_show[kMaxDotNum];
	// for (int i = 0; i < kMaxDotNum; i++)
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
	if (x < 1 || x >= kDotWidthNum - 1 || y < 1 || y >= kDotHeightNum - 1)
	{
		LOG_MESSAGE(FUNC_NAME + "(): (x, y) is (" + to_string(x) + ", " + to_string(y) + "and strange", MODE_NORMAL);
	}

	if (prev_now_dot_id != now_dot_id || prev_x != x || prev_y != y)
	{
		Dijkstra();
	}
	prev_now_dot_id = now_dot_id;
	prev_x = x;
	prev_y = y;

	// printf("from %d %d to %d %d\n", now_dot_id - (int)(now_dot_id / kDotWidthNum) * kDotWidthNum, now_dot_id / kDotWidthNum, x, y);

	int goal_dot = y * kDotWidthNum + x;

	if (goal_dot < 0 || goal_dot >= kMaxDotNum)
	{
		LOG_MESSAGE("strainge(x, y)", MODE_NORMAL);
		return 0;
	}

	int temp = goal_dot;
	// map_data_to_show[goal_dot] = 'T';
	int i = 0;

	while (dot[temp].from != now_dot_id && i < 200)
	{
		// int go_x, go_y;
		// go_y = temp / kDotWidthNum;
		// go_x = temp - (int)go_y * kDotWidthNum;
		temp = dot[temp].from;
		// map_data_to_show[temp] = '#';
		// printf("%d\n", dot[temp].point);
		i++;
		if (temp < 0 || temp >= kMaxDotNum)
		{
			LOG_MESSAGE("temp = " + to_string(temp) + "is strange. I will continue", MODE_NORMAL);
			GoToPosition(x * kSize, y * kSize, 5, 5, 5);
			return 0;
		}
	}
	if (i == 200)
	{
		printf("\n\n\niの値が200ですByGoToNode()\n\n\n\n");
		LOG_MESSAGE(FUNC_NAME + "(): iの値が200です", MODE_NORMAL);
	}

	// map_data_to_show[now_dot_id] = '@';

	int next_x, next_y;
	next_y = temp / kDotWidthNum;
	next_x = temp - next_y * kDotWidthNum;

	int now_y = now_dot_id / kDotWidthNum;
	int now_x = now_dot_id - now_y * kDotWidthNum;

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
	return 0;
}

int Game1_Hikaru::GoToDots(int x, int y, int wide_decide_x, int wide_decide_y)
{
	//fprintf(logfile, " %d Start GoToDots(%d, %d, %d, %d)\n", getRepeatedNum(), x, y, wide_decide_x, wide_decide_y);
	// printf("GoToDots(): %d %d %d %d\n", x, y, wide_decide_x, wide_decide_y);

	static int prev_x = -1;
	static int prev_y = -1;
	static int target_x = -1;
	static int target_y = -1;
	static int local_same_target = 0;
	static int same_target_border = 0;
	if (x != prev_x || y != prev_y)
	{
		LOG_MESSAGE("changed dots", MODE_NORMAL);
		local_same_target = 0;
		prev_x = x;
		prev_y = y;
		//0:left bottom corner 1:right bottom corner 2:right bottom corner
		int corner_x[2], corner_y[2];
		corner_x[0] = (x - wide_decide_x) / kSize;
		corner_y[0] = (y - wide_decide_y) / kSize;
		corner_x[1] = (x + wide_decide_x) / kSize;
		corner_y[1] = (y + wide_decide_y) / kSize;

		for (int i = 0; i < 2; i++)
		{
			if (corner_x[i] < 0)
			{
				//fprintf(errfile, " %d GoToDots() corner_x[%d] is %d < 0\n", getRepeatedNum(), i, corner_x[i]);
				//fprintf(logfile, " %d GoToDots() corner_x[%d] is %d < 0\n", getRepeatedNum(), i, corner_x[i]);
				corner_x[i] = 0;
			}
			if (corner_x[i] >= kDotWidthNum)
			{
				//fprintf(errfile, " %d GoToDots() corner_x[%d] is %d >= %d\n", getRepeatedNum(), i, corner_x[i], kDotWidthNum);
				//fprintf(logfile, " %d GoToDots() corner_x[%d] is %d >= %d\n", getRepeatedNum(), i, corner_x[i], kDotWidthNum);
				corner_x[i] = kDotWidthNum - 1;
			}
			if (corner_y[i] < 0)
			{
				//fprintf(errfile, " %d GoToDots() corner_y[%d] is %d < 0\n", getRepeatedNum(), i, corner_y[i]);
				//fprintf(logfile, " %d GoToDots() corner_y[%d] is %d < 0\n", getRepeatedNum(), i, corner_y[i]);
				corner_y[i] = 0;
			}
			if (corner_y[i] >= kDotHeightNum)
			{
				//fprintf(errfile, " %d GoToDots() corner_y[%d] is %d >= %d\n", getRepeatedNum(), i, corner_y[i], kDotHeightNum);
				//fprintf(logfile, " %d GoToDots() corner_y[%d] is %d >= %d\n", getRepeatedNum(), i, corner_y[i], kDotHeightNum);
				corner_y[i] = kDotHeightNum - 1;
			}
		}

		int min = 100000, id = -1;
		// n回に1回移動する
		int option = rnd() % 5;
		for (int i = corner_x[0]; i <= corner_x[1]; i++)
		{
			for (int j = corner_y[0]; j <= corner_y[1]; j++)
			{
				int investigating_dot_id = j * kDotWidthNum + i;
				if (i <= 0 || i >= kDotWidthNum - 1 || j <= 0 || j >= kDotHeightNum - 1)
				{
					continue;
				}

				//yellow or wall or deposit
				if (dot[investigating_dot_id].point < POINT_DEPOSIT)
				{
					continue;
				}

				int costs = static_cast<int>(dot[investigating_dot_id].arrived_times * 100 + rand() % 10);
				if (option)
				{
					// 移動しないとき
					int k = 20;
					costs += static_cast<int>(pow(abs(i * kSize - log_x) - k, 2) * 100 - pow(abs(j * kSize - log_y) - k, 2) * 100);
				}
				else
				{
					// 移動するとき
					costs -= static_cast<int>(pow(i * kSize - log_x, 2) / 100 - pow(j * kSize - log_y, 2) / 100);
				}
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
			target_x = x / kSize;
			target_y = y / kSize;
		}
		else
		{
			target_y = id / kDotWidthNum;
			target_x = id - target_y * kDotWidthNum;
			//fprintf(logfile, " %d decide target as (%d, %d)\n", getRepeatedNum(), target_x, target_y);
		}

		same_target_border = static_cast<int>(sqrt(pow(log_x - target_x * kSize, 2) + pow(log_y - target_y * kSize, 2)));
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
		// 		target_x /= kSize;
		// 		target_y /= kSize;
		// 		break;
		// 	}
		// 	target_x = x - wide_decide_x + rand() % (wide_decide_x * 2 + 1);
		// 	target_y = y - wide_decide_y + rand() % (wide_decide_y * 2 + 1);
		// 	target_x /= kSize;
		// 	target_y /= kSize;
		// 	if(target_x <= 0) {
		// 		target_x = 1;
		// 	}
		// 	if(target_x >= kDotWidthNum - 1) {
		// 		target_x = kDotWidthNum - 2;
		// 	}
		// 	if(target_y <= 0) {
		// 		target_y = 1;
		// 	}
		// 	if(target_y >= kDotHeightNum - 1) {
		// 		target_y = kDotHeightNum - 2;
		// 	}
		// } while(dot[target_y * kDotWidthNum + target_x].point <= POINT_WALL);
	}
	local_same_target++;
	// printf("%d %d\n", local_same_target, same_target_border);
	if (GoToDot(target_x, target_y) || local_same_target > same_target_border)
	{
		prev_x = -1;
		local_same_target = 0;
		//fprintf(logfile, " %d End GoToDots() with returning 1\n", getRepeatedNum());
		return 1;
	}
	else
	{
		//fprintf(logfile, " %d End GoToDots() with returning 0\n", getRepeatedNum());
		return 0;
	}
}

int Game1_Hikaru::GoInDots(int x, int y, int wide_decide_x, int wide_decide_y, int color)
{
	//fprintf(logfile, " %d Start GoToDots(%d, %d, %d, %d)\n", getRepeatedNum(), x, y, wide_decide_x, wide_decide_y);
	// printf("GoToDots(): %d %d %d %d\n", x, y, wide_decide_x, wide_decide_y);
	static int prev_x = -1;
	static int prev_y = -1;
	static int prev_color = -1000;
	static int target_x = -1;
	static int target_y = -1;
	static int local_same_target = 0;
	static int same_target_border = 0;
	if (x != prev_x || y != prev_y || color != prev_color)
	{
		LOG_MESSAGE("changed dots", MODE_NORMAL);
		local_same_target = 0;
		prev_x = x;
		prev_y = y;
		//0:left bottom corner 1:right bottom corner 2:right bottom corner
		int corner_x[2], corner_y[2];
		corner_x[0] = (x - wide_decide_x) / kSize;
		corner_y[0] = (y - wide_decide_y) / kSize;
		corner_x[1] = (x + wide_decide_x) / kSize;
		corner_y[1] = (y + wide_decide_y) / kSize;

		for (int i = 0; i < 2; i++)
		{
			if (corner_x[i] < 0)
			{
				//fprintf(errfile, " %d GoToDots() corner_x[%d] is %d < 0\n", getRepeatedNum(), i, corner_x[i]);
				//fprintf(logfile, " %d GoToDots() corner_x[%d] is %d < 0\n", getRepeatedNum(), i, corner_x[i]);
				corner_x[i] = 0;
			}
			if (corner_x[i] >= kDotWidthNum)
			{
				//fprintf(errfile, " %d GoToDots() corner_x[%d] is %d >= %d\n", getRepeatedNum(), i, corner_x[i], kDotWidthNum);
				//fprintf(logfile, " %d GoToDots() corner_x[%d] is %d >= %d\n", getRepeatedNum(), i, corner_x[i], kDotWidthNum);
				corner_x[i] = kDotWidthNum - 1;
			}
			if (corner_y[i] < 0)
			{
				//fprintf(errfile, " %d GoToDots() corner_y[%d] is %d < 0\n", getRepeatedNum(), i, corner_y[i]);
				//fprintf(logfile, " %d GoToDots() corner_y[%d] is %d < 0\n", getRepeatedNum(), i, corner_y[i]);
				corner_y[i] = 0;
			}
			if (corner_y[i] >= kDotHeightNum)
			{
				//fprintf(errfile, " %d GoToDots() corner_y[%d] is %d >= %d\n", getRepeatedNum(), i, corner_y[i], kDotHeightNum);
				//fprintf(logfile, " %d GoToDots() corner_y[%d] is %d >= %d\n", getRepeatedNum(), i, corner_y[i], kDotHeightNum);
				corner_y[i] = kDotHeightNum - 1;
			}
		}

		int min = 100000, id = -1;
		// n回に1回移動する
		int option = rnd() % 3;
		for (int i = corner_x[0]; i <= corner_x[1]; i++)
		{
			for (int j = corner_y[0]; j <= corner_y[1]; j++)
			{
				int investigating_dot_id = j * kDotWidthNum + i;
				if (i <= 0 || i >= kDotWidthNum - 1 || j <= 0 || j >= kDotHeightNum - 1)
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

				int costs = static_cast<int>(dot[investigating_dot_id].arrived_times * 100 + rnd() % 10);

				if (option)
				{
					// 移動しないとき
					int k = 20;
					costs += static_cast<int>(pow(abs(i * kSize - log_x) - k, 2) / 10 + pow(abs(j * kSize - log_y) - k, 2) / 10);
				}
				else
				{
					// 移動するとき
					costs -= static_cast<int>(pow(i * kSize - log_x, 2) / 100 - pow(j * kSize - log_y, 2) / 100);
				}
				if (color == POINT_DEPOSIT)
				{
					costs = static_cast<int>(pow(i * kSize - log_x, 2) + pow(j * kSize - log_y, 2));
				}
				// cout << "position cost " << pow(i * kSize - log_x, 2) / 100 + pow(j * kSize - log_y, 2) / 100 << " arrived cost " << dot[investigating_dot_id].arrived_times * 100 << endl;
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
			//fprintf(errfile, "%d GoInDots(): There is no dot that can go\n", getRepeatedNum());
			//fprintf(logfile, " %d GoToDots(): There is no dot that can go\n", getRepeatedNum());
			//fprintf(stdout, "%d GoInDots(): There is no dot that can go\n", getRepeatedNum());
			target_x = x / kSize;
			target_y = y / kSize;
		}
		else
		{
			target_y = id / kDotWidthNum;
			target_x = id - target_y * kDotWidthNum;
			LOG_MESSAGE("target(" + to_string(target_x) + ", " + to_string(target_y), MODE_NORMAL);
			//fprintf(logfile, " %d decide target as (%d, %d)\n", getRepeatedNum(), target_x, target_y);
		}

		same_target_border = static_cast<int>(sqrt(pow(log_x - target_x * kSize, 2) + pow(log_y - target_y * kSize, 2)));
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
		// 		target_x /= kSize;
		// 		target_y /= kSize;
		// 		break;
		// 	}
		// 	target_x = x - wide_decide_x + rand() % (wide_decide_x * 2 + 1);
		// 	target_y = y - wide_decide_y + rand() % (wide_decide_y * 2 + 1);
		// 	target_x /= kSize;
		// 	target_y /= kSize;
		// 	if(target_x <= 0) {
		// 		target_x = 1;
		// 	}
		// 	if(target_x >= kDotWidthNum - 1) {
		// 		target_x = kDotWidthNum - 2;
		// 	}
		// 	if(target_y <= 0) {
		// 		target_y = 1;
		// 	}
		// 	if(target_y >= kDotHeightNum - 1) {
		// 		target_y = kDotHeightNum - 2;
		// 	}
		// } while(dot[target_y * kDotWidthNum + target_x].point <= POINT_WALL);
	}

	prev_x = x;
	prev_y = y;
	prev_color = color;

	local_same_target++;
	// printf("%d\n", local_same_target);
	// printf("%d %d\n", local_same_target, same_target_border);
	LOG_MESSAGE("target_x, y " + to_string(target_x * kSize) + " " + to_string(target_y * kSize), MODE_NORMAL);
	if (GoToDot(target_x, target_y) || local_same_target > same_target_border)
	{
		prev_x = -1;
		local_same_target = 0;
		//fprintf(logfile, " %d End GoToDots() with returning 1\n", getRepeatedNum());
		return 1;
	}
	else
	{
		//fprintf(logfile, " %d End GoToDots() with returning 0\n", getRepeatedNum());
		return 0;
	}
}

int Game1_Hikaru::HowManyCurved(int id)
{
	/*
    道の長さ * 10 + 曲がった回数 * 20 + (Object < 6 のとき) Objectのとれる試算
    */
	int route[kMaxDotNum];
	//曲がった回数
	int curved_times = 0;
	//道の長さ
	int distance_way = -1;
	route[0] = id;
	// printf("id is %d now is %d \n", id, now_dot_id);
	for (int i = 1; i < kMaxDotNum; i++)
	{
		if (route[i - 1] < 0 || route[i - 1] > kMaxDotNum)
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
		if (route[i] >= kMaxDotNum || route[i] < 0)
		{
			//fprintf(errfile, "%d HowManyCurved(): route[%d]の値が%dでおかしい\n", getRepeatedNum(), i, route[i]);
			//fprintf(logfile, "%d HowManyCurved(): route[%d]の値が%dでおかしい\n", getRepeatedNum(), i, route[i]);
			distance_way = i;
			break;
		}
	}
	// printf("distance_way = %d\n", distance_way);
	int x[kMaxDotNum], y[kMaxDotNum], direction[kMaxDotNum];
	//directionは、左上=0で、右に行くごとに+1、下に行くごとに+3される
	if (distance_way >= kMaxDotNum)
	{
		//fprintf(logfile, " %d Warming HowManyCurved(): routeの要素数が%dで kMaxDotNum を超えている\n", getRepeatedNum(), distance_way);
		//fprintf(errfile, "%d Warming HowManyCurved(): routeの要素数が%dで kMaxDotNum を超えている\n", getRepeatedNum(), distance_way);
		distance_way = kMaxDotNum - 2;
	}
	y[0] = route[0] / kDotWidthNum;
	x[0] = route[0] - y[0] * kDotWidthNum;
	direction[0] = -1;
	for (int i = 1; i < distance_way; i++)
	{
		y[i] = route[i] / kDotWidthNum;
		x[i] = route[i] - y[i] * kDotWidthNum;
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

void Game1_Hikaru::GoToAngle(int angle, int distance)
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

	int classification = obstacle(10, 12, 10);
	if (log_superobj_num > 0)
	{
		classification = obstacle(5, 7, 5);
	}

	int big_motor = 5;
	int short_motor = 3;
	if (IsNearYellow(2, -1, -1))
	{
		big_motor = 4;
		short_motor = 2;
	}
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
			else if (IsNearYellow(2, -1, -1) && LoadedObjects != 0)
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
						motor(2, -3);
					}
					else
					{
						motor(-3, 2);
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
						motor(2, -3);
					}
					else
					{
						motor(-3, 2);
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
}
