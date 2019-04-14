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

private:
};

// World2
class Game1_Hikaru : public UserGame1
{
public:
	void setup(void) override;
	void loop(void) override;

private:
};

#endif // !HIKARU
