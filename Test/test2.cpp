#include <iostream>
#include <chrono>
#include <random>
#include <cstdlib>

#define N 200000
#define ABS(i) ((i ^ (i >> 31)) - (i >> 31))

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
  double end;
  // Processing_time.start();
  int x[2], y[2];
  // for (int ni = 0; ni < N; ni++)
  // {
  //   for (int j = 0; j < 1000; j++)
  //   {
  //     x[0] *= x[1];
  //     x[1] = x[0] / x[1];
  //     x[0] = x[0] / x[1];
  //   }
  // }
  // end = Processing_time.end();
  // cout << end << endl;

  Processing_time.start();
  for (int ni = 0; ni < N; ni++)
  {
    for (int j = 0; j < 1000; j++)
    {
      int temp = x[0];
      x[0] = x[1];
      x[1] = temp;
    }
  }
  end = Processing_time.end();
  cout << end << endl;
  Processing_time.start();
  int temp;
  for (int ni = 0; ni < N; ni++)
  {
    for (int j = 0; j < 1000; j++)
    {
      temp = x[0];
      x[0] = x[1];
      x[1] = temp;
    }
  }
  end = Processing_time.end();
  cout << end << endl;
}
