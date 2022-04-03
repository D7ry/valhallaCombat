#pragma once
#include "include/debuffHandler.h"
#include "include/blockHandler.h"
#include "include/stunHandler.h"
#include "include/lib/TrueHUDAPI.h"
#include "include/settings.h"
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


	enum HANDLER {
		debuffHandler = 0,
		blockHandler,
		stunHandler,
	};

	/*Start updating a handler per tick.
	@param handlerToActivate: the handler which will start updating per tick.*/
	void activateUpdate(HANDLER handlerToActivate) {
		switch (handlerToActivate)
		{
		case ValhallaCombat::debuffHandler: update_DebuffHandler = true;
			break;
		case ValhallaCombat::blockHandler: update_BlockHandler = true;
			break;
		case ValhallaCombat::stunHandler: update_StunHandler = true;
			break;
		}
	}
	/*Stop updating a handler per tick.
	@param handlerToDeactivate: the handler which will stop updating per tick.*/
	void deactivateUpdate(HANDLER handlerToDeactivate) {
		switch (handlerToDeactivate) {
		case ValhallaCombat::debuffHandler: update_DebuffHandler = false;
			break;
		case ValhallaCombat::blockHandler: update_BlockHandler = false;
			break;
		case ValhallaCombat::stunHandler: update_StunHandler = false;
			break;
		}
	}

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

	/*Request special bar control from truehud API. 
	If successful, set the truehud specialmeter global value to true.*/
	void requestTrueHudSpecialBarControl() {
		INFO("Request trueHUD API special bar control...");
		if (g_trueHUD) {
			if (g_trueHUD->RequestSpecialResourceBarsControl(SKSE::GetPluginHandle()) == TRUEHUD_API::APIResult::OK) {
				g_trueHUD->RegisterSpecialResourceFunctions(SKSE::GetPluginHandle(), stunHandler::getStun, stunHandler::getMaxStun , true, false);
				settings::TrueHudAPI_HasSpecialBarControl = true;
				settings::updateGlobals();
				INFO("...Success");
			}
			else {
				INFO("...Failure");
			}
		}
	}

	void releaseTrueHudSpecialBarControl() {
		INFO("Release trueHUD API special bar control...");
		if (ValhallaCombat::GetSingleton()->g_trueHUD
			->ReleaseSpecialResourceBarControl(SKSE::GetPluginHandle()) == TRUEHUD_API::APIResult::OK) {
			settings::TrueHudAPI_HasSpecialBarControl = false;
			settings::updateGlobals();
			INFO("...Success");
		}
		else {
			INFO("...Failure");
		}
	}

private:
	bool update_DebuffHandler;
	bool update_BlockHandler;
	bool update_StunHandler;

};