#include "CommonTools.hpp"

using namespace std;

enum Action action;
enum Mode mode = MODE_NORMAL;
enum Mode default_mode = MODE_NORMAL;

int loaded_objects[4];
int repeated_num = 0;

void commonSetup0(void)
{
	srand((unsigned int)time(NULL));
}

void commonSetup1(void)
{
}

void commonLoop0(void)
{
	addRepeatedNum();
	if (CurGame == 1)
	{
		LoadedObjects = 0;
		for (int i = 0; i < sizeof(loaded_objects) / sizeof(*loaded_objects); i++)
		{
			loaded_objects[i] = 0;
		}
		resetRepeatedNum();
	}
}

void commonLoop1(void)
{
	addRepeatedNum();
}

int getRepeatedNum(void)
{
	return repeated_num;
}

void addRepeatedNum(void)
{
	repeated_num++;
}

void resetRepeatedNum(void)
{
	repeated_num = 0;
}

void motor(int left, int right)
{
	WheelLeft = left;
	WheelRight = right;
	action = DEFINED;
}

void motor_no_action_change(int left, int right)
{
	WheelLeft = left;
	WheelRight = right;
}

int obstacle(int left, int front, int right)
{
	int classification = 0;
	if (US_Left < left)
	{
		classification++;
	}
	if (US_Front < front)
	{
		classification += 2;
	}
	if (US_Right < right)
	{
		classification += 4;
	}
	return classification;
}

void ProcessingTime::start(void)
{
	start_time = chrono::system_clock::now();
}
double ProcessingTime::end(void)
{
	end_time = chrono::system_clock::now();
	double elapsed = (double)chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
	return elapsed;
}

void setAction(Action pushed_action)
{
	action = pushed_action;
}

Action getAction(void)
{
	return action;
}

Mode getDefaultRunMode()
{
	return default_mode;
}

void setDefaultRunMode(Mode pushed_mode)
{
	default_mode = pushed_mode;
}

void setRunMode(Mode pushed_mode)
{
	mode = pushed_mode;
}

Mode getRunMode(void)
{
	return mode;
}
