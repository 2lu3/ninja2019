#include "Hikaru.h"

#define FIND_OBJ_DURATION 50
#define DEPOSIT_OBJ_DURATION 80

void localGame0(void)
{
  if (SuperDuration > 0)
  {
    SuperDuration--;
  }
  else if (IsOnCyanObj() && loaded_objects[CYAN_LOADED_ID] < 3 && LoadedObjects < 6)
  {
    setAction(FIND_OBJ);
    loaded_objects[CYAN_LOADED_ID]++;
    LoadedObjects++;
    SuperDuration = FIND_OBJ_DURATION;
  }
  else if (IsOnRedObj() && loaded_objects[RED_LOADED_ID] < 3 && LoadedObjects < 6)
  {
    setAction(FIND_OBJ);
    loaded_objects[RED_LOADED_ID]++;
    LoadedObjects++;
    SuperDuration = FIND_OBJ_DURATION;
  }
  else if (IsOnBlackObj() && loaded_objects[BLACK_LOADED_ID] < 3 && LoadedObjects < 6)
  {
    setAction(FIND_OBJ);
    loaded_objects[BLACK_LOADED_ID]++;
    LoadedObjects++;
    SuperDuration = FIND_OBJ_DURATION;
  }
  else if (IsOnDepositArea() && LoadedObjects > 4)
  {
    switch (IsOnDepositArea())
    {
    case 1:
      motor(0, 4);
      break;
    case 2:
      motor(4, 0);
      break;
    case 3:
      motor(0, 0);
      setAction(DEPOSIT_OBJ);
      SuperDuration = DEPOSIT_OBJ_DURATION;
    default:
      break;
    }
  }
  else if (Duration > 0)
  {
    Duration--;
  }
  else if (IsOnYellowLine())
  {
    if (LoadedObjects < 6)
    {
      motor(3, -5);
    }
    else
    {
      motor(-5, -1);
    }
    Duration = 3;
  }
  else if (obstacle(5, 10, 5))
  {
    if (LoadedObjects < 6)
    {
      motor(2, -2);
    }
    else
    {
      motor(-2, 2);
    }
  }
  else if (LoadedObjects >= 6 || (LoadedObjects > 3 && Time > 120))
  {
    if (US_Front < 15)
    {
      motor(-3, 3);
    }
    else if (US_Right < 10)
    {
      motor(2, 4);
    }
    else if (US_Right < 15)
    {
      motor(3, 4);
    }
    else if (US_Right < 40)
    {
      motor(4, 3);
    }
    else
    {
      motor(4, 2);
    }
  }
  else
  {
    if (US_Front < 15)
    {
      motor(3, -3);
    }
    else if (US_Left < 10)
    {
      motor(5, 1);
    }
    else if (US_Left < 15)
    {
      motor(5, 3);
    }
    else if (US_Left < 30)
    {
      motor(4, 5);
    }
    else if (US_Left < 80)
    {
      motor(2, 4);
    }
    else
    {
      motor(2, 4);
    }
  }

  switch (getAction())
  {
  case DEFINED:
    break;
  case FIND_OBJ:
    motor_no_action_change(0, 0);
    LED_1 = 1;
    break;
  case DEPOSIT_OBJ:
    motor_no_action_change(0, 0);
    LED_1 = 2;
    if (Duration == 0 && SuperDuration == 0)
    {
      for (int i = 0; i < sizeof(loaded_objects) / sizeof(loaded_objects[0]); i++)
      {
        loaded_objects[i] = 0;
      }
      LoadedObjects = 0;
    }
    else
    {
      if (!IsOnDepositArea())
      {
        Duration = 0;
        SuperDuration = 0;
        LoadedObjects = 6;
        for (int i = 0; i < 3; i++)
        {
          loaded_objects[i] = 2;
        }
      }
    }
    break;
  default:
    break;
  }
}

void localGame1(void)
{
}
