#pragma once
#include "include/debuffHandler.h"
#include "include/blockHandler.h"
#include "include/stunHandler.h"
#include "include/balanceHandler.h"
#include "include/lib/TrueHUDAPI.h"
#include "include/lib/PrecisionAPI.h"
#include "include/settings.h"
/*Combat tweaks to make Skyrim's melee combat feel like AC:Valhalla.*/
class ValhallaCombat
{
public:

	TRUEHUD_API::IVTrueHUD3* ersh_TrueHUD = nullptr;
	PRECISION_API::IVPrecision1* ersh_Precision = nullptr;
	
	static ValhallaCombat* GetSingleton()
	{
		static ValhallaCombat singleton;
		return  std::addressof(singleton);
	}


	enum HANDLER {
		debuffHandler = 0,
		blockHandler,
		stunHandler,
		balanceHandler,
	};

	/*Start updating a handler per tick.
	@param handlerToActivate: the handler which will start updating per tick.*/
	void activateUpdate(HANDLER a_ValhallaHandler) {
		switch (a_ValhallaHandler)
		{
		case ValhallaCombat::debuffHandler: update_DebuffHandler = true;
			break;
		case ValhallaCombat::blockHandler: update_BlockHandler = true;
			break;
		case ValhallaCombat::stunHandler: update_StunHandler = true;
			break;
		case ValhallaCombat::balanceHandler: update_balanceHandler = true;
		}
	}
	/*Stop updating a handler per tick.
	@param handlerToDeactivate: the handler which will stop updating per tick.*/
	void deactivateUpdate(HANDLER a_ValallaHandler) {
		switch (a_ValallaHandler) {
		case ValhallaCombat::debuffHandler: update_DebuffHandler = false;
			break;
		case ValhallaCombat::blockHandler: update_BlockHandler = false;
			break;
		case ValhallaCombat::stunHandler: update_StunHandler = false;
			break;
		case ValhallaCombat::balanceHandler: update_balanceHandler = false;
		}
	}

	/*Runs every frame when game is not paused.*/
	void update() {
		if (RE::UI::GetSingleton()->GameIsPaused()) {
			return;
		}
		if (update_DebuffHandler) {
			debuffHandler::GetSingleton()->update();
		}
		if (update_BlockHandler) {
			blockHandler::GetSingleton()->update();
		}
		if (update_StunHandler) {
			stunHandler::GetSingleton()->update();
		}
		//if (update_balanceHandler) {
			//balanceHandler::GetSingleton()->update();
		//}
	}


	/*Request special bar control from truehud API. 
	If successful, set the truehud specialmeter global value to true.*/
	void requestTrueHudSpecialBarControl() {
		logger::info("Requesting trueHUD API special bar control...");
		if (!ersh_TrueHUD) {
			logger::info("...Failure: TrueHUD API is a null pointer.");
			return;
		}

		auto res = ersh_TrueHUD->RequestSpecialResourceBarsControl(SKSE::GetPluginHandle());
		switch (res) {
		case TRUEHUD_API::APIResult::OK:
		case TRUEHUD_API::APIResult::AlreadyGiven:
			ersh_TrueHUD->RegisterSpecialResourceFunctions(SKSE::GetPluginHandle(), stunHandler::getCurrentStun_static, stunHandler::getMaxStun_static, true, false);
			settings::facts::TrueHudAPI_HasSpecialBarControl = true;
			logger::info("...Success");
			settings::updateGlobals();
			break;
		case TRUEHUD_API::APIResult::AlreadyTaken:
			logger::info("...Failure: TrueHUD API already taken by another plugin");
			break;
		}
	}

	void releaseTrueHudSpecialBarControl() {
		logger::info("Release trueHUD API special bar control...");
		if (ValhallaCombat::GetSingleton()->ersh_TrueHUD
			->ReleaseSpecialResourceBarControl(SKSE::GetPluginHandle()) == TRUEHUD_API::APIResult::OK) {
			settings::facts::TrueHudAPI_HasSpecialBarControl = false;
			settings::updateGlobals();
			logger::info("...Success");
		}
		else {
			//logger::info("...Failure");
		}
	}

private:
	bool update_DebuffHandler;
	bool update_BlockHandler;
	bool update_StunHandler;
	bool update_balanceHandler;
};
