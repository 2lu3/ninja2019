#ifndef COSPACE_SETTINGS
#define COSPACE_SETTINGS

/*
	注意点：すべてのファイルにインクルードされる
*/

enum Action
{
  DEFINED,
  FIND_OBJ,
  DEPOSIT_OBJ,
  TELEPORT,
  YELLOW_AVOIDANCE
};
//extern enum Action action;

enum Mode
{
  MODE_MATCH,
  MODE_NORMAL,
  MODE_DEBUG,
  MODE_VERBOSE
};

#define CsBot_AI_H //DO NOT delete this line
#ifndef CSBOT_REAL
#include <windows.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#define DLL_EXPORT extern __declspec(dllexport)
#define false 0
#define true 1
#endif //The robot ID : It must be two char, such as '00','kl' or 'Cr'.

extern char AI_MyID[2];
extern int Duration;
extern int SuperDuration;
extern int bGameEnd;
extern int CurAction;
extern int CurGame;
extern int SuperObj_Num;
extern int SuperObj_X;
extern int SuperObj_Y;
extern int Teleport;
extern int LoadedObjects;
extern int US_Front;
extern int US_Left;
extern int US_Right;
extern int CSLeft_R;
extern int CSLeft_G;
extern int CSLeft_B;
extern int CSRight_R;
extern int CSRight_G;
extern int CSRight_B;
extern int PositionX;
extern int PositionY;
extern int TM_State;
extern int Compass;
extern int Time;
extern int WheelLeft;
extern int WheelRight;
extern int LED_1;
extern int MyState;
extern int AI_SensorNum;

void Game0();
void Game1();

#define CsBot_AI_C //DO NOT delete this line

DLL_EXPORT void SetGameID(int GameID);
DLL_EXPORT int GetGameID();
//Only Used by CsBot Dance Platform
DLL_EXPORT int IsGameEnd();

#ifndef CSBOT_REAL

DLL_EXPORT char *GetDebugInfo();
DLL_EXPORT char *GetTeamName();
DLL_EXPORT int GetCurAction();
//Only Used by CsBot Rescue Platform
DLL_EXPORT int GetTeleport();
//Only Used by CsBot Rescue Platform
DLL_EXPORT void SetSuperObj(int X, int Y, int num);
//Only Used by CsBot Rescue Platform
DLL_EXPORT void GetSuperObj(int *X, int *Y, int *num);

#endif ////CSBOT_REAL

DLL_EXPORT void SetDataAI(volatile int *packet, volatile int *AI_IN);
DLL_EXPORT void GetCommand(int *AI_OUT);
DLL_EXPORT void OnTimer();
#endif //COSPACE_SETTINGS
#ifndef COMMON_TOOLS
#define COMMON_TOOLS

#ifndef LOG_ERROR_TOOLS
#define LOG_ERROR_TOOLS

#include <stdio.h>
#include <stdbool.h>

int logErrorToolsSetup(void);
bool errorMessage(char *message);
bool logMessage(char *message);
bool errorMessageOption(char *message, int option);
bool logMessageOption(char *message, int option);
bool outputData(char file_name[], char *message);
bool outputDataOption(char file_name[], char *message, int option);
void delErrorFile();
void delLogFile();
void delOutFile(char file_name[]);

#endif // !LOG_ERROR_TOOLS
// #include "JudgeAngleTools.hpp"
// #include "JudgeColorTools.hpp"

#define PI 3.14

// red cyan black super
#define RED_LOADED_ID 0
#define CYAN_LOADED_ID 1
#define BLACK_LOADED_ID 2
#define ID_SUPER_LOADED 3
extern int loaded_objects[4];

int getRepeatedNum(void);
void addRepeatedNum(void);
void resetRepeatedNum(void);
void motor(int left, int right);
void motor_no_action_change(int left, int right);
int obstacle(int left, int front, int right);

void commonSetup0(void);
void commonSetup1(void);
void commonSetup1(void);
void commonLoop0(void);

void setAction(enum Action pushed_action);
enum Action getAction(void);
enum Mode getDefaultRunMode();
void setDefaultRunMode(enum Mode pushed_mode);
void setRunMode(enum Mode pushed_mode);
enum Mode getRunMode(void);

#endif // !COMMON_TOOLS

// sprintfのエラーを無視するため
#define _CRT_SECURE_NO_WARNINGS 1

/*
	命名規則名	先頭	区切り	適用先
	PascalCase	大		大		クラス名
	camelCase	小		大		メソッド名、関数名
	snake_case	全て小	_		変数名、メンバ名、ファイル名
	SNAKE_CASE	全て大	_		マクロ名、定数、列挙体
	ただし、Cospaceで既に決まっている変数は上に従わない(Cospaceで定まっているものに同じ形式に変数名を変更する？)
	また、数字などの関係で視認性が悪いときもこれに従わなくて良い
*/
/*
	気をつけること
	・関数の引数がない場合、引数にvoidを入れること。引数がなにもない関数に引数を渡すと、全く別の変数の値が書き換わる。voidを入れると、引数を入れた時点でエラー表示が出る
	・
*/

/*	インクルードについて
ヘッダファイル同士が依存関係を持つことがないようにする
![image](out/include_files/include_files.svg)
*/

void Game0()
{
    if (getRepeatedNum() == 0)
    {
        commonSetup0();
    }
    commonLoop0();
    addRepeatedNum();
}

void Game1()
{
    if (getRepeatedNum() == 0)
    {
        commonSetup1();
    }
    addRepeatedNum();
}
#include "CospaceSettings.h"

char AI_MyID[2] = {'0', '2'};
int Duration = 0;
int SuperDuration = 0;
int bGameEnd = false;
int CurAction = -1;
int CurGame = 0;
int SuperObj_Num = 0;
int SuperObj_X = 0;
int SuperObj_Y = 0;
int Teleport = 0;
int LoadedObjects = 0;
int US_Front = 0;
int US_Left = 0;
int US_Right = 0;
int CSLeft_R = 0;
int CSLeft_G = 0;
int CSLeft_B = 0;
int CSRight_R = 0;
int CSRight_G = 0;
int CSRight_B = 0;
int PositionX = 0;
int PositionY = 0;
int TM_State = 0;
int Compass = 0;
int Time = 0;
int WheelLeft = 0;
int WheelRight = 0;
int LED_1 = 0;
int MyState = 0;
int AI_SensorNum = 13;

DLL_EXPORT void SetGameID(int GameID)
{
  CurGame = GameID;
  bGameEnd = 0;
}

DLL_EXPORT int GetGameID()
{
  return CurGame;
}

//Only Used by CsBot Dance Platform
DLL_EXPORT int IsGameEnd()
{
  return bGameEnd;
}

DLL_EXPORT char *GetDebugInfo()
{
  static char info[1024];
  sprintf(info, "Duration=%d;SuperDuration=%d;bGameEnd=%d;CurAction=%d;CurGame=%d;SuperObj_Num=%d;SuperObj_X=%d;SuperObj_Y=%d;Teleport=%d;LoadedObjects=%d;US_Front=%d;US_Left=%d;US_Right=%d;CSLeft_R=%d;CSLeft_G=%d;CSLeft_B=%d;CSRight_R=%d;CSRight_G=%d;CSRight_B=%d;PositionX=%d;PositionY=%d;TM_State=%d;Compass=%d;Time=%d;WheelLeft=%d;WheelRight=%d;LED_1=%d;MyState=%d;", Duration, SuperDuration, bGameEnd, CurAction, CurGame, SuperObj_Num, SuperObj_X, SuperObj_Y, Teleport, LoadedObjects, US_Front, US_Left, US_Right, CSLeft_R, CSLeft_G, CSLeft_B, CSRight_R, CSRight_G, CSRight_B, PositionX, PositionY, TM_State, Compass, Time, WheelLeft, WheelRight, LED_1, MyState);
  return info;
}

DLL_EXPORT char *GetTeamName()
{
  static char team_name[30] = "Cospace teams";
  return team_name;
}

DLL_EXPORT int GetCurAction()
{
  return CurAction;
}

//Only Used by CsBot Rescue Platform
DLL_EXPORT int GetTeleport()
{
  return Teleport;
}

//Only Used by CsBot Rescue Platform
DLL_EXPORT void SetSuperObj(int X, int Y, int num)
{
  SuperObj_X = X;
  SuperObj_Y = Y;
  SuperObj_Num = num;
}
//Only Used by CsBot Rescue Platform
DLL_EXPORT void GetSuperObj(int *X, int *Y, int *num)
{
  *X = SuperObj_X;
  *Y = SuperObj_Y;
  *num = SuperObj_Num;
}

DLL_EXPORT void SetDataAI(volatile int *packet, volatile int *AI_IN)
{

  int sum = 0;

  US_Front = AI_IN[0];
  packet[0] = US_Front;
  sum += US_Front;
  US_Left = AI_IN[1];
  packet[1] = US_Left;
  sum += US_Left;
  US_Right = AI_IN[2];
  packet[2] = US_Right;
  sum += US_Right;
  CSLeft_R = AI_IN[3];
  packet[3] = CSLeft_R;
  sum += CSLeft_R;
  CSLeft_G = AI_IN[4];
  packet[4] = CSLeft_G;
  sum += CSLeft_G;
  CSLeft_B = AI_IN[5];
  packet[5] = CSLeft_B;
  sum += CSLeft_B;
  CSRight_R = AI_IN[6];
  packet[6] = CSRight_R;
  sum += CSRight_R;
  CSRight_G = AI_IN[7];
  packet[7] = CSRight_G;
  sum += CSRight_G;
  CSRight_B = AI_IN[8];
  packet[8] = CSRight_B;
  sum += CSRight_B;
  PositionX = AI_IN[9];
  packet[9] = PositionX;
  sum += PositionX;
  PositionY = AI_IN[10];
  packet[10] = PositionY;
  sum += PositionY;
  TM_State = AI_IN[11];
  packet[11] = TM_State;
  sum += TM_State;
  Compass = AI_IN[12];
  packet[12] = Compass;
  sum += Compass;
  Time = AI_IN[13];
  packet[13] = Time;
  sum += Time;
  packet[14] = sum;
}
DLL_EXPORT void GetCommand(int *AI_OUT)
{
  AI_OUT[0] = WheelLeft;
  AI_OUT[1] = WheelRight;
  AI_OUT[2] = LED_1;
  AI_OUT[3] = MyState;
}

DLL_EXPORT void OnTimer()
{
  switch (CurGame)
  {
  case 9:
    break;
  case 10:
    WheelLeft = 0;
    WheelRight = 0;
    LED_1 = 0;
    MyState = 0;
    break;
  case 0:
    Game0();
    break;
  case 1:
    Game1();
  default:
    break;
  }
}
#include "CommonTools.h"
#include "CospaceSettings.h"

enum Action action;
enum Mode mode = MODE_NORMAL;
enum Mode default_mode = MODE_NORMAL;

int loaded_objects[4];
int repeated_num = 0;

void commonSetup0(void)
{
  srand((unsigned int)time(NULL));
  logErrorToolsSetup();
}

void commonSetup1(void)
{
}

void commonLoop0(void)
{
  addRepeatedNum();
  if (CurAction == 1)
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

void setAction(enum Action pushed_action)
{
  action = pushed_action;
}

enum Action getAction(void)
{
  return action;
}

enum Mode getDefaultRunMode()
{
  return default_mode;
}

void setDefaultRunMode(enum Mode pushed_mode)
{
  default_mode = pushed_mode;
}

void setRunMode(enum Mode pushed_mode)
{
  mode = pushed_mode;
}

enum Mode getRunMode(void)
{
  return mode;
}
#include "LogErrorTools.h"

#define FILE_NAME_LENGTH 20

char error_file_name[FILE_NAME_LENGTH] = "error_file.txt";
char log_file_name[FILE_NAME_LENGTH] = "log_file.txt";
FILE *error_file = NULL;
FILE *log_file = NULL;
FILE *output_file = NULL;

// World1の最初に実行する
int logErrorToolsSetup(void)
{
	error_file = fopen(error_file_name, "w");
	if (error_file == NULL)
	{
		printf("LogErrorTools.c setup() : failed to create error file\n");
	}
	log_file = fopen(log_file_name, "w");
	if (log_file == NULL)
	{
		printf("LogErrorTools.c setup() : failed to create log file");
	}
}

void delErrorFile()
{
	char command[FILE_NAME_LENGTH + 10];
	sprintf(command, "del %s\n", error_file_name);
	system(command);
}

void delLogFile()
{
	char command[FILE_NAME_LENGTH + 10];
	sprintf(command, "del %s\n", log_file_name);
	system(command);
}

void delOutFile(char file_name[])
{
	char command[FILE_NAME_LENGTH + 10];
	sprintf(command, "del %s\n", file_name);
	system(command);
}

void convertMessageFormat(char *message)
{
	sprintf(message, "%d %s", getRepeatedNum(), message);
	return message;
}

bool writeErrorMessage(char *message)
{
	convertMessageFormat(message);
	fprintf(error_file, "%s\n", message);
	return true;
}

// mode = default mode
bool errorMessage(char *message)
{
	if (getDefaultRunMode() <= getRunMode())
	{
		logMessage(message);
		bool result = writeErrorMessage(message);
		return result;
	}
	return true;
}

// mode = option
bool errorMessageOption(char *message, int option)
{
	if (option <= getRunMode())
	{
		logMessageOption(message, option);
		bool result = writeErrorMessage(message);
		return result;
	}
	return true;
}

bool writeLogMessage(char *message)
{
	convertMessageFormat(message);
	fprintf(log_file, "%s\n", message);
	return true;
}

// mode = default mode
bool logMessage(char *message)
{
	if (getDefaultRunMode() <= getRunMode())
	{
		bool result = writeLogMessage(message);
		if (MODE_DEBUG <= getRunMode())
		{
			convertMessageFormat(message);
			printf("%s\n", message);
		}
		return result;
	}
	return true;
}

bool logMessageOption(char *message, int option)
{
	if (option <= getRunMode())
	{
		bool result = writeLogMessage(message);
		if (MODE_DEBUG <= getRunMode())
		{
			convertMessageFormat(message);
			printf("%s\n", message);
		}
		return result;
	}
	return true;
}

bool outputData(char file_name[], char *message)
{
	if (getDefaultRunMode() < getRunMode())
	{
		return true;
	}
	output_file = fopen(file_name, "a");
	if (output_file == NULL)
	{
		char error_message[100] = "outputData() : Failed to output data file";
		printf("%s\n", error_message);
		errorMessage(error_message);
		return false;
	}
	if (getDefaultRunMode() >= MODE_DEBUG)
	{
		printf("%s\n", message);
	}
	fprintf(output_file, "%s\n", message);
	fclose(output_file);
	return true;
}

bool outputDataOption(char file_name[], char *message, int option)
{
	if (option < getRunMode())
	{
		return true;
	}
	bool result = outputData(file_name, message);
	if (getDefaultRunMode() < MODE_DEBUG && option >= MODE_DEBUG)
	{
		printf("%s\n", message);
	}
	return true;
}
