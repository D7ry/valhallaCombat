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
				if (staminaRegen > upperLimit) {
					staminaRegen = upperLimit;
				}
				race->data.staminaRegen = staminaRegen;
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

	inline void safeApplyPerk(RE::BGSPerk* a_perk, RE::Actor* a_actor) {
		if (a_actor && a_perk && !a_actor->HasPerk(a_perk)) {
			a_actor->AddPerk(a_perk);
			DEBUG("perk {} added to {}", a_perk->GetName(), a_actor->GetName());
		}
	}

	inline void safeRemovePerk(RE::BGSPerk* a_perk, RE::Actor* a_actor) {
		if (a_actor && a_perk && a_actor->HasPerk(a_perk)) {
			a_actor->RemovePerk(a_perk);
			DEBUG("perk {} removed from {}", a_perk->GetName(), a_actor->GetName());
		}
	}
	/*flash this actor's stmaina meter once.*/
	inline void flashStaminaMeter(RE::Actor* actor) {
		//DEBUG("Blinking {}'s stamina", actor->GetName());
		//ValhallaCombat::GetSingleton()->g_trueHUD->FlashActorValue(actor->GetHandle(), RE::ActorValue::kStamina, true);
	}
	/*Flash this actor's health meter once.*/
	inline void flashHealthMeter(RE::Actor* actor) {
		//ValhallaCombat::GetSingleton()->g_trueHUD->FlashActorValue(actor->GetHandle(), RE::ActorValue::kHealth, true);
	}

	inline void sendConsoleCommand(std::string a_command)
	{
		const auto scriptFactory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::Script>();
		const auto script = scriptFactory ? scriptFactory->Create() : nullptr;
		if (script) {
			const auto selectedRef = RE::Console::GetSelectedRef();
			script->SetCommand(a_command);
			script->CompileAndRun(selectedRef.get());
			delete script;
		}
	}
	typedef void(_fastcall* _shakeCamera)(float strength, RE::NiPoint3 source, float duration);
	static REL::Relocation<_shakeCamera> shakeCamera{ REL::ID(32275) };

	typedef void(_fastcall* tFlashHUDMenuMeter)(RE::ActorValue a_actorValue);
	static REL::Relocation<tFlashHUDMenuMeter> FlashHUDMenuMeter{ REL::ID(51907) };

	static float* g_deltaTime = (float*)REL::ID(523660).address();                            // 2F6B948
	static float* g_deltaTimeRealTime = (float*)REL::ID(523661).address();                  // 2F6B94C

	/*get the weapon the actor is most likely wielding.*/
	inline RE::TESObjectWEAP* getWieldingWeapon(RE::Actor* actor) {
		if (actor) {
			if (actor->GetAttackingWeapon() && actor->GetAttackingWeapon()->object) {
				return actor->GetAttackingWeapon()->object->As<RE::TESObjectWEAP>();
			}
			auto weapon = actor->GetEquippedObject(false);
			if (weapon && weapon->GetFormType() == RE::FormType::Weapon) {
				return weapon->As<RE::TESObjectWEAP>();
			}
			weapon = actor->GetEquippedObject(true);
			if (weapon && weapon->GetFormType() == RE::FormType::Weapon) {
				return weapon->As<RE::TESObjectWEAP>();
			}
		}
		return nullptr;
	}

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



};


