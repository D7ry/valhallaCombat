#pragma once
#include "data.h"
#include "attackHandler.h"
#include "blockHandler.h"
using HITFLAG = RE::HitData::Flag;
class hitProcessor {
public:
	static hitProcessor* GetSingleton()
	{
		static hitProcessor singleton;
		return  std::addressof(singleton);
	}

<<<<<<< HEAD
=======
	/*Calculate the real hit damage based on game settings.
	@param damage: raw damage taken from hitdata.
	@param isPlayerAggressor: whether the play is aggressor or victim of this hitdata.*/
	float calculateRealDamage(float damage, bool isPlayerAggressor);

>>>>>>> parent of 53af9c6 (new guardbreak function)
	/*Process a valid hit passed from hook. Offset the raw damage based on game setting.*/
	void processHit(RE::Actor* aggressor, RE::Actor* victim, RE::HitData& hitData);


};
