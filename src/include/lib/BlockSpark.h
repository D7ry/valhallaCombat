#pragma once
#include "include/Utils.h"
/*Maxsu's block spark function*/
namespace MaxsuBlockSpark
{

	class blockSpark
	{
	public:

		static blockSpark* GetSingleton()
		{
			static blockSpark singleton;
			return  std::addressof(singleton);
		}

		void playPerfectBlockSpark(RE::Actor* defender) {
			if (!defender || !defender->currentProcess || !defender->currentProcess->high || !defender->Get3D()) {
				return;
			}
			auto GetBipeObjIndex = [](RE::TESForm* parryEquipment, bool rightHand) -> RE::BIPED_OBJECT {
				if (!parryEquipment)
					return RE::BIPED_OBJECT::kNone;

				if (parryEquipment->As<RE::TESObjectWEAP>()) {
					switch (parryEquipment->As<RE::TESObjectWEAP>()->GetWeaponType()) {
					case RE::WEAPON_TYPE::kOneHandSword:
						return rightHand ? RE::BIPED_OBJECT::kOneHandSword : RE::BIPED_OBJECT::kShield;

					case RE::WEAPON_TYPE::kOneHandAxe:
						return rightHand ? RE::BIPED_OBJECT::kOneHandAxe : RE::BIPED_OBJECT::kShield;

					case RE::WEAPON_TYPE::kOneHandMace:
						return rightHand ? RE::BIPED_OBJECT::kOneHandMace : RE::BIPED_OBJECT::kShield;

					case RE::WEAPON_TYPE::kTwoHandAxe:
					case RE::WEAPON_TYPE::kTwoHandSword:
						return RE::BIPED_OBJECT::kTwoHandMelee;
					}
				}
				else if (parryEquipment->IsArmor())
					return RE::BIPED_OBJECT::kShield;

				return RE::BIPED_OBJECT::kNone;
			};
			RE::BIPED_OBJECT BipeObjIndex;
			auto defenderLeftEquipped = defender->currentProcess->GetEquippedLeftHand();
			auto defenderData = defender->currentProcess->high->attackData;

			if (defenderData)	//To compatible with "Simple Weapon Swing Parry" while attacking.
				BipeObjIndex = defenderData->IsLeftAttack() && defenderLeftEquipped ? GetBipeObjIndex(defenderLeftEquipped, false) : GetBipeObjIndex(defender->currentProcess->GetEquippedRightHand(), true);
			else
				BipeObjIndex = defenderLeftEquipped && (defenderLeftEquipped->IsWeapon() || defenderLeftEquipped->IsArmor()) ? GetBipeObjIndex(defenderLeftEquipped, false) : GetBipeObjIndex(defender->currentProcess->GetEquippedRightHand(), true);

			if (BipeObjIndex == RE::BIPED_OBJECT::kNone) {
				return;
			}

			auto defenderNode = defender->GetCurrentBiped()->objects[BipeObjIndex].partClone;
			if (!defenderNode || !defenderNode.get()) {
				return;
			}

			auto cell = defender->GetParentCell();

			const auto modelName = BipeObjIndex == RE::BIPED_OBJECT::kShield && defenderLeftEquipped && defenderLeftEquipped->IsArmor() ? "ValhallaCombat\\impactShieldRoot.nif" : "ValhallaCombat\\impactWeaponRoot.nif";
			
			RE::NiPoint3 sparkPos = defenderNode->worldBound.center;

			//DEBUG("Get Weapon Spark Position!");
			if (cell->PlaceParticleEffect(0.0f, modelName, defenderNode->world.rotate, sparkPos, 1.0f, 4U, defenderNode.get())) {
				//DEBUG("Play Spark Effect Successfully!");
			}
		}
	};
}

