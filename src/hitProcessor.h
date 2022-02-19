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

	/*Offset the hit damage based on game settings.
	@param hitData: reference to hitdata whose damage will be offset.
	@param isPlayerAggressor: whether the play is aggressor or victim of this hitdata.*/
	inline void offsetRawDamage(RE::HitData& hitData, bool isPlayerAggressor);

	/*Process a valid hit passed from hook. Offset the raw damage based on game setting.*/
	void processHit(RE::Actor* aggressor, RE::Actor* victim, RE::HitData& hitData);


};
