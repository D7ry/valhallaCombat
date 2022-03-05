#include "include/Papyrus.h"

#include "include/data.h"
#include "include/settings.h"
namespace Papyrus
{
	void ValhallaCombat_MCM::OnConfigClose(RE::TESQuest*)
	{
		settings::readSettings();
	}

	bool ValhallaCombat_MCM::Register(RE::BSScript::IVirtualMachine* a_vm)
	{
		a_vm->RegisterFunction("OnConfigClose", "ValhallaCombat_MCM", OnConfigClose);

		INFO("Registered ValhallaCombat MCM class");
		return true;
	}

	void Register()
	{
		auto papyrus = SKSE::GetPapyrusInterface();
		papyrus->Register(ValhallaCombat_MCM::Register);
		INFO("Registered papyrus functions");
	}
}