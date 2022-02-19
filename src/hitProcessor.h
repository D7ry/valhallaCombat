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

	/*Process a valid hit passed from hook. Offset the raw damage based on game setting.*/
	void processHit(RE::Actor* aggressor, RE::Actor* victim, RE::HitData& hitData);


};

namespace Utils
{
	/*Calculate the real hit damage based on game difficulty settings, and whether the player is aggressor/victim.
	@param damage: raw damage taken from hitdata.
	@param isPlayerAggressor: whether the play is aggressor or victim of this hitdata.*/
	inline void offsetRealDamage(float& damage, bool isPlayerAggressor) {
		DEBUG("damage before offset: {}", damage);
		if (isPlayerAggressor) {
			switch (RE::PlayerCharacter::GetSingleton()->getDifficultySetting()) {
			case RE::DIFFICULTY::kNovice: damage *= gameDataCache::fDiffMultHPByPCVE; break;
			case RE::DIFFICULTY::kApprentice: damage *= gameDataCache::fDiffMultHPByPCE; break;
			case RE::DIFFICULTY::kAdept: damage *= gameDataCache::fDiffMultHPByPCN; break;
			case RE::DIFFICULTY::kExpert: damage *= gameDataCache::fDiffMultHPByPCH; break;
			case RE::DIFFICULTY::kMaster: damage *= gameDataCache::fDiffMultHPByPCVH; break;
			case RE::DIFFICULTY::kLegendary: damage *= gameDataCache::fDiffMultHPByPCL; break;
			}
		}
		else {
			switch (RE::PlayerCharacter::GetSingleton()->getDifficultySetting()) {
			case RE::DIFFICULTY::kNovice: damage *= gameDataCache::fDiffMultHPToPCVE; break;
			case RE::DIFFICULTY::kApprentice: damage *= gameDataCache::fDiffMultHPToPCE; break;
			case RE::DIFFICULTY::kAdept: damage *= gameDataCache::fDiffMultHPToPCN; break;
			case RE::DIFFICULTY::kExpert: damage *= gameDataCache::fDiffMultHPToPCH; break;
			case RE::DIFFICULTY::kMaster: damage *= gameDataCache::fDiffMultHPToPCVH; break;
			case RE::DIFFICULTY::kLegendary: damage *= gameDataCache::fDiffMultHPToPCL; break;
			}
			DEBUG("real damage is {}", damage);
		}
	}

}