#include <iostream>
#include <chrono>
#include <random>

using namespace std;
int map[180][270];

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

int main()
{
  random_device rnd;
  for (int i = 0; i < 180; i++)
  {
    for (int j = 0; j < 270; j++)
    {
      map[i][j] = rnd();
    }
  }
  int temp;
  Processing_time.start();
  for (int k = 0; k < 1000; k++)
  {
    for (int i = 0; i < 180; i++)
    {
      for (int j = 0; j < 270; j++)
      {
        map[i][j] = k * 3;
      }
    }
  }
  cout << Processing_time.end() << endl;

  Processing_time.start();

  for (int k = 0; k < 1000; k++)
  {
    for (int i = 0; i < 180; i++)
    {
      for (int j = 0; j < 270; j++)
      {
        map[i][j] = map[i][j] * 3;
      }
    }
  }
  cout << Processing_time.end() << endl;
}
