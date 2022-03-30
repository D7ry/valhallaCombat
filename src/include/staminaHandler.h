#pragma once
#include "data.h"
#include "include/settings.h"
#include "include/attackHandler.h"
/*Convenience clas to deal with stamina operations.*/
class staminaHandler
{
public:
	static void checkStamina(RE::Actor* actor);
	static void staminaLightMiss(RE::Actor* a);
	static void staminaLightHit(RE::Actor* a);

	static void staminaHeavyMiss(RE::Actor* a);

	static void staminaHeavyHit(RE::Actor* a);



};
