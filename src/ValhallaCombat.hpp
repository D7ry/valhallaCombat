#pragma once
#include "debuffHandler.h"
#include "blockHandler.h"
#include "include/TrueHUDAPI.h"
/*Combat tweaks to make Skyrim's melee combat feel like AC:Valhalla.*/
class ValhallaCombat
{
public:

	TRUEHUD_API::IVTrueHUD2* g_trueHUD = nullptr;


	static ValhallaCombat* GetSingleton()
	{
		static ValhallaCombat singleton;
		return  std::addressof(singleton);
	}

	/*Runs every frame*/
	void update() {
		debuffHandler::GetSingleton()->update();
		blockHandler::GetSingleton()->update();
	}

};