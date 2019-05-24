#ifndef HIKARU
#define HIKARU

#include "CospaceSettings.hpp"
#include "CommonTools.hpp"

// World1
class Game0_Hikaru : public UserGame0
{
public:
	void setup(void) override;
	void loop(void) override;
	int shouldTeleport(void) override;
	void taskOnTeleport(void) override;
	const static int kBorderSameObjNum = 2;
	const static int kFindObjDuration = 44;

private:
	ProcessingTime pt;
};

// World2
class Game1_Hikaru : public UserGame1
{
public:
	void setup(void) override;
	void loop(void) override;

private:
	ProcessingTime pt;
	const static int kCospaceWidth = 360;
	const static int kCospaceHeight = 270;
	const static int kSize = 10;
	const static int kDotWidthNum = kCospaceWidth / kSize;
	const static int kDotHeightNum = kCospaceHeight / kSize;
	const static int kMaxDotNum = kCospaceWidth * kCospaceHeight / kSize / kSize;
	const static int kMaxEdgeNum = 25;
	const static int kBorderSameObjNum = 2;
	const static int kFindObjDuration = Game0_Hikaru::kFindObjDuration;
	int log_superobj_num, log_superobj_x[10], log_superobj_y[10];
	int absolute_x = -1, absolute_y = -1;
	int now_dot_id;
	int emergency_now_dot_id = 80;
	int super_sameoperate = 0;
	int searching_object;
	int log_compass;
	int same_target_num = 5;
	int same_target = 0;
	int log_x = -1, log_y = -1;

	int large_process = -1;
	int process = 0;

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
		int distance_from_start;			  //Dijkstra()
		int curved_times;					  //Dijkstra()
		unsigned long long int arrived_times; //そこにいた回数
		int edge_num;						  //そのドットに行くことのできるドットの数
		int edge_to[kMaxEdgeNum];			  //
		int edge_cost[kMaxEdgeNum];			  //
		int red;							  //もし、Redがとれるなら、1
		int cyan;							  //もし、Cyanがとれないなら0
		int black;							  //もし、Blackが...
		int color;
	};
	struct Dot dot[kMaxDotNum];

	char dot_status[kMaxDotNum];
	int dot_from[kMaxDotNum];
	int dot_cost[kMaxDotNum];
	int dot_estimated_cost[kMaxDotNum];
	int dot_edge_to[kMaxEdgeNum];
	int dot_edge_cost[kMaxEdgeNum];
	char dot_is_red[kMaxDotNum];
	char dot_is_cyan[kMaxDotNum];
	char dot_is_black[kMaxDotNum];

	void GoToAngle(int angle, int distance);
	int GoToPosition(int x, int y, int wide_decide_x, int wide_decide_y, int wide_judge_arrived);
	void Dijkstra(void);
	void Astar(void);
	int GoToDot(int x, int y);
	int CheckNowDot(void);
	long WhereIsMotor(void);
	long WhereIsColorSensor(void);
	void InputDotInformation(void);
	int GoToDots(int x, int y, int wide_decide_x, int wide_decide_y);
	int GoInDots(int x, int y, int wide_decide_x, int wide_decide_y, int color);
	int HowManyCurved(int id);
	void AutoStrategy(void);
	int IsNearYellow(int num, int x, int y);
};

#endif // !HIKARU
