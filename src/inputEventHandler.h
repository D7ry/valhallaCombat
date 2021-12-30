#pragma once
#include "dataHandler.h"
using EventResult = RE::BSEventNotifyControl;

class inputEventHandler : public RE::BSTEventSink<RE::InputEvent*>
{
public:
	virtual EventResult ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource) override;

	static void Register()
	{
		INFO("Registering input event sink!");
		auto deviceManager = RE::BSInputDeviceManager::GetSingleton();
		deviceManager->AddEventSink(inputEventHandler::GetSingleton());
		INFO("Registered {}"sv, typeid(RE::InputEvent).name());
	}

	static std::uint32_t getAttackKey(RE::INPUT_DEVICE a_device);

private:
	static inputEventHandler* GetSingleton()
	{
		static inputEventHandler singleton;
		return std::addressof(singleton);
	}

	inputEventHandler() = default;
	inputEventHandler(const inputEventHandler&) = delete;
	inputEventHandler(inputEventHandler&&) = delete;
	virtual ~inputEventHandler() = default;

	inputEventHandler& operator=(const inputEventHandler&) = delete;
	inputEventHandler& operator=(inputEventHandler&&) = delete;


	static std::uint32_t GetGamepadIndex(RE::BSWin32GamepadDevice::Key a_key);

	enum : uint32_t
	{
		kInvalid = static_cast<uint32_t>(-1),
		kKeyboardOffset = 0,
		kMouseOffset = 256,
		kGamepadOffset = 266
	};
};
