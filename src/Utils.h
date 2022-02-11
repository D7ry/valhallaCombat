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
	/*a map of all game races' original stamina regen, in case player wants to tweak the stamina regen values again*/
	inline static std::unordered_map<std::string, float> staminaRegenMap;

	/*multiplies stamina regen of every single race by MULT.
	@param mult multiplier for stamina regen.
	@param upperLimit upper limit for stamina regen.*/
	inline void multStaminaRegen(float mult, float upperLimit) {
		for (auto& race : RE::TESDataHandler::GetSingleton()->GetFormArray<RE::TESRace>()) {
			if (race) {
				std::string raceName = race->GetName();
				float staminaRegen;
				if (staminaRegenMap.find(raceName) == staminaRegenMap.end()) {
					DEBUG("recording race default stamina for {}!", raceName);
					staminaRegenMap[raceName] = race->data.staminaRegen;
					staminaRegen = race->data.staminaRegen * mult;
				}
				else {
					staminaRegen = mult * staminaRegenMap.at(raceName);
				}
				if (staminaRegen <= upperLimit) {
					race->data.staminaRegen = staminaRegen;
				}
				else {
					race->data.staminaRegen = upperLimit;
				}
				INFO("setting stamina regen rate for race {} to {}.", race->GetName(), staminaRegen);
			}
		}
	}

	inline void damageav(RE::Actor* a, RE::ActorValue av, float val)
	{
		if (a) {
			a->As<RE::ActorValueOwner>()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, av, -val);
			DEBUG("{}'s {} damaged for {} points", a->GetName(), av, val);
		}
	}

	inline void restoreav(RE::Actor* a, RE::ActorValue av, float val)
	{
		if (a) {
			a->As<RE::ActorValueOwner>()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, av, val);
			DEBUG("{}'s {} restored for {} points", a->GetName(), av, val);
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

	inline void safeApplySpell(RE::SpellItem* a_spell, RE::Actor* a_actor) {
		if (a_actor && a_spell) {
			a_actor->AddSpell(a_spell);
			DEBUG("spell {} applied to {}.", a_spell->GetName(), a_actor->GetName());
		}
	}

	inline void safeRemoveSpell(RE::SpellItem* a_spell, RE::Actor* a_actor) {
		if (a_actor && a_spell) {
			a_actor->RemoveSpell(a_spell);
			DEBUG("spell {} removed from {}.", a_spell->GetName(), a_actor->GetName());
		}
	}

	typedef void(_fastcall* tFlashHUDMenuMeter)(RE::ActorValue a_actorValue);
	static REL::Relocation<tFlashHUDMenuMeter> FlashHUDMenuMeter{ REL::ID(51907) };

	static float* g_deltaTime = (float*)REL::ID(523660).address();                            // 2F6B948
	static float* g_deltaTimeRealTime = (float*)REL::ID(523661).address();                  // 2F6B94C


};


