#include "include/events.h"
#include "include/data.h"
#include "include/settings.h"
#include "include/executionHandler.h"
#include "include/stunHandler.h"
#include "include/blockHandler.h"
using EventType = RE::INPUT_EVENT_TYPE;
using DeviceType = RE::INPUT_DEVICE;
const auto ui = RE::UI::GetSingleton();
uint32_t inputEventHandler::getBlockKey(RE::INPUT_DEVICE a_device){
	using DeviceType = RE::INPUT_DEVICE;
	const auto controlMap = RE::ControlMap::GetSingleton();
	auto key = controlMap->GetMappedKey(RE::UserEvents::GetSingleton()->blockStart, a_device);
	switch (a_device) {
	case DeviceType::kMouse:
		key += kMouseOffset;
		break;
	case DeviceType::kKeyboard:
		key += kKeyboardOffset;
		break;
	case DeviceType::kGamepad:
		key = GetGamepadIndex((RE::BSWin32GamepadDevice::Key)key);
		break;
	}
	//DEBUG("block key is {}", key);
	return key;
}
EventResult inputEventHandler::ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>*)
{
	if (!a_event || RE::UI::GetSingleton()->GameIsPaused()) {
		return EventResult::kContinue;
	}

	for (auto event = *a_event; event; event = event->next) {
		if (event->eventType != EventType::kButton)
		{
			continue;
		}
		auto button = static_cast<RE::ButtonEvent*>(event);
		if (button) {
			auto key = button->idCode;
			switch (button->device.get()) {
			case DeviceType::kMouse:
				key += kMouseOffset;
				break;
			case DeviceType::kKeyboard:
				key += kKeyboardOffset;
				break;
			case DeviceType::kGamepad:
				key = GetGamepadIndex((RE::BSWin32GamepadDevice::Key)key);
				break;
			default:
				continue;
			}
			if (key == settings::iExecutionKey) {
				if (button->IsDown()) {
					executionHandler::GetSingleton()->tryPcExecution();
				}
				break;
			}
			if (key == settings::iAltBlockKey || button->QUserEvent() == "Left Attack/Block") {
				if (button->IsDown()) {
					if (settings::bTimedBlockToggle || settings::bTimedBlockProjectileToggle) {
						blockHandler::GetSingleton()->onBlockKeyDown();
					}
					if (settings::bTackleToggle) {
						blockHandler::GetSingleton()->onTackleKeyDown();
					}
				}
				else if (button->IsUp()) {
					if (settings::bTimedBlockToggle || settings::bTimedBlockProjectileToggle) {
						blockHandler::GetSingleton()->onBlockKeyUp();
					}
					
				}
			}
		}

	}

	return EventResult::kContinue;

}

std::uint32_t inputEventHandler::GetGamepadIndex(RE::BSWin32GamepadDevice::Key a_key)
{
	using Key = RE::BSWin32GamepadDevice::Key;

	std::uint32_t index;
	switch (a_key)
	{
	case Key::kUp:
		index = 0;
		break;
	case Key::kDown:
		index = 1;
		break;
	case Key::kLeft:
		index = 2;
		break;
	case Key::kRight:
		index = 3;
		break;
	case Key::kStart:
		index = 4;
		break;
	case Key::kBack:
		index = 5;
		break;
	case Key::kLeftThumb:
		index = 6;
		break;
	case Key::kRightThumb:
		index = 7;
		break;
	case Key::kLeftShoulder:
		index = 8;
		break;
	case Key::kRightShoulder:
		index = 9;
		break;
	case Key::kA:
		index = 10;
		break;
	case Key::kB:
		index = 11;
		break;
	case Key::kX:
		index = 12;
		break;
	case Key::kY:
		index = 13;
		break;
	case Key::kLeftTrigger:
		index = 14;
		break;
	case Key::kRightTrigger:
		index = 15;
		break;
	default:
		index = kInvalid;
		break;
	}

	return index != kInvalid ? index + kGamepadOffset : kInvalid;
}
