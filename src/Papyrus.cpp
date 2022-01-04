#include "Papyrus.h"

#include "dataHandler.h"

namespace Papyrus
{
	void Valhamina_MCM::OnConfigClose(RE::TESQuest*)
	{
		dataHandler::GetSingleton()->readSettings();
	}

	bool Valhamina_MCM::Register(RE::BSScript::IVirtualMachine* a_vm)
	{
		a_vm->RegisterFunction("OnConfigClose", "Valhamina_MCM", OnConfigClose);

		INFO("Registered Valhamina MCM class");
		return true;
	}

	void Register()
	{
		auto papyrus = SKSE::GetPapyrusInterface();
		papyrus->Register(Valhamina_MCM::Register);
		INFO("Registered papyrus functions");
	}
}