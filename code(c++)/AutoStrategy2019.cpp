#include "AutoStrategy2019.hpp"
#include <stdio.h>

#define TO_INT(VALUE) static_cast<int>((VALUE))
#define IF if
#define LOG_MESSAGE(MESSAGE, OPTION) \
	IF((OPTION) <= getRunMode()) { logErrorMessage.logMessage((MESSAGE), (OPTION)); }
#define ERROR_MESSAGE(MESSAGE, OPTION) \
	IF((OPTION) <= getRunMode()) { logErrorMessage.errorMessage((MESSAGE), (OPTION)); }

using std::cout;
using std::endl;
using std::extent;
using std::string;
using std::to_string;

void AutoStrategy::loop()
{
	pt.start();
	UserGame1::loop();
	LOG_MESSAGE("Auto Strategy loop start", MODE_DEBUG);

	// static int same_time = 0;
	// static int prev_repeated_num = 0;
	if (PositionX != 0 || PositionY != 0)
	{
		LOG_MESSAGE(FUNCNAME + "(): not in PLA and (x, y) is (" + to_string(PositionX) + ", " + to_string(PositionY) + ")", MODE_DEBUG);
		pos_x = PositionX;
		pos_y = PositionY;
		pos_compass = Compass;

		if (pos_x < 0)
			pos_x = 0;
		if (pos_x >= kCospaceWidth)
			pos_x = kCospaceWidth - 1;
		if (pos_y < 0)
			pos_y = 0;
		if (pos_y >= kCospaceHeight)
			pos_y = kCospaceHeight - 1;
		CheckNowDot();
		// arrived_timesを更新
		cospaceMap.addMapArrivedTimes(robot_dot_positions[0][0], robot_dot_positions[0][1]);
		cospaceMap.addMapArrivedTimes(robot_dot_positions[2][0], robot_dot_positions[2][1]);
	}
	else
	{
		PositionX = -1;
		int range = 2;
		for (int wi = 0; wi <= range * 2; ++wi)
		{
			for (int hj = 0; hj <= range * 2; ++hj)
			{
				int x = pos_x / kCM2DotScale + wi - range;
				int y = pos_y / kCM2DotScale + hj - range;
				if (x < 0 || x >= kDotWidth || y < 0 || y >= kDotHeight)
				{
					continue;
				}
				cospaceMap.addMapArrivedTimes(x, y, 10);
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

	LOG_MESSAGE(FUNCNAME + "(): start saving color data " + to_string(pt.end()) + " ms", MODE_NORMAL);

	// 色のデータを保存
	saveColorInfo();

	LOG_MESSAGE(FUNCNAME + "(): start calculate wall position " + to_string(pt.end()) + " ms", MODE_NORMAL);

	// 壁の位置を計算
	calculateWallPosition();
	LOG_MESSAGE(FUNCNAME + "(): start beautifying map data " + to_string(pt.end()) + " ms", MODE_NORMAL);

	// スレッドを使って、整形
	{
		LOG_MESSAGE(FUNCNAME + "(): データの整形を開始", MODE_DEBUG);
		int start_len = (getRepeatedNum() % kThreadNum) * kProcessingNumOfOneThread;
		int end_len = start_len + kProcessingNumOfOneThread;
		if (end_len > kDotHeight)
		{
			end_len = kDotHeight;
		}
		for (int yi = start_len; yi < end_len; ++yi)
		{
			if (yi < 0 || yi >= kDotHeight)
			{
				ERROR_MESSAGE(FUNCNAME + "(): thread yi is abnormal; " + to_string(yi), MODE_NORMAL);
			}
			if (yi != 0 && yi != kDotHeight - 1)
			{
				CospaceMap::MapInfo color_id[20];
				char color_num[20];
				int color_pointer = 0; // 最大値18(=20-2)
				int color_pointer_limit = 18;
				for (int xj = 1; xj < kDotWidth - 1; ++xj)
				{
					color_pointer = 0;
					// そのドットの周りで一番多い床の色を探す
					for (int y = yi - 1; y <= yi + 1; ++y)
					{
						for (int x = xj - 1; x <= xj + 1; ++x)
						{
							if (cospaceMap.getMapInfo(x, y) == cospaceMap.MAP_UNKNOWN)
							{
								continue;
							}
							color_id[color_pointer] = cospaceMap.getMapInfo(x, y);
							rep(i, color_pointer + 1)
							{
								if (color_id[i] == cospaceMap.getMapInfo(x, y))
								{
									if (i == color_pointer)
									{
										// 新しい色を追加
										if (color_pointer < color_pointer_limit)
										{
											color_id[color_pointer] = cospaceMap.getMapInfo(x, y);
											color_num[color_pointer] = 1;
											color_pointer++;
										}
										else
										{
											ERROR_MESSAGE(FUNCNAME + "(): error; To fix the floor map, the array num is over " + to_string(color_pointer_limit), MODE_NORMAL);
										}
									}
									else
									{
										// 色の数を++
										color_num[i]++;
									}
									break;
								}
							}
						}
					}

					int max_num = 0;
					CospaceMap::MapInfo max_id = cospaceMap.MAP_UNKNOWN;
					rep(i, color_pointer)
					{
						if (color_num[i] > max_num)
						{
							max_num = color_num[i];
							max_id = color_id[i];
						}
					}

					if (max_id == cospaceMap.MAP_UNKNOWN)
					{
						// ERROR_MESSAGE(FUNCNAME + "(): error; color_num has fatal error", MODE_NORMAL);
						continue;
					}

					if (max_num >= 5)
					{
						cospaceMap.setMapInfo(xj, yi, max_id);
					}
					// else if (max_num == 3)
					// {
					//     // 64128256
					//     //  8 16 32
					//     //  1  2  4
					//     int sum = 0;
					//     for (int y = yi - 1; y <= yi + 1; ++y)
					//     {
					//         for (int x = xj - 1; x <= xj + 1; ++x)
					//         {
					//             if (map[0][y][x] == max_id && (x != 0 || y != 0))
					//             {
					//                 sum += TO_INT(pow(2, (yi - y + 1) * 3 + (xj - x + 1)));
					//             }
					//         }
					//     }
					//     if (sum != 1 + 8 + 64 && sum != 1 + 2 + 4 && sum != 4 + 32 + 256 && sum != 64 + 128 + 256)
					//     {
					//         // 一列になっていないので、変更する
					//         map[0][yi][xj] = static_cast<char>(max_id);
					//     }
					// }
					// else
					// {
					//     int pos_x, pos_y, flag = 0;
					//     for (int y = yi - 1; y <= yi + 1; ++y)
					//     {
					//         for (int x = xj - 1; x <= xj + 1; ++x)
					//         {
					//             if (map[0][y][x] == max_id)
					//             {
					//                 if (flag == 0)
					//                 {
					//                     pos_x = x;
					//                     pos_y = y;
					//                 }
					//                 else
					//                 {
					//                     if (pos_x != x && pos_y != y)
					//                     {
					//                         // 縦横が重なっていないので、変更する
					//                         map[0][yi][xj] = static_cast<char>(max_id);
					//                     }
					//                 }
					//             }
					//         }
					//     }
					// }
				}

				// xj == 0の場合 上下の比較のみおこなう
				CospaceMap::MapInfo temp_map_color = cospaceMap.getMapInfo(0, yi - 1);
				if (temp_map_color == cospaceMap.getMapInfo(0, yi + 1))
				{
					if (temp_map_color != static_cast<char>(cospaceMap.MAP_UNKNOWN))
					{
						cospaceMap.setMapInfo(0, yi, temp_map_color);
					}
				}
				for (int xj = 1; xj < kDotWidth - 1; ++xj)
				{

					// 左右の比較
					if (cospaceMap.getMapInfo(xj - 1, yi) == cospaceMap.getMapInfo(xj + 1, yi))
					{
						if (cospaceMap.getMapInfo(xj + 1, yi) != cospaceMap.MAP_UNKNOWN)
						{
							cospaceMap.setMapInfo(xj, yi, cospaceMap.getMapInfo(xj + 1, yi));
						}
					}
					// 上下の比較
					else if (cospaceMap.getMapInfo(xj, yi - 1) == cospaceMap.getMapInfo(xj, yi + 1))
					{
						if (cospaceMap.getMapInfo(xj, yi + 1) != cospaceMap.MAP_UNKNOWN)
						{
							cospaceMap.setMapInfo(xj, yi, cospaceMap.getMapInfo(xj, yi + 1));
						}
					}
				}
				// xj == kDotWidth - 1 の場合、上下の比較のみ行う
				temp_map_color = cospaceMap.getMapInfo(kDotWidth - 1, yi - 1);
				if (temp_map_color == cospaceMap.getMapInfo(kDotWidth - 1, yi + 1))
				{
					if (temp_map_color != cospaceMap.MAP_UNKNOWN)
					{
						cospaceMap.setMapInfo(kDotWidth - 1, yi, temp_map_color);
					}
				}
			}
			else
			{
				// 左右の比較しか行わない
				for (int xj = 1; xj < kDotWidth - 1; ++xj)
				{
					// 左右の比較
					if (cospaceMap.getMapInfo(xj - 1, yi) == cospaceMap.getMapInfo(xj + 1, yi))
					{
						if (cospaceMap.getMapInfo(xj + 1, yi) != cospaceMap.MAP_UNKNOWN)
						{
							cospaceMap.setMapInfo(xj, yi, cospaceMap.getMapInfo(xj + 1, yi));
						}
					}
				}
			}
		}
		LOG_MESSAGE(FUNCNAME + "(): データの整形終了", MODE_DEBUG)
	}

	LOG_MESSAGE(FUNCNAME + "(): end beautifying map data " + to_string(pt.end()) + " ms", MODE_NORMAL);

	// データの出力
	if (getRepeatedNum() % (60 * 1000 / 60) == 0 && getRepeatedNum() != 0)
	{
		/*cout << "output!" << endl;
		logErrorMessage.outputData("out.txt", "\n");
		logErrorMessage.outputData("out.txt", "Start\n");
		rep(i, 5)
		{
			rep(yi, kDotHeight)
			{
				rep(xj, kDotWidth)
				{
					logErrorMessage.outputData("out.txt", to_string(TO_INT(cospaceMap.getMapInfo(xj, kDotHeight - 1 - yi))));
				}
				logErrorMessage.outputData("out.txt", "\n");
			}
			logErrorMessage.outputData("out.txt", "\n");
			logErrorMessage.outputData("out.txt", "\n");
		}
		logErrorMessage.outputData("out.txt", "End\n");
		logErrorMessage.outputData("out.txt", "\n");
		cout << "output! finished" << endl;*/
	}
	if (SuperDuration > 0)
	{
		--SuperDuration;
	}
	else if (IsOnBlackObj() && loaded_objects[BLACK_LOADED_ID] < kBorderSameObjNum && LoadedObjects < 6)
	{
		setAction(FIND_OBJ);
		SuperDuration = kFindObjDuration;
		++loaded_objects[BLACK_LOADED_ID];
		LOG_MESSAGE("Find Black Object!", MODE_DEBUG);
	}
	else if (IsOnCyanObj() && loaded_objects[CYAN_LOADED_ID] < kBorderSameObjNum && LoadedObjects < 6)
	{
		setAction(FIND_OBJ);
		SuperDuration = kFindObjDuration;
		++loaded_objects[CYAN_LOADED_ID];
		LOG_MESSAGE("Find Cyan Object!", MODE_NORMAL);
	}
	else if (IsOnRedObj() && loaded_objects[RED_LOADED_ID] < kBorderSameObjNum && LoadedObjects < 6)
	{
		setAction(FIND_OBJ);
		SuperDuration = kFindObjDuration;
		++loaded_objects[RED_LOADED_ID];
		LOG_MESSAGE("Find Red Object!", MODE_NORMAL);
	}
	else if (IsOnSuperObj() && LoadedObjects < 6)
	{

		int min = 10000;
		int num = 0;
		for (int i = 0; i < log_superobj_num; i++)
		{
			if (pow(log_superobj_x[i] - pos_x, 2) + pow(log_superobj_y[i] - pos_y, 2) < min)
			{
				min = static_cast<int>(pow(log_superobj_x[i] - pos_x, 2) + pow(log_superobj_y[i] - pos_y, 2));
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
			setAction(FIND_OBJ);
			SuperDuration = kFindObjDuration;
			++loaded_objects[SUPER_LOADED_ID];
			LOG_MESSAGE("Find Super Object!", MODE_NORMAL);
		}
		else
		{
			motor(3, 3);
			LOG_MESSAGE(FUNCNAME + "(): Super Obj Passed because it's not mine", MODE_NORMAL);
		}
	}
	else if (IsOnDepositArea() && LoadedObjects >= 5)
	{
		switch (IsOnDepositArea())
		{
		case 1:
			motor(5, 3);
			break;
		case 2:
			motor(3, 5);
			break;
		case 3:
			setAction(DEPOSIT_OBJ);
			SuperDuration = kDepositObjDuration;
			break;
		default:
			ERROR_MESSAGE(FUNCNAME + "(): switch(IsOnDepositArea) value is " + to_string(IsOnDepositArea()), MODE_NORMAL);
			break;
		}
	}
	else if (Duration > 0)
	{
		--Duration;
	}
	else if (IsOnYellowLine() && LoadedObjects != 0)
	{
		setAction(YELLOW_AVOIDANCE);
		Duration = 5;
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
				LOG_MESSAGE("There is no superobj", MODE_MATCH);
			}
			GoToPosition(log_superobj_x[0] - 5 + rnd() % 10, log_superobj_y[0] - 5 + rnd() % 10, 1, 1, 1);
			same_time++;
		}
	}
	else
	{
		autoSearch(0);
		// GoToDot(17, 22);
		// GoToAngle(345, 30);
		// motor(0, 0);
	}

	switch (TO_INT(getAction()))
	{
	case DEFINED:
		break;
	case YELLOW_AVOIDANCE:
		if (IsOnSwampland())
		{
			if (BothColorJudge(trap_line))
			{
				motor_no_action_change(5, -5);
			}
			else if (ColorJudgeLeft(trap_line))
			{
				motor_no_action_change(5, -5);
			}
			else
			{
				motor_no_action_change(-5, 5);
			}
		}
		else
		{
			if (IsOnYellowLine())
			{
				if (Duration >= 3)
				{
					motor_no_action_change(-3, -3);
				}
				else
				{

					if (BothColorJudge(trap_line))
					{
						motor_no_action_change(2, -3);
					}
					else if (ColorJudgeLeft(trap_line))
					{
						motor_no_action_change(2, -3);
					}
					else
					{
						motor_no_action_change(-3, 2);
					}
				}
			}
			else
			{
				motor(3, 3);
				Duration = 2;
				SuperDuration = 0;
			}
		}
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
	default:
		ERROR_MESSAGE(FUNCNAME + "(): action's value is ubnormal : " + to_string(TO_INT(getAction())), MODE_NORMAL);
		break;
	}

	pt.print("AS time :");
}

int AutoStrategy::GoToDot(int x, int y)
{
	static int prev_x = -1, prev_y = -1, prev_now_dot_id = -1;

	LOG_MESSAGE(FUNCNAME + "(" + to_string(x) + "," + to_string(y) + "): start", MODE_DEBUG);

	if (PositionX == -1 && (PLUSMINUS(pos_x, x * kCM2DotScale, (20 < kCM2DotScale ? kCM2DotScale : 20)) && PLUSMINUS(pos_y, y * kCM2DotScale, (20 < kCM2DotScale ? kCM2DotScale : 20))))
	{
		LOG_MESSAGE(FUNCNAME + "() end returning 1 because I am in PLA and it's near target(" + to_string(x) + ", " + to_string(y) + ")", MODE_NORMAL);
		GoToPosition(x * kCM2DotScale, y * kCM2DotScale, 10, 10, 5);
		return 1;
	}
	if (PLUSMINUS(pos_x, x * kCM2DotScale, kCM2DotScale) && PLUSMINUS(pos_y, y * kCM2DotScale, kCM2DotScale)) {
		LOG_MESSAGE(FUNCNAME + "(): Arrived!", MODE_NORMAL);
		GoToPosition(x * kCM2DotScale, y * kCM2DotScale, 10, 10, 5);
		return 1;
	}
	char map_data_to_show[kDotWidth * kDotHeight];
	rep(yi, kDotHeight)
	{
		rep(xj, kDotWidth)
		{
			if (cospaceMap.getMapInfo(xj, yi) == cospaceMap.MAP_WALL)
			{
				map_data_to_show[yi * kDotWidth + xj] = '#';
			}
			else if (cospaceMap.getMapInfo(xj, yi) == cospaceMap.MAP_YELLOW)
			{
				map_data_to_show[yi * kDotWidth + xj] = 'Y';
			}
			else if (cospaceMap.getMapInfo(xj, yi) == cospaceMap.MAP_SWAMPLAND)
			{
				map_data_to_show[yi * kDotWidth + xj] = '$';
			}
			else if (cospaceMap.getMapInfo(xj, yi) == cospaceMap.MAP_DEPOSIT)
			{
				map_data_to_show[yi * kDotWidth + xj] = 'D';
			}
			else
			{
				map_data_to_show[yi * kDotWidth + xj] = ' ';
			}
		}
	}

	//If the node I want to go will be go out
	if (x < 1 || x >= kDotWidth - 1 || y < 1 || y >= kDotHeight - 1)
	{
		ERROR_MESSAGE(FUNCNAME + "(): (x, y) is (" + to_string(x) + ", " + to_string(y) + ") and strange", MODE_NORMAL);
	}
	if (x < 0)
	{
		ERROR_MESSAGE(FUNCNAME + "() x is " + to_string(x), MODE_NORMAL);
		x = 0;
	}
	if (x >= kDotWidth)
	{
		ERROR_MESSAGE(FUNCNAME + "() x is " + to_string(x), MODE_NORMAL);
		x = kDotWidth - 1;
	}

	if (y < 0)
	{
		ERROR_MESSAGE(FUNCNAME + "() y is " + to_string(y), MODE_NORMAL);
		y = 0;
	}
	if (y >= kDotHeight)
	{
		ERROR_MESSAGE(FUNCNAME + "() y is " + to_string(y), MODE_NORMAL);
		y = kDotHeight - 1;
	}

	if (prev_now_dot_id != now_dot_id || prev_x != x || prev_y != y)
	{
		// Dijkstra();
		Astar(x, y);
	}
	prev_now_dot_id = now_dot_id;
	prev_x = x;
	prev_y = y;

	map_data_to_show[y * kDotWidth + x] = 'T';
	int i = 0;

	int back_search_x = x;
	int back_search_y = y;
	int temp_y = y;
	int temp_x = x;
	cospaceMap.getMapFrom(x, y, &temp_x, &temp_y);
	LOG_MESSAGE(FUNCNAME + "(): map_from(goal_x, goal_y) is (" + to_string(temp_x) + ", " + to_string(temp_y) + ")", MODE_VERBOSE);
	if (temp_x < 0)
	{
		LOG_MESSAGE(FUNCNAME + "(): gave up to calculate because of time", MODE_NORMAL);
		// Astarを途中で切り上げた
		int min_score_x = -1;
		int min_score_y = -1;
		int min_score = 100000;
		rep(yi, kDotHeight)
		{
			rep(xj, kDotWidth)
			{
				if (cospaceMap.getMapStatus(xj, yi) >= 1)
				{
					int score = TO_INT(sqrt(pow((x - xj) * kCM2DotScale, 2) + pow((y - yi) * kCM2DotScale, 2)));
					if (min_score > score)
					{
						min_score = score;
						min_score_x = xj;
						min_score_y = yi;
					}
				}
			}
		}
		back_search_x = min_score_x;
		back_search_y = min_score_y;
		LOG_MESSAGE(FUNCNAME + "(): Best Coordinate (" + to_string(back_search_x) + ", " + to_string(back_search_y) + ") after gave up; status = " + to_string(cospaceMap.getMapStatus(back_search_x, back_search_y)), MODE_NORMAL);
	}
	while (i < kDotWidth * 3)
	{
		LOG_MESSAGE(FUNCNAME + "(): back(" + to_string(back_search_x) + ", " + to_string(back_search_y) + ")", MODE_VERBOSE);
		cospaceMap.getMapFrom(back_search_x, back_search_y, &temp_x, &temp_y);
		LOG_MESSAGE(FUNCNAME + "(): temp(" + to_string(temp_x) + ", " + to_string(temp_y) + ")", MODE_VERBOSE);
		map_data_to_show[temp_y * kDotWidth + temp_x] = '+';
		i++;
		if (temp_x < 0 || temp_x >= kDotWidth)
		{
			ERROR_MESSAGE(FUNCNAME + "(): temp_x (=" + to_string(temp_x) + ") is strange. I continue", MODE_NORMAL);
			GoToPosition(x * kCM2DotScale, y * kCM2DotScale, 5, 5, 5);
		}
		if (temp_y < 0 || temp_y >= kDotHeight)
		{
			ERROR_MESSAGE(FUNCNAME + "(): temp_y (=" + to_string(temp_y) + ") is strange. I continue", MODE_NORMAL);
			GoToPosition(x * kCM2DotScale, y * kCM2DotScale, 5, 5, 5);
		}

		if (temp_x == robot_dot_positions[1][0] && temp_y == robot_dot_positions[1][1])
		{
			break;
		}
		back_search_x = temp_x;
		back_search_y = temp_y;
	}
	if (i == 200)
	{
		printf("\n\n\niの値が200ですByGoToNode()\n\n\n\n");
		LOG_MESSAGE(FUNCNAME + "(): iの値が200です", MODE_NORMAL);
	}

	map_data_to_show[now_dot_id] = '@';

	int next_x = back_search_x, next_y = back_search_y;

	int now_x = robot_dot_positions[1][0];
	int now_y = robot_dot_positions[1][1];

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
			GoToPosition(pos_x - 3 + rnd() % 6, pos_y - 3 + rnd() % 6, 6, 6, 3);
			ERROR_MESSAGE(FUNCNAME + "(): next_x == now_x && next_y == now_y; but we alredy passed plusminus; we run GoToPosition() and return 1", MODE_NORMAL);
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

	if (getRepeatedNum() % 5 == 0)
	{
		cout << "out map" << endl;
		ProcessingTime pt2;
		pt2.start();
		FILE* fp = fopen("map_out.txt", "w");
		if (fp == NULL)
		{
			ERROR_MESSAGE(FUNCNAME + "(): failed to make map_out.txt", MODE_NORMAL);
		}
		else
		{
			cout << "out map start" << endl;
			rep(xj, kDotWidth + 2)
			{
				fprintf(fp, "#");
				// printf("#");
			}
			rep(yi, kDotHeight)
			{
				fprintf(fp, "#");
				// printf("#");
				rep(xj, kDotWidth)
				{
					fprintf(fp, "%c", map_data_to_show[(kDotHeight - 1 - yi) * kDotWidth + xj]);
				}
				fprintf(fp, "#");
				// printf("#");
				fprintf(fp, "\n");
			}
			rep(xj, kDotWidth + 2)
			{
				fprintf(fp, "#");
			}
			fprintf(fp, "\n");
			fclose(fp);
			cout << "out map end " << pt2.end() << endl;
		}
	}

	LOG_MESSAGE(FUNCNAME + "(): return 0", MODE_DEBUG);
	return 0;
}

int AutoStrategy::
GoToDots(int x, int y, int wide_decide_x, int wide_decide_y)
{
	LOG_MESSAGE(FUNCNAME + "(" + to_string(x) + "," + to_string(y) + "," + to_string(wide_decide_x) + "," + to_string(wide_decide_y) + ")", MODE_VERBOSE);

	static int prev_x = -1;
	static int prev_y = -1;
	static int target_x = -1;
	static int target_y = -1;
	static int local_same_target = 0;
	static int same_target_border = 0;
	if (x != prev_x || y != prev_y)
	{
		LOG_MESSAGE(FUNCNAME + "(): changed dots", MODE_NORMAL);
		// Dijkstra();
		int max_value = 0;
		// int max_pos[2] = {-1, -1};
		int min_value = INT_MAX;
		int min_pos[2] = { TO_INT(x / kCM2DotScale), TO_INT(y / kCM2DotScale) };
		int cost;
		// for (long yi = kDotHeight - 1; yi >= 0; --yi)
		// {
		//     rep(xj, kDotWidth)
		//     {
		//         if (min_pos[0] == xj && min_pos[1] == yi)
		//         {
		//             printf("m");
		//             continue;
		//         }
		//         if (xj == robot_dot_positions[1][0] && yi == robot_dot_positions[1][1])
		//         {
		//             printf("#");
		//             continue;
		//         }
		//         switch (map[0][yi][xj])
		//         {
		//         case cospaceMap.MAP_YELLOW:
		//         case cospaceMap.MAP_WALL:
		//             printf("*");
		//             break;
		//         default:
		//             printf("%d", TO_INT(map_cost[yi][xj] * 9 / max_value));
		//         }
		//     }
		//     printf("\n");
		// }
		// printf("\n");

		local_same_target = 0;
		prev_x = x;
		prev_y = y;
		//0:left bottom corner 1:right bottom corner 2:right bottom corner
		int corner_x[2], corner_y[2];
		corner_x[0] = (x - wide_decide_x) / kCM2DotScale;
		corner_y[0] = (y - wide_decide_y) / kCM2DotScale;
		corner_x[1] = (x + wide_decide_x) / kCM2DotScale;
		corner_y[1] = (y + wide_decide_y) / kCM2DotScale;

		for (int i = 0; i < 2; i++)
		{
			if (corner_x[i] < 0)
			{
				corner_x[i] = 0;
			}
			if (corner_x[i] >= kDotWidth)
			{
				corner_x[i] = kDotWidth - 1;
			}
			if (corner_y[i] < 0)
			{
				corner_y[i] = 0;
			}
			if (corner_y[i] >= kDotHeight)
			{
				corner_y[i] = kDotHeight - 1;
			}
		}

		// int min = 100000, id = -1;
		// n回に1回移動する
		// int option = rnd() % 5;
		// int option = 1;
		for (int yi = corner_y[0]; yi <= corner_y[1]; ++yi)
		{
			for (int xj = corner_x[0]; xj <= corner_x[1]; ++xj)
			{
				cost = abs(yi - robot_dot_positions[1][1]) + abs(xj - robot_dot_positions[1][0]);
				if (yi <= TO_INT((10 - 1) / kCM2DotScale) || yi >= TO_INT((kCospaceHeight - 10) / kCM2DotScale))
				{
					continue;
				}
				if (xj <= TO_INT(10 - 1) / kCM2DotScale || xj >= TO_INT((kCospaceWidth - 10) / kCM2DotScale))
				{
					continue;
				}
				if (cospaceMap.getMapInfo(xj, yi) == cospaceMap.MAP_UNKNOWN)
				{
					cost -= 1000;
				}

				cost = cospaceMap.getMapCost(xj, yi);
				if (max_value < cost)
				{
					max_value = cost;
					// max_pos[0] = xj;
					// max_pos[1] = yi;
				}
				if (cost < min_value)
				{
					min_value = cost;
					min_pos[0] = xj;
					min_pos[1] = yi;
				}
				// int investigating_dot_id = j * kDotHeight + i;
				// if (i <= 0 || i >= kDotWidth - 1 || j <= 0 || j >= kDotHeight - 1)
				// {
				//     continue;
				// }

				// //yellow or wall or deposit
				// if (map[0][j][i] == cospaceMap.MAP_YELLOW || map[0][j][i] == cospaceMap.MAP_WALL || map[0][j][i] == cospaceMap.MAP_DEPOSIT)
				// {
				//     continue;
				// }

				// int costs = TO_INT(map_arrived_times[j][i] * 100 + rnd() % 10);
				// if (option)
				// {
				//     // 移動しないとき
				//     int k = 30;
				//     costs += TO_INT(pow(abs(i * kCM2DotScale - pos_x) - k, 2) * 100 - pow(abs(j * kCM2DotScale - pos_y) - k, 2) * 100);
				// }
				// else
				// {
				//     // 移動するとき
				//     costs -= TO_INT(pow(i * kCM2DotScale - pos_x, 2) / 100 - pow(j * kCM2DotScale - pos_y, 2) / 100);
				// }
				// if (costs < min)
				// {
				//     min = costs;
				//     id = investigating_dot_id;
				// }
			}
		}
		target_x = min_pos[0];
		target_y = min_pos[1];
		LOG_MESSAGE(FUNCNAME + "(): calculated best coordinate(" + to_string(target_x * kCM2DotScale) + "," + to_string(target_y * kCM2DotScale) + ")", MODE_DEBUG);
		// if (id == -1)
		// {
		//     target_x = x / kCM2DotScale;
		//     target_y = y / kCM2DotScale;
		// }
		// else
		// {
		//     target_y = id / kDotWidth;
		//     target_x = id - target_y * kDotWidth;
		// }

		same_target_border = TO_INT(sqrt(pow(pos_x - target_x * kCM2DotScale, 2) + pow(pos_y - target_y * kCM2DotScale, 2)));
		same_target_border *= 2;
		same_target_border += 30;
		LOG_MESSAGE(FUNCNAME + "(): set same_target_border as " + to_string(same_target_border), MODE_VERBOSE);
	}
	else
	{
		LOG_MESSAGE(FUNCNAME + "(): continued best coordinate(" + to_string(target_x * kCM2DotScale) + "," + to_string(target_y * kCM2DotScale) + ")", MODE_NORMAL);
	}
	local_same_target++;

	if (GoToDot(target_x, target_y))
	{
		prev_x = -1;
		local_same_target = 0;
		LOG_MESSAGE(FUNCNAME + "() : GoToDot() return 1; GoToDots() return 1", MODE_DEBUG);
		return 1;
	}
	else if (local_same_target > same_target_border)
	{
		prev_x = -1;
		local_same_target = 0;
		LOG_MESSAGE(FUNCNAME + "() : local_same_target(" + to_string(local_same_target) + ") < same_target_border(" + to_string(same_target_border) + ") return 1", MODE_DEBUG);
		return 1;
	}
	else
	{
		LOG_MESSAGE(FUNCNAME + "() : return 0", MODE_DEBUG);
		//fprintf(logfile, " %d End GoToDots() with returning 0\n", getRepeatedNum());
		return 0;
	}
}

void AutoStrategy::autoSearch(float parameter)
{
	// 0:通常の探索 1:Despositエリア(確定済み)に直行 2:Depositエリアを探索中
	static int status = 0;
	static int is_changed = 1;
	static int target_x = -1, target_y = -1;
	LOG_MESSAGE(FUNCNAME + "(" + to_string(parameter) + "): start; status = " + to_string(status), MODE_DEBUG);
	if (LoadedObjects >= 6 && (status != 1 && status != 2))
	{

		LOG_MESSAGE(FUNCNAME + "(): fully loaded; is_changed = true", MODE_VERBOSE);
		is_changed = 1;
	}
	if (LoadedObjects < 6 && (status == 1 || status == 2))
	{
		LOG_MESSAGE(FUNCNAME + "(): not loaded objects; is_changed = true", MODE_VERBOSE);
		is_changed = 1;
		status = 0;
	}
	if (0 <= target_x && target_x < kAreaWidth && 0 <= target_y && target_y < kAreaHeight) {
		if (cospaceMap.getMapInfo(target_x * kDot2AreaScale, target_y * kDot2AreaScale) == cospaceMap.MAP_YELLOW
			|| cospaceMap.getMapInfo(target_x * kDot2AreaScale, target_y * kDot2AreaScale) == cospaceMap.MAP_WALL) {
			is_changed = 1;
		}
	}


	if (is_changed)
	{
		LOG_MESSAGE(FUNCNAME + "(): is_changed true!", MODE_VERBOSE);
		is_changed = 0;
		if (LoadedObjects >= 6)
		{
			int min_cost = INT_MAX;
			target_x = -1;
			target_y = -1;
			rep(yi, kDotHeight)
			{
				rep(xj, kDotWidth)
				{
					if (cospaceMap.getMapInfo(xj, yi) == cospaceMap.MAP_DEPOSIT)
					{
						if (abs(yi - robot_dot_positions[1][1]) + abs(xj - robot_dot_positions[1][0]) < min_cost)
						{
							target_x = xj;
							target_y = yi;
							min_cost = abs(yi - robot_dot_positions[1][1]) + abs(xj - robot_dot_positions[1][0]);
						}
					}
				}
			}
			if (target_x == -1)
			{
				LOG_MESSAGE(FUNCNAME + "(): there is no deposit area. status = 2", MODE_VERBOSE);
				status = 2;
			}
			else
			{
				LOG_MESSAGE(FUNCNAME + "(): I go to deposit area (" + to_string(target_x * kCM2DotScale) + ", " + to_string(target_y * kCM2DotScale) + ")", MODE_VERBOSE);
				status = 1;
			}
		}
		if (status != 1)
		{
			int score_area_map[kAreaHeight][kAreaWidth];
			rep(ayi, kAreaHeight)
			{
				rep(axj, kAreaWidth)
				{
					score_area_map[ayi][axj] = 0;
				}
			}
			int score = 0;
			int max_score = INT_MIN;
			int max_score_x = -1;
			int max_score_y = -1;
			/*cout << "Arrived Times" << endl;
			rep(yi, kDotHeight) {
				rep(xj, kDotWidth) {
					printf("%3d", cospaceMap.getMapArrivedTimes(xj, yi));
				}
				printf("\n");
			}
			printf("\n");*/
			// 候補のエリアを選ぶ
			rep(yi, kDotHeight)
			{
				rep(xj, kDotWidth)
				{
					if (cospaceMap.getMapInfo(xj, yi) == cospaceMap.MAP_WALL || cospaceMap.getMapInfo(xj, yi) == cospaceMap.MAP_YELLOW)
					{
						continue;
					}

					score = kCM2AreaScale;
					switch (status)
					{
					case 0:
						if (cospaceMap.getMapInfo(xj, yi) == cospaceMap.MAP_UNKNOWN)
						{
							score *= 2;
						}
						if (cospaceMap.getMapArrivedTimes(xj, yi) > 0)
						{
							score -= cospaceMap.getMapArrivedTimes(xj, yi);
						}
						if (score < 0)
						{
							score = 0;
						}
						break;
					case 1:
						ERROR_MESSAGE(FUNCNAME + "(): switch status value is " + to_string(status), MODE_NORMAL);
						break;
					case 2:
						if (cospaceMap.getMapInfo(xj, yi) == cospaceMap.MAP_UNKNOWN)
						{
							score *= 10;
						}
						break;
					default:
						ERROR_MESSAGE(FUNCNAME + "(): switch status value is " + to_string(status), MODE_NORMAL);
						break;
					}
					score_area_map[TO_INT(yi / kDot2AreaScale)][TO_INT(xj / kDot2AreaScale)] += score;
				}
			}

			int max_value = INT_MIN;
			int base_lengh = 80;
			if (status == 2)
			{
				base_lengh = kCM2AreaScale;
			}
			rep(ayi, kAreaHeight)
			{
				rep(axj, kAreaWidth)
				{
					int value = TO_INT(sqrt(pow(abs(ayi * kCM2AreaScale - pos_y) - base_lengh, 2) + pow(abs(axj * kCM2AreaScale - pos_x) - base_lengh, 2)));
					if (value > max_value)
					{
						max_value = value;
					}
				}
			}
			rep(ayi, kAreaHeight)
			{
				rep(axj, kAreaWidth)
				{
					double distance = TO_INT(sqrt(pow(abs(ayi * kCM2AreaScale - pos_y) - base_lengh, 2) + pow(abs(axj * kCM2AreaScale - pos_x) - base_lengh, 2)));
					//cout << axj * kCM2AreaScale << " " << ayi * kCM2AreaScale << " score = " << score_area_map[ayi][axj] << " sigmoid " << i_sigmoid(distance / static_cast<double>(max_value) * 20.0 - 10.0, static_cast<double>(max_value)) << endl;
					// if (status == 2)
					// {
					score_area_map[ayi][axj] += i_sigmoid(10.0 - distance / static_cast<double>(max_value) * 20.0, static_cast<double>(max_value)) / 10;
					// }
					// else
					// {
					// score_area_map[ayi][axj] += i_sigmoid(distance / static_cast<double>(max_value) * 20.0 - 10.0, static_cast<double>(max_value)) + rnd() % 20;
					// }
					if (max_score < score_area_map[ayi][axj])
					{
						max_score = score_area_map[ayi][axj];
						max_score_x = axj;
						max_score_y = ayi;
					}
				}
			}
			//cout << "best " << max_score_x << " " << max_score_y << endl;
			target_x = max_score_x;
			target_y = max_score_y;
			LOG_MESSAGE(FUNCNAME + "(): calculated best area (" + to_string(max_score_x * kCM2AreaScale + TO_INT(kCM2AreaScale / 2)) + ", " + to_string(max_score_y * kCM2AreaScale + TO_INT(kCM2AreaScale / 2)) + ")", MODE_DEBUG);
			GoToDots(max_score_x * kCM2AreaScale + TO_INT(kCM2AreaScale / 2), max_score_y * kCM2AreaScale + TO_INT(kCM2AreaScale / 2), TO_INT(kCM2AreaScale / 2), TO_INT(kCM2AreaScale / 2));
		}
	}

	cout << "target " << target_x * kCM2AreaScale << " " << target_y * kCM2AreaScale << endl;
	if (GoToDots(target_x * kCM2AreaScale + TO_INT(kCM2AreaScale / 2), target_y * kCM2AreaScale + TO_INT(kCM2AreaScale / 2), TO_INT(kCM2AreaScale / 2), TO_INT(kCM2AreaScale / 2)))
	{
		is_changed = 1;
		LOG_MESSAGE(FUNCNAME + "(): is_changed true", MODE_VERBOSE);
	}
}

void AutoStrategy::Astar(int goal_x, int goal_y)
{
	static int is_not_finished= 0;
	static int log_astar[kDotHeight][kDotWidth];
	static int log_pointer = 0;
	static int loop_times = 0;
	ProcessingTime pt2;
	pt2.start();
	if (!is_not_finished) {
		LOG_MESSAGE(FUNCNAME + "(" + to_string(goal_x) + "," + to_string(goal_y) + "): start with init", MODE_DEBUG);
		// 初期化
		rep(yi, kDotHeight)
		{
			rep(xj, kDotWidth)
			{
				cospaceMap.setMapFrom(xj, yi, -1, -1);
				cospaceMap.setMapStatus(xj, yi, 0);
			}
		}
		if (robot_dot_positions[1][0] < 0 || robot_dot_positions[1][0] >= kDotWidth || robot_dot_positions[1][1] < 0 || robot_dot_positions[1][1] >= kDotHeight)
		{
			ERROR_MESSAGE(FUNCNAME + "(); now dot is (" + to_string(robot_dot_positions[1][0]) + ", " + to_string(robot_dot_positions[1][1]) + ")", MODE_NORMAL);
		}

		cospaceMap.setMapCost(robot_dot_positions[1][0], robot_dot_positions[1][1], 0);
		cospaceMap.setMapFrom(robot_dot_positions[1][0], robot_dot_positions[1][1], robot_dot_positions[1][0], robot_dot_positions[1][1]);
		cospaceMap.setMapStatus(robot_dot_positions[1][0], robot_dot_positions[1][1], 1);
		cospaceMap.setMapCurvedTimes(robot_dot_positions[1][0], robot_dot_positions[1][1], 0);
		log_pointer = 0;
		loop_times = 0;
		rep(i, kDotHeight)
		{
			rep(j, kDotWidth)
			{
				log_astar[i][j] = -1;
			}
		}
	}
	else {
		LOG_MESSAGE(FUNCNAME + "(" + to_string(goal_x) + "," + to_string(goal_y) + "): start with no-init " + to_string(is_not_finished), MODE_DEBUG);

	}

	

	
	
	// 外に出そうな危険な範囲には行かないようにする
	int dangerous_range = 10;
	// ドット1つ分と、dangerous_range、大きい方を採用する

	if (kCM2DotScale > dangerous_range)
	{
		dangerous_range = kCM2DotScale;
	}
	dangerous_range /= kCM2DotScale;
	while (loop_times < (is_not_finished+ 1) * 50)
	{
		loop_times++;
		int investigating_dot_x = -1, investigating_dot_y = -1;

		rep(yi, kDotHeight)
		{
			rep(xj, kDotWidth)
			{
				if (cospaceMap.getMapStatus(xj, yi) != 1)
				{
					continue;
				}
				if (
					investigating_dot_x == -1 ||
					cospaceMap.getMapTotalCost(xj, yi) < cospaceMap.getMapTotalCost(investigating_dot_x, investigating_dot_y) ||
					(cospaceMap.getMapTotalCost(xj, yi) == cospaceMap.getMapTotalCost(investigating_dot_x, investigating_dot_y) &&
						cospaceMap.getMapCost(xj, yi) < cospaceMap.getMapCost(investigating_dot_x, investigating_dot_y)))
				{
					investigating_dot_x = xj;
					investigating_dot_y = yi;
				}
			}
		}

		// goalに到着したとき
		int goal[2];
		cospaceMap.getMapFrom(goal_x, goal_y, &goal[0], &goal[1]);
		if (goal[0] != -1)
		{
			LOG_MESSAGE(FUNCNAME + "(): reach to goal(" + to_string(goal_x) + ", " + to_string(goal_y) + ")", MODE_DEBUG);
			break;
		}
		if (investigating_dot_x == -1)
		{
			ERROR_MESSAGE(FUNCNAME + "(): investigating_dot_x is -1", MODE_NORMAL);
			break;
		}

		cospaceMap.setMapStatus(investigating_dot_x, investigating_dot_y, 2);

		log_astar[investigating_dot_y][investigating_dot_x] = log_pointer;
		++log_pointer;

		for (int y = investigating_dot_y - 1; y <= investigating_dot_y + 1; ++y)
		{
			if (0 <= y && y < kDotHeight)
			{
				for (int x = investigating_dot_x - 1; x <= investigating_dot_x + 1; ++x)
				{
					if (0 <= x && x < kDotWidth)
					{
						// x = investigating_x, y = investigating_yは、setMapStatus(inv_x, inv_y, 2)で回避
						if (cospaceMap.getMapStatus(x, y) == 2)
						{
							continue;
						}
						int cost = kCM2DotScale;

						if (x < dangerous_range || x >= kDotWidth - dangerous_range || y < dangerous_range || y >= kDotHeight - dangerous_range)
						{
							cost += 10 * kCM2DotScale;
						}

						if (isNearTheFloor(cospaceMap.MAP_WALL, x, y, kCM2DotScale))
						{
							cost += kCM2DotScale / 2;
						}
						cost += kCM2DotScale / 2 * cospaceMap.getMapCurvedTimes(investigating_dot_x, investigating_dot_y, x, y);
						cost += cospaceMap.getMapArrivedTimes(x, y) / 5;

						if (cospaceMap.getMapInfo(x, y) == cospaceMap.MAP_YELLOW || cospaceMap.getMapInfo(x, y) == cospaceMap.MAP_WALL)
						{
							cost += 100 * kCM2DotScale;
							// continue;
						}
						if (cospaceMap.getMapInfo(x, y) == cospaceMap.MAP_SWAMPLAND)
						{
							cost += 100 * kCM2DotScale;
						}
						if (cospaceMap.getMapInfo(x, y) == cospaceMap.MAP_UNKNOWN && LoadedObjects < 6)
						{
							if (Time < 60)
							{
								cost -= kCM2DotScale;
							}
							else
							{
								cost -= kCM2DotScale / 2;
							}
						}
						/*if (cospaceMap.getMapObjInfo(x, y, RED_LOADED_ID) > 0 && loaded_objects[RED_LOADED_ID] < kBorderSameObjNum)
						{
							LOG_MESSAGE(FUNCNAME + "(): We can get Red Obj here(" + to_string(x * kCM2DotScale) + ',' + to_string(y * kCM2DotScale) + ")", MODE_VERBOSE);
							cost -= kCM2DotScale / 2;
						}
						if (cospaceMap.getMapObjInfo(x, y, CYAN_LOADED_ID) > 0 && loaded_objects[CYAN_LOADED_ID] < kBorderSameObjNum)
						{
							LOG_MESSAGE(FUNCNAME + "(): We can get Cyan Obj here(" + to_string(x * kCM2DotScale) + ',' + to_string(y * kCM2DotScale) + ")", MODE_VERBOSE);
							cost -= kCM2DotScale / 2;
						}
						if (cospaceMap.getMapObjInfo(x, y, BLACK_LOADED_ID) > 0 && loaded_objects[BLACK_LOADED_ID] < kBorderSameObjNum)
						{
							LOG_MESSAGE(FUNCNAME + "(): We can get Black Obj here(" + to_string(x * kCM2DotScale) + ',' + to_string(y * kCM2DotScale) + ")", MODE_VERBOSE);
							cost -= kCM2DotScale / 2;
						}*/
						if (cospaceMap.getMapStatus(x, y) == 0 || cospaceMap.getMapCost(investigating_dot_x, investigating_dot_y) + cost < cospaceMap.getMapCost(x, y))
						{
							cospaceMap.setMapCost(x, y, cospaceMap.getMapCost(investigating_dot_x, investigating_dot_y) + cost);
							cospaceMap.setMapTotalCost(x, y, cospaceMap.getMapCost(x, y) + TO_INT(sqrt(pow((goal_x - x) * kCM2DotScale, 2) + pow((goal_y - y) * kCM2DotScale, 2))));
							cospaceMap.setMapFrom(x, y, investigating_dot_x, investigating_dot_y);
							cospaceMap.setMapStatus(x, y, 1);
							cospaceMap.setMapCurvedTimes(x, y, cospaceMap.getMapCurvedTimes(investigating_dot_x, investigating_dot_y, x, y));
						}
					}
				}
			}
		}
	}
	if (getRepeatedNum() % 10 == 0) {
		rep(yi, kDotHeight)
		{
			rep(xj, kDotWidth)
			{

				if (goal_y == kDotHeight - yi - 1 && goal_x == xj)
				{
					printf("goal");
				}
				else if (robot_dot_positions[1][1] == kDotHeight - yi - 1 && robot_dot_positions[1][0] == xj)
				{
					printf(" go ");
				}
				else if (log_astar[kDotHeight - yi - 1][xj] == -1)
				{
					printf("    ");
				}
				else
				{

					printf("%3d ", log_astar[kDotHeight - yi - 1][xj]);
				}
			}
			printf("\n");
		}
		printf("\n");
	}
	
	// goalに到着したとき
	int goal[2];
	cospaceMap.getMapFrom(goal_x, goal_y, &goal[0], &goal[1]);
	if (goal[0] != -1)
	{
		is_not_finished= 1;
		return;
	}
	else {
		++is_not_finished;
	}

	LOG_MESSAGE(FUNCNAME + "(): while num " + to_string(loop_times), MODE_DEBUG);
	cout << "astar num " << loop_times << endl;
	cout << "astar " << pt2.end() << endl;

	// // 出力
	// int max_value = INT_MIN;
	// rep(yi, kDotHeight)
	// {
	//     rep(xj, kDotWidth)
	//     {
	//         if (cospaceMap.getMapCost(xj, yi) > max_value)
	//         {
	//             max_value = cospaceMap.getMapCost(xj, yi);
	//         }
	//     }
	// }
	// LOG_MESSAGE(FUNCNAME + "(): max value : " + to_string(max_value), MODE_VERBOSE);
	// if (max_value == INT_MIN)
	// {
	//     ERROR_MESSAGE(FUNCNAME + "(): max value is 0", MODE_NORMAL);
	//     return;
	// }
	// if (max_value < 0)
	// {
	//     ERROR_MESSAGE(FUNCNAME + "(): max value is " + to_string(max_value), MODE_NORMAL);
	// }

	// for (long yi = kDotHeight - 1; yi >= 0; --yi)
	// {
	//     rep(xj, kDotWidth)
	//     {
	//         switch (map[0][yi][xj])
	//         {
	//         case cospaceMap.MAP_YELLOW:
	//         case cospaceMap.MAP_WALL:
	//             printf("*");
	//             break;
	//         default:
	//             if (map_cost[yi][xj] < 0)
	//             {
	//                 printf("'");
	//             }
	//             else
	//             {
	//                 printf("%d", TO_INT(map_cost[yi][xj] * 9 / max_value));
	//             }
	//         }
	//     }
	//     printf("\n");
	// }
	// printf("\n");
}
