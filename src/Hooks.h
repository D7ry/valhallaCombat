#include "attackHandler.h"
class StaminaHook
{
public:
	static void InstallHook();
private:
	static void readFromAttackData(RE::Actor* a_actor, RE::BGSAttackData* atkData);
};