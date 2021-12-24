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
			INFO("setting {} to {}", settingStr, val);
			setting->data.f = val;
		}
	}

	/*multiplies stamina regen of every single race by MULT.*/
	inline void multStaminaRegen(int mult) {
		for (auto& race : RE::TESDataHandler::GetSingleton()->GetFormArray<RE::TESRace>()) {
			if (race && race->GetPlayable()) {
				DEBUG("setting regen value for race {} from {} to {}.", race->GetName(), race->data.staminaRegen, race->data.staminaRegen*mult);
				race->data.staminaRegen *= mult;
			}
		}
	}

	inline void damageav(RE::Actor* a, RE::ActorValue av, float val)
	{
		a->As<RE::ActorValueOwner>()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, av, -val);
	}

	inline void restoreav(RE::Actor* a, RE::ActorValue av, float val)
	{
		a->As<RE::ActorValueOwner>()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, av, val);
	}

};

