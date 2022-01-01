class blockHandler {
public:
	static blockHandler* GetSingleton()
	{
		static blockHandler singleton;
		return  std::addressof(singleton);
	}

	blockHandler() {
		//FIXME::
		perfectBlockPerk = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSPerk>(0x0000433E, "ValhallaCombat.esp");
	}

	void raiseShield();

	void unRaiseShield();

	void blockAttack();



private:
	RE::BGSPerk* perfectBlockPerk;
	bool isPerfectBlocking = false;
	float perfectBlockTimer;
};