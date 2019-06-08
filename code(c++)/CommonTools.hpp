#ifndef COMMON_TOOLS
#define COMMON_TOOLS

#define REP for
#define rep(i, n) REP(int i = 0; i < (n); ++i)
#define PLUSMINUS(a, b, difference) ((b) < (a) + (difference) && (a) - (difference) < (b))
// abs() より18 % 速い
#define ABS(i) (((i) ^ ((i) >> 31)) - ((i) >> 31))
#define FUNCNAME getFuncName(__FUNCTION__)

// red cyan black super
#define RED_LOADED_ID 0
#define CYAN_LOADED_ID 1
#define BLACK_LOADED_ID 2
#define ID_SUPER_LOADED 3
extern int loaded_objects[4];

#include "CospaceSettings.hpp"
#include "LogErrorTools.hpp"
#include "JudgeAngleTools.hpp"
#include "JudgeColorTools.hpp"
#include "CalculatePositionTools.hpp"

int getRepeatedNum(void);
void addRepeatedNum(void);
void resetRepeatedNum(void);
void motor(int left, int right);
void motor_no_action_change(int left, int right);
int obstacle(int left, int front, int right);
void resetLoadedObjects(void);

// __FUNCTION__(const char) -> strinのへの変換
std::string getFuncName(const char *name);

class UserGame0
{
public:
	virtual ~UserGame0();
	virtual void setup(void);
	virtual void loop(void);
	virtual int shouldTeleport(void);
	virtual void taskOnTeleport(void);

private:
};

class UserGame1
{
public:
	virtual ~UserGame1();
	virtual void setup(void);
	virtual void loop(void);

private:
};

extern std::random_device rnd;

class ProcessingTime
{
public:
	void start(void);
	double end(void);
	void print(std::string message);
	void print();

private:
	std::chrono::system_clock::time_point start_time, end_time;
};

void setAction(Action pushed_action);
Action getAction(void);

Mode getDefaultRunMode();
void setDefaultRunMode(Mode pushed_mode);
Mode getRunMode(void);
void setRunMode(Mode pushed_mode);
bool getIsOutputLogMessage2Console(void);
void setIsOutputLogMessage2Console(bool option);
bool getIsOutputErrorMessage2Console(void);
void setIsOutputErrorMessage2Console(bool option);

#endif // !COMMON_TOOLS
