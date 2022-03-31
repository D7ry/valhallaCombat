#pragma once
#include "include/debuffHandler.h"
#include "include/blockHandler.h"
#include "include/stunHandler.h"
#include "include/lib/TrueHUDAPI.h"
/*Combat tweaks to make Skyrim's melee combat feel like AC:Valhalla.*/
class ValhallaCombat
{
public:

	TRUEHUD_API::IVTrueHUD3* g_trueHUD = nullptr;

	static ValhallaCombat* GetSingleton()
	{
		static ValhallaCombat singleton;
		return  std::addressof(singleton);
	}

	bool update_DebuffHandler;
	bool update_BlockHandler;
	bool update_StunHandler;
	/*Runs every frame*/
	void update() {
		if (update_DebuffHandler) {
			debuffHandler::GetSingleton()->update();
		}
		if (update_BlockHandler) {
			blockHandler::GetSingleton()->update();
		}
		if (update_StunHandler) {
			stunHandler::GetSingleton()->update();
		}
		
	}

};