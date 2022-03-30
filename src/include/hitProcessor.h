#pragma once
#include "PCH.h"
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
		auto difficulty = data::GetSingleton();
		if (isPlayerAggressor) {

			switch (RE::PlayerCharacter::GetSingleton()->getDifficultySetting()) {
			case RE::DIFFICULTY::kNovice: damage *= difficulty->fDiffMultHPByPCVE; break;
			case RE::DIFFICULTY::kApprentice: damage *= difficulty->fDiffMultHPByPCE; break;
			case RE::DIFFICULTY::kAdept: damage *= difficulty->fDiffMultHPByPCN; break;
			case RE::DIFFICULTY::kExpert: damage *= difficulty->fDiffMultHPByPCH; break;
			case RE::DIFFICULTY::kMaster: damage *= difficulty->fDiffMultHPByPCVH; break;
			case RE::DIFFICULTY::kLegendary: damage *= difficulty->fDiffMultHPByPCL; break;
			}
		}
		else {
			switch (RE::PlayerCharacter::GetSingleton()->getDifficultySetting()) {
			case RE::DIFFICULTY::kNovice: damage *= difficulty->fDiffMultHPToPCVE; break;
			case RE::DIFFICULTY::kApprentice: damage *= difficulty->fDiffMultHPToPCE; break;
			case RE::DIFFICULTY::kAdept: damage *= difficulty->fDiffMultHPToPCN; break;
			case RE::DIFFICULTY::kExpert: damage *= difficulty->fDiffMultHPToPCH; break;
			case RE::DIFFICULTY::kMaster: damage *= difficulty->fDiffMultHPToPCVH; break;
			case RE::DIFFICULTY::kLegendary: damage *= difficulty->fDiffMultHPToPCL; break;
			}
		}
		DEBUG("real damage is {}", damage);
	}

}