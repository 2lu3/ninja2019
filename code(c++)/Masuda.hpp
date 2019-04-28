#ifndef MASUDA
#define MASUDA

#include "CospaceSettings.hpp"
#include "CommonTools.hpp"

// World1
class Game0_Masuda : public UserGame0
{
public:
	void setup(void) override;
	void loop(void) override;
	int shouldTeleport(void) override;
	void taskOnTeleport(void) override;

private:
};


#endif // !MASUDA
