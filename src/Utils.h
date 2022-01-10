#pragma once
namespace Utils
{
	/*tweaks the value of designated game setting
	@param gameSettingStr game setting to be tweaked.
	@param val desired float value of gamesetting.*/
	inline void setGameSettingf(const char* settingStr, float val) {
		RE::Setting* setting = nullptr;
		RE::GameSettingCollection* _settingCollection = RE::GameSettingCollection::GetSingleton();
		setting = _settingCollection->GetSetting(settingStr);
		if (!setting) {
			INFO("invalid setting: {}", settingStr);
		}
		else {
			INFO("setting {} from {} to {}", settingStr, setting->GetFloat(), val);
			setting->data.f = val;
		}
	}

	inline static std::unordered_map<std::string, float> staminaRegenMap;

	/*multiplies stamina regen of every single race by MULT.*/
	inline void multStaminaRegen(float mult) {
		for (auto& race : RE::TESDataHandler::GetSingleton()->GetFormArray<RE::TESRace>()) {
			if (race && race->GetPlayable()) {
				std::string raceName = race->GetName();
				if (staminaRegenMap.find(raceName) == staminaRegenMap.end()) {
					DEBUG("recording race default stamina for {}!", raceName);
					staminaRegenMap[raceName] = race->data.staminaRegen;
					race->data.staminaRegen *= mult;
				}
				else {
					race->data.staminaRegen = mult * staminaRegenMap.at(raceName);
				}
				INFO("setting regen value for race {} to {}.", race->GetName(), race->data.staminaRegen);
			}
		}
	}



	inline void damageav(RE::Actor* a, RE::ActorValue av, float val)
	{
		if (a) {
			a->As<RE::ActorValueOwner>()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, av, -val);
		}
		/*DEBUG("{}'s {} damaged to {}",
			a->GetName(), 
			av, 
			a->GetActorValue(av));*/
	}

	inline void restoreav(RE::Actor* a, RE::ActorValue av, float val)
	{
		if (a) {
			a->As<RE::ActorValueOwner>()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, av, val);
		}
	}

	inline bool wieldingOneHanded(RE::Actor* a) {
		if (!a->GetEquippedObject(false)) {
			return true;
		}
		RE::WEAPON_TYPE wpnType = a->GetEquippedObject(false)->As<RE::TESObjectWEAP>()->GetWeaponType();
		if (wpnType >= RE::WEAPON_TYPE::kHandToHandMelee && wpnType <= RE::WEAPON_TYPE::kOneHandMace) {
			DEBUG("player wielding one handed weapon!");
			return true;
		}
		else {
			DEBUG("player wielding two handed weapon!");
			return false;
		}
	}

	inline void addPerkToPc(RE::BGSPerk* perk) {
		DEBUG("adding perk to pc");
		auto pc = RE::PlayerCharacter::GetSingleton();
		if (!perk) {
			DEBUG("perk does not exist!");
			return;
		}
		if (pc->HasPerk(perk)) {
			DEBUG("pc already has {}!", perk->GetFullName());
			return;
		}
		pc->AddPerk(perk);
		DEBUG("successfully added {} to pc", perk->GetFullName());
	}

	inline void rmPerkFromPc(RE::BGSPerk* perk) {
		DEBUG("removing perk from pc");
		auto pc = RE::PlayerCharacter::GetSingleton();
		if (!perk) {
			DEBUG("perk does not exist!");
			return;
		}
		if (!pc->HasPerk(perk)) {
			DEBUG("pc does not have {}!", perk->GetFullName());
			return;
		}
		pc->RemovePerk(perk);
		DEBUG("successfully removed {} from pc", perk->GetFullName());
	}

	typedef void(_fastcall* tFlashHUDMenuMeter)(RE::ActorValue a_actorValue);
	static REL::Relocation<tFlashHUDMenuMeter> FlashHUDMenuMeter{ REL::ID(51907) };

};

