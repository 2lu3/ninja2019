#include "Hikaru.h"

#define FIND_OBJ_DURATION 45
#define DEPOSIT_OBJ_DURATION 45

#define REP for
#define rep(i, n) REP(int i = 0; i < n; i++)

int subject[6] = {0, 0, 0, 0, 0, 0};

int triger_object_num = 5;

int triger(void)
{
	return !((LoadedObjects >= triger_object_num) || (Time > 180 && LoadedObjects >= 3));
}

void localGame0Setup(void)
{
	// shift-JIS 932
	// UTF-8 65001
	system("chcp 65001");
	init();
	init2();
}

void localGame0(void)
{
	rep(i, 3)
	{
		loaded_objects[i] = 2;
	}
	calculate2(US_Left, US_Front, US_Right, Compass + 90);
	showMap2();
	if (IsOnTrapBlue())
	{
		for (int i = 0; i < sizeof(loaded_objects) / sizeof(loaded_objects[0]); i++)
		{
			loaded_objects[i] = 0;
		}
		LoadedObjects = 0;
	}
	if (SuperDuration > 0)
	{
		SuperDuration--;
	}
	else if (IsOnCyanObj() && loaded_objects[CYAN_LOADED_ID] < 2 && LoadedObjects < 6)
	{
		setAction(FIND_OBJ);
		loaded_objects[CYAN_LOADED_ID]++;
		LoadedObjects++;
		SuperDuration = FIND_OBJ_DURATION;
	}
	else if (IsOnRedObj() && loaded_objects[RED_LOADED_ID] < 2 && LoadedObjects < 6)
	{
		setAction(FIND_OBJ);
		loaded_objects[RED_LOADED_ID]++;
		LoadedObjects++;
		SuperDuration = FIND_OBJ_DURATION;
	}
	else if (IsOnBlackObj() && loaded_objects[BLACK_LOADED_ID] < 2 && LoadedObjects < 6)
	{
		setAction(FIND_OBJ);
		loaded_objects[BLACK_LOADED_ID]++;
		LoadedObjects++;
		SuperDuration = FIND_OBJ_DURATION;
	}
	else if (IsOnDepositArea() && LoadedObjects >= 6)
	{
		switch (IsOnDepositArea())
		{
		case 1:
			motor(0, 4);
			break;
		case 2:
			motor(4, 0);
			break;
		case 3:
			motor(0, 0);
			setAction(DEPOSIT_OBJ);
			SuperDuration = DEPOSIT_OBJ_DURATION;
		default:
			break;
		}
	}
	else if (Duration > 0)
	{
		Duration--;
	}
	else if (IsOnYellowLine())
	{
		setAction(YELLOW_AVOIDANCE);
		Duration = 9;
	}
	else if (obstacle(5, 10, 5))
	{
		if (triger())
		{
			motor(2, -2);
		}
		else
		{
			motor(-2, 2);
		}
	}
	// else if (IsOnDepositArea() && LoadedObjects < 6)
	// {
	// 	if (triger())
	// 	{
	// 		motor(5, 3);
	// 	}
	// 	else
	// 	{
	// 		motor(3, 5);
	// 	}
	// }
	else if (getRepeatedNum() < 100)
	{
		double target_angle = 360 - pow(getRepeatedNum(), 3) / pow(100, 2);
		// printf("continue %d %lf\n", getRepeatedNum(), target_angle);
		target_angle -= Compass;
		while (target_angle > 180)
		{
			target_angle -= 360;
		}
		while (target_angle < -180)
		{
			target_angle += 360;
		}
		if (target_angle < 0)
		{
			// 左を向いている
			motor(5, 3);
		}
		else
		{
			// 右を向いている
			motor(3, 5);
		}
	}
	else if (LoadedObjects >= triger_object_num || (LoadedObjects >= 3 && Time > 120))
	{
		if (US_Front < 15)
		{
			motor(-3, 3);
		}
		else if (US_Right < 10)
		{
			motor(2, 4);
		}
		else if (US_Right < 15)
		{
			motor(3, 4);
		}
		else if (US_Right < 40)
		{
			motor(4, 3);
		}
		else if (US_Right < 60)
		{
			motor(4, 1);
		}
		else
		{
			motor(4, 2);
		}
	}
	else
	{
		if (US_Front < 15)
		{
			motor(3, -3);
			subject[0]++;
		}
		else if (US_Left < 10)
		{
			motor(4, 1);
			subject[1]++;
		}
		else if (US_Left < 15)
		{
			if ((Compass > 45 && Compass < 135) || (Compass < 335 && Compass > 225))
			{
				motor(4, 2);
			}
			else
			{
				motor(5, 3);
			}
			subject[2]++;
		}
		else if (US_Left < 20 + rand() % 20)
		{
			if ((Compass > 45 && Compass < 135) || (Compass < 335 && Compass > 225))
			{
				motor(2, 4);
			}
			else
			{
				motor(4, 5);
			}
			subject[3]++;
		}
		else if (US_Left < 50)
		{
			if ((Compass > 45 && Compass < 135) || (Compass < 335 && Compass > 225))
			{
				motor(1, 4);
			}
			else
			{
				motor(3, 5);
			}
			subject[4]++;
		}
		else
		{
			if (!(rand() % 10))
			{
				motor(4, 5);
			}
			else
			{
				motor(2, 4);
			}
			subject[5]++;
		}
		for (int i = 0; i < 6; i++)
		{
			// printf("%d ", subject[i]);
		}
		// printf("\n");
	}

	switch (getAction())
	{
	case DEFINED:
		break;
	case YELLOW_AVOIDANCE:
		if (SuperDuration < 3 && Duration < 3)
		{
			if (triger())
			{
				motor_no_action_change(3, -3);
			}
			else
			{
				motor_no_action_change(-3, 3);
			}
		}
		else if ((SuperDuration != 0 && SuperDuration <= 5) || (Duration != 0 && Duration <= 5))
		{
			motor_no_action_change(0, 0);
		}
		else
		{
			motor_no_action_change(-5, -5);
		}
		break;
	case FIND_OBJ:
		motor_no_action_change(0, 0);
		LED_1 = 1;
		break;
	case DEPOSIT_OBJ:
		motor_no_action_change(0, 0);
		LED_1 = 2;
		if (Duration == 0 && SuperDuration == 0)
		{
			for (int i = 0; i < sizeof(loaded_objects) / sizeof(loaded_objects[0]); i++)
			{
				loaded_objects[i] = 0;
			}
			LoadedObjects = 0;
		}
		else
		{
			if (!IsOnDepositArea())
			{
				Duration = 0;
				SuperDuration = 0;
				LoadedObjects = 6;
				for (int i = 0; i < 3; i++)
				{
					loaded_objects[i] = 2;
				}
			}
		}
		break;
	default:
		break;
	}
}

FILE *fp;

void localGame1Setup(void)
{
	// fp = fopen("motor.txt", "a");
	srand((unsigned int)time(NULL));
	system("cls");
}

void localGame1(void)
{
	int left = 5;
	int right = 5;
	motor(5, 5);
	printf("%d\n", getRepeatedNum());
	if (getRepeatedNum() == 20)
	{
		printf("%d %d\n", PositionX, PositionY);
	}
	else if (getRepeatedNum() == 40)
	{
		printf("%d %d\n", PositionX, PositionY);
	}
	else if (getRepeatedNum() > 40)
	{
		motor(0, 0);
	}
	fflush(stdout);
}
