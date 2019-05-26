#include <iostream>
#include <chrono>
#include <random>
#include <cstdlib>

#define N 2000000
#define ABS(i) (i ^ (i >> 31) - (i >> 31))

using namespace std;

class ProcessingTime
{
public:
  void start(void)
  {
    start_time = chrono::system_clock::now();
  }
  double end(void)
  {
    end_time = chrono::system_clock::now();
    double elapsed = (double)chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    return elapsed;
  }

private:
  chrono::system_clock::time_point start_time, end_time;
};

ProcessingTime Processing_time;

void test(int *a, int *b)
{
  int *pointer[2] = {a, b};
  *pointer[0] = 10;
  *pointer[1] = 20;
}
int main()
{
  int x = 0, y = 0;
  test(&x, &y);
  cout << x << y << endl;
  return 0;

  double end;
  int temp = 0;
  Processing_time.start();
  for (int ni = 0; ni < N; ni++)
  {
    for (int j = 0; j < 1000; j++)
    {
      if (temp < ABS(j) + ABS(j + 1))
      {
        temp = ABS(j) + ABS(j + 1);
      }
      else
      {
        temp = ABS(j) + ABS(j + 1);
      }
    }
    temp = 0;
  }
  end = Processing_time.end();
  cout << end << endl;

  Processing_time.start();
  register double temp2;
  for (int ni = 0; ni < N; ni++)
  {
    for (int j = 0; j < 1000; j++)
    {
      temp2 = ABS(j) + ABS(j + 1);
      if (temp < temp2)
      {
        temp = temp2;
      }
      else
      {
        temp = temp2;
      }
    }
    temp = 0;
  }
  end = Processing_time.end();
  cout << end << endl;

  Processing_time.start();
  for (int ni = 0; ni < N; ni++)
  {
    for (int j = 0; j < 1000; j++)
    {
      temp = max(temp, ABS(j) + ABS(j + 1));
    }
  }
  end = Processing_time.end();
  cout << end << endl;
}
