#include <iostream>
#include <random>

using namespace std;

#define MAX_DOT_NUMBER 10
class Que
{
private:
    long que[MAX_DOT_NUMBER];
    long start_point = 0, end_point = 0, reading_point = 0;

public:
    const long error = -1;
    int add(long id)
    {
        if (start_point == (end_point + 1) % MAX_DOT_NUMBER)
        {
            return error;
        }
        que[end_point++] = id;
        if (end_point == MAX_DOT_NUMBER)
        {
            end_point = 0;
        }
        return -1;
    }

    int read(void)
    {
        if (reading_point == end_point)
        {
            return error;
        }
        long value = que[reading_point++];
        if (reading_point == MAX_DOT_NUMBER)
        {
            reading_point = 0;
        }
        return value;
    }

    void readReset(void)
    {
        reading_point = start_point;
    }

    long pop(void)
    {
        if (start_point == end_point)
        {
            return error;
        }
        long value = que[start_point++];
        if (start_point == MAX_DOT_NUMBER)
        {
            start_point = 0;
        }
        return value;
    }

    long search(long value)
    {
        long index = start_point;
        while (index != end_point)
        {
            if (que[index] == value)
            {
                return index;
            }
            index++;
            if (index == MAX_DOT_NUMBER)
            {
                index = 0;
            }
        }
        return error;
    }

    long pop(long value)
    {
        long id = search(value);
        if (start_point < end_point)
        {
            if (start_point <= id && id < end_point)
            {
                value = que[id];
                for (long i = id; i < end_point - 1; i++)
                {
                    que[i] = que[i + 1];
                }
                end_point--;
            }
            else
            {
                return error;
            }
        }
        else if (start_point == end_point)
        {
            return error;
        }
        else
        {
            if (start_point <= id + MAX_DOT_NUMBER && id < end_point)
            {
                value = que[id];
                for (long i = id; i < MAX_DOT_NUMBER - 1; i++)
                {
                    que[i] = que[i + 1];
                }
                que[MAX_DOT_NUMBER - 1] = que[0];
                for (long i = 0; i < end_point - 1; i++)
                {
                    que[i] = que[i + 1];
                }
                end_point--;
            }
            else
            {
                return error;
            }
        }
        return value;
    }

    void showList(void)
    {
        if (start_point <= end_point)
        {
            for (long i = start_point; i < end_point; i++)
            {
                cout << que[i] << " ";
            }
            cout << endl;
        }
        else
        {
            for (long i = start_point; i < MAX_DOT_NUMBER; i++)
            {
                cout << que[i] << " ";
            }
            for (long i = 0; i < start_point; i++)
            {
                cout << que[i] << " ";
            }
            cout << endl;
        }
    }
};

int command = 0;

int main()
{
    Que que;
    random_device rnd;
    while (1)
    {
        scanf("%d", &command);
        switch (command)
        {
        case 0:
            que.showList();
            break;
        case 1:
            que.add(rnd());
            que.showList();
            break;
        case 2:
            cout << que.read() << endl;
            que.showList();
            break;
        case 3:
            que.readReset();
            cout << "reset" << endl;
            break;
        default:
            que.pop(command);
            que.showList();
            break;
        }
    }
}
