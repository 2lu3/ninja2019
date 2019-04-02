#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define PI 3.14

float wheelpower2cm[11] = {-3, -2.5, -1.85, -1.2, -0.45, 0, 0.45, 1.2, 1.85, 2.5, 3};

void calculateOdometory(int wheelLeft, int wheelRight, float log_compass, float compass, float log_position[], float calculated_position[])
{
  float x1, y1, x2, y2;
  // 回転角
  // float delta_theta = PI * (compass_after - compass_before) / 180;
  float delta_theta;
  // 回転する前の角度
  float theta = log_compass;
  // 左、右のモータ、ロボットの中央からの、回転の中心との距離
  float lenL, lenR, lenM = 15;
  // 左、右のモーターの動いた距離
  float deltaL, deltaR;

  deltaL = wheelpower2cm[wheelLeft + 5];
  deltaR = wheelpower2cm[wheelRight + 5];

  lenL = 15 * deltaL / (deltaR - deltaL);
  lenR = 15 * deltaR / (deltaR - deltaL);

  x1 = lenM * cos(compass);
  y1 = lenM * sin(compass);
  x2 = lenM * cos(theta + delta_theta);
  y2 = lenM * sin(theta + delta_theta);

  float x, y;
  x = x2 - x1;
  y = y2 - y1;

  calculated_position[0] = x + log_position[0];
  calculated_position[1] = y + log_position[1];
}
#define N 1000

float position[2];
float log_position[2];

int wheel[N][2];

int n;
int compass[N];
int correct_position[N][2];

int main()
{
  scanf("%d", &n);
  for (int i = 0; i < n; i++)
  {
    scanf("%d %d %d %d %d", &wheel[i][0], &wheel[i][1], &compass[i], &correct_position[i][0], &correct_position[1]);
  }

  position[0] = correct_position[0][0];
  position[1] = correct_position[0][1];

  for (int i = 1; i < 10; i++)
  {
    calculateOdometory(wheel[i - 1][0], wheel[i - 1][1], compass[i - 1], compass[i], log_position, position);
    printf("%f %f\n", position[0], position[1]);
  }
}
