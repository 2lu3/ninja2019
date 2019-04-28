#include "Masuda.hpp"
// #include "MapData.hpp"
#include <iostream>

#define FOR for
#define rep(i, n) FOR(long i = 0; i < n; i++)

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
#define SIZE 5
#define DOT_WIDTH_NUMBER (COSPACE_WIDTH / SIZE)
#define DOT_HEIGHT_NUMBER (COSPACE_HEIGHT / SIZE)
#define MAX_DOT_NUMBER (COSPACE_WIDTH * COSPACE_HEIGHT / SIZE / SIZE)
#define MAX_EDGE_NUMBER 25
#define BORDER_SAME_OBJECT_NUMBER 2
#define FIND_OBJECT_DURATION 46
#define PLUSMINUS(a, b, c) ((a) <= (b) + (c) && (a) >= (b) - (c))

#define FIND_OBJ_DURATION 46

using namespace std;

int go_to_up=0;
bool go_to_deposit=false;
bool ikkaime =true;
bool hukki = false;

void GoToAngle2(int angle, int distance);

int process_world1;


const int border_same_obj_number = 3;

void Game0_Masuda::setup(void)
{
	ProcessingTime pt;
	pt.start();

	UserGame0::setup();

	delErrorFile();
	delLogFile();
	InputColorInformation();
	setRunMode(MODE_DEBUG);

	double seconds = pt.end();
	logMessage("game0 setup() : " + to_string(seconds) + " milliseconds");
}

void Game0_Masuda::loop(void)
{
	UserGame0::loop();
	logMessage("World1 loop start");
	if (SuperDuration > 0)
	{
		SuperDuration--;
	}
	else if (hukki && Duration == 0 ) {
		Duration = 30;
		motor(-3,-3);
		hukki = false;
		// if (go_to_up) {
		// 	if (Duration < 60) {
		// 		motor(3,3);
		// 		go_to_up = 0;
		// 		hukki=false;
		// 	}else{
		// 		motor(-3,3);;
		// 	}
		// }else{
		// 	if (Duration < 60) {
		// 		motor(3,3);
		// 		go_to_up = 0;
		// 		hukki=false;
		// 	}else{
		// 		motor(-3,3);
		// 	}
		// }

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
			go_to_deposit = false;
			ikkaime = true;
			hukki = true;
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
	else if (EitherColorJudge(cyan_obj) && loaded_objects[CYAN_LOADED_ID] < border_same_obj_number && LoadedObjects < 6 && (loaded_objects[CYAN_LOADED_ID] + loaded_objects[RED_LOADED_ID] < 5))
	{
		logMessage("find cyan obj");
		setAction(FIND_OBJ);
		loaded_objects[CYAN_LOADED_ID]++;
		SuperDuration = FIND_OBJ_DURATION;
	}
	else if (EitherColorJudge(red_obj) && loaded_objects[RED_LOADED_ID] < border_same_obj_number && LoadedObjects < 6 && (loaded_objects[CYAN_LOADED_ID] + loaded_objects[RED_LOADED_ID] < 5))
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
	else if (IsOnYellowLine() )//(IsOnYellowLine() && LoadedObjects > 0)
	{
		if (LoadedObjects<4) {
			motor(-3,-5	);
			Duration = 25;
			setAction(YELLOW_AVOIDANCE);
		}else{
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
	else if ((LoadedObjects >=5 && IsOnBlueFloor() )|| go_to_deposit) {
		setAction(TO_DEPOSIT);
		Duration = 113;
		// std::cout << "deponiikuifnonaka" << '\n';
		// if (Compass < 90 || Compass >270) {
		// 	go_to_up=1;
		// }
		// if (go_to_up) {
		// 	if (ikkaime) {
		// 		Duration = 10;
		// 		motor(3,3);
		// 		ikkaime = false;
		// 		go_to_deposit = true;
		// 		std::cout << "ikkaimeue" << '\n';
		// 	}else{
		// 		if (Compass<280 && Compass>260) {
		// 			motor(3,3);
		// 		}else if (Compass<270 && Compass>91) {
		// 			motor(3,-3);
		// 		}else{
		// 			motor(-3,3);
		// 		}
		// 		std::cout << "llllllll" << '\n';
		// 	}
		// }else{
		// 	if (ikkaime) {
		// 		Duration = 10;
		// 		motor_no_action_change(3,3);
		// 		ikkaime = false;
		// 		go_to_deposit = true;
		// 		std::cout << "ikkaimesita" << '\n';
		// 	}else{
		// 		std::cout << "rrrrrrr" << '\n';
		// 		if (Compass<100 && Compass>80) {
		// 			motor(3,3);
		// 		}else if (Compass<90 && Compass>270) {
		// 			motor(3,-3);
		// 		}else{
		// 			motor(-3,3);
		// 		}
		// 	}
		// }
	}
	else if (LoadedObjects >= 5)
	{
		cout << "deposit" << endl;
		if (LoadedObjects >= 6) {
				if (US_Front < 14) //突当りで曲がるよ
				{
					motor(-3, 1);
				}
				else if (US_Left < 8)//なんでか左が反応してる、、、元に戻ろう
				{
					motor(-3, -1);
				}
				else if (US_Right < 8)//
				{
					motor(-3, -1);
				}
				else if (US_Right < 15)
				{
					motor(1, 4);
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
					motor(5, 1);
				}
		}else{
				if (US_Front < (14 + rand()%20)) //突当りで曲がるよ
				{
					motor(-3, 1);
				}
				else if (US_Left < 8)//なんでか左が反応してる、、、元に戻ろう
				{
					motor(-3, -1);
				}
				else if (US_Right < 8)//
				{
					motor(-3, -1);
				}
				else if (US_Right < 15)
				{
					motor(3, 4);
				}
				else if (US_Right < (80 - rand()%55))
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
	}else{
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
				motor(4, 5);
			}
			else if (US_Right < 40 + rand() % 30)
			{
				motor(4, 3);
			}
			else if (US_Right < 80)
			{
				motor(3, 1);
			}
			else
			{
				motor(4, 2);
			}
		// motor(3, 3);
	}

	/*if (Time > 180 && getAction() != FIND_OBJ && getAction() != DEPOSIT_OBJ && (!EitherColorJudge(object_box) && LoadedObjects < 3) || Time > 200)
	{
		logMessage("Teleport");
		cout << "TELEPORT" << endl;
		setAction(TELEPORT);
	}*/
	if (go_to_deposit) {
		std::cout << "depodepo~~~~" << '\n';
	}
	if (Time > 180)
	{
		cout << "TELEPORT" << endl;
		setAction(TELEPORT);
	}

	if(getAction() != TO_DEPOSIT) {
		process_world1 = 0;
	}


	switch (getAction())
	{
	case DEFINED:
		break;
	case TO_DEPOSIT:
		if(Duration>= 100) {
			motor_no_action_change(3, 3);
		}
		else if(process_world1 == 0) { // 左むく
			if(Compass > 85 && Compass < 95) {
				if(US_Front < 30) {
					process_world1 = 1;
				}
				else {
					process_world1 = 3;
				}
			}
			else {
				if(Compass >90 && Compass < 270) {
					motor_no_action_change(1, -2);
				}
				else {
					motor_no_action_change(-2, 1);
				}
			}
		}
		else if(process_world1 == 1) {//右むく
			if(Compass > 260 && Compass < 280) {
				process_world1 = 2;
			}
			else {
				if(Compass >90 && Compass < 270) {
					motor_no_action_change(-1, 1);
				}
				else {
					motor_no_action_change(1, -1);
				}
			}
		}
		else if(process_world1 == 2) {
			if(Compass >90 && Compass < 270) {
				motor_no_action_change(2, 4);
			}
			else {
				motor_no_action_change(4, 2);
			}
		}
		else if(process_world1 == 3) {
			if(Compass >90 && Compass < 270) {
				motor_no_action_change(4, 2);
			}
			else {
				motor_no_action_change(2, 4);
			}
		}
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
		Duration = 0;
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

int Game0_Masuda::shouldTeleport(void)
{
	return UserGame0::shouldTeleport();
}
void Game0_Masuda::taskOnTeleport(void)
{
	UserGame0::taskOnTeleport();
	Teleport = 3;
}
void GoToAngle2(int angle, int distance)
{

	angle = angle - Compass;

	//180より大きい場合
	for (int i = 0; i < 10 && angle > 180; i++)
	{
		if (i == 9)
		{
			printf("GoToAngle2(): angle's value:%d is abnormality\n", angle);
		}
		angle -= 360;
	}
	//-180より小さい場合
	for (int i = 0; i < 10 && angle < -180; i++)
	{
		if (i == 9)
		{
			printf("GoToAngle2(): angle's value:%d is abnormality\n", angle);
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
						motor(4, -4);
					}
					else
					{
						motor(-4, 4);
					}
					// Duration = 3;
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
