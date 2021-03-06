// API.cpp: Definiert die exportierten Funktionen für die DLL-Anwendung.
//

#include "stdafx.h"
#include "API.h"
#include "SerializeableQueue.h"
#include "ZoneManager.h"
#include "dllmain.h"
#include <Psapi.h>

#define CheckConnection(ret) if(g_client == nullptr) return ret
#define CheckValidation(message, ret) if(message->IsInvalid()) return ret

DllPublic int API::Initialize(const wchar_t * path, const wchar_t * file)
{
	if (g_client != nullptr)
		return -1;

	HWND window = FindWindow(0, L"GTA:SA:MP");
	if (!window)
		return -2;

	DWORD processId;
	GetWindowThreadProcessId(window, &processId);
	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, 0, processId);

	if (!process)
		return -3;

	// Check if API is already injected
	std::wstring apiName = std::wstring(file) + L".dll";
	HMODULE modules[1024] = {};
	DWORD sizeNeeded = 0;
	if (EnumProcessModules(process, modules, sizeof(modules), &sizeNeeded))
	{
		for (size_t i = 0; i < (sizeNeeded / sizeof(HMODULE)); i++)
		{
			wchar_t moduleName[MAX_PATH + 1];
			if (GetModuleFileNameEx(process, modules[i], moduleName, MAX_PATH))
			{
				std::wstring name = moduleName;
				auto fileName = name.substr(name.find_last_of(L'\\') + 1);
				if (fileName == apiName)
				{
					CloseHandle(process);

					g_client = std::make_shared<Client>();
					return 0;
				}
			}
		}
	}

	int pathLength = wcslen(path);
	HMODULE module = GetModuleHandle(L"kernel32.dll");
	LPTHREAD_START_ROUTINE routine = (LPTHREAD_START_ROUTINE)GetProcAddress(module, "LoadLibraryW");

	void *pathMemory = VirtualAllocEx(process, 0, (MAX_PATH * 2) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!pathMemory)
	{
		CloseHandle(process);
		return GetLastError();
	}

	WriteProcessMemory(process, pathMemory, path, pathLength * 2, 0);
	HANDLE thread = CreateRemoteThread(process, 0, 0, routine, pathMemory, 0, 0);
	WaitForSingleObject(thread, INFINITE);
	VirtualFreeEx(process, pathMemory, (MAX_PATH * 2) + 1, MEM_RELEASE);

	// Check if DLL is injected.
	if (EnumProcessModules(process, modules, sizeof(modules), &sizeNeeded))
	{
		for (size_t i = 0; i < (sizeNeeded / sizeof(HMODULE)); i++)
		{
			wchar_t moduleName[MAX_PATH + 1];
			if (GetModuleFileNameEx(process, modules[i], moduleName, MAX_PATH))
			{
				std::wstring name = moduleName;
				auto fileName = name.substr(name.find_last_of(L'\\') + 1);
				if (fileName == apiName)
				{
					CloseHandle(process);

					g_client = std::make_shared<Client>();
					return 0;
				}
			}
		}
	}
	return -4;
}

DllPublic void API::ResetInitialize()
{
	g_client = nullptr;
}

DllPublic int API::Other::GetWeatherID()
{
	CheckConnection(-1);
	return g_client->FastReadInteger(PacketIdentifier::General_GetWeatherId);
}

DllPublic int API::Other::SetKey(int key, bool pressed)
{
	CheckConnection(-1);
	auto message = g_client->CreateMessage(PacketIdentifier::General_SetKey);
	message->GetInput().WriteInteger(key);
	message->GetInput().WriteBoolean(pressed);
	g_client->Write(message);
}

DllPublic int API::Other::Useless(int useless)
{
	auto date = __DATE__;
	return (int)date;
}

DllPublic int API::Other::Useless1()
{
	auto time = __TIME__;
	return (int)time;
}

DllPublic int API::Overlay::Box::Create()
{
	CheckConnection(-1);

	return g_client->FastReadInteger(PacketIdentifier::Overlay_Box_Create);
}

DllPublic void API::Overlay::Box::Delete(int id)
{
	CheckConnection();

	g_client->FastWriteInteger(PacketIdentifier::Overlay_Box_Delete, id);
}

DllPublic void API::Overlay::Box::SetColor(int id, unsigned long color)
{
	CheckConnection();

	auto message = g_client->CreateMessage(PacketIdentifier::Overlay_Box_SetColor);
	message->GetInput().WriteInteger(id);
	message->GetInput().WriteInteger(color);
	g_client->Write(message);
}

DllPublic void API::Overlay::Box::SetX(int id, int x)
{
	CheckConnection();

	auto message = g_client->CreateMessage(PacketIdentifier::Overlay_Box_SetX);
	message->GetInput().WriteInteger(id);
	message->GetInput().WriteInteger(x);
	g_client->Write(message);
}

DllPublic void API::Overlay::Box::SetY(int id, int y)
{
	CheckConnection();

	auto message = g_client->CreateMessage(PacketIdentifier::Overlay_Box_SetY);
	message->GetInput().WriteInteger(id);
	message->GetInput().WriteInteger(y);
	g_client->Write(message);
}

DllPublic void API::Overlay::Box::SetWidth(int id, int width)
{
	CheckConnection();

	auto message = g_client->CreateMessage(PacketIdentifier::Overlay_Box_SetWidth);
	message->GetInput().WriteInteger(id);
	message->GetInput().WriteInteger(width);
	g_client->Write(message);
}

DllPublic void API::Overlay::Box::SetHeight(int id, int height)
{
	CheckConnection();

	auto message = g_client->CreateMessage(PacketIdentifier::Overlay_Box_SetHeight);
	message->GetInput().WriteInteger(id);
	message->GetInput().WriteInteger(height);
	g_client->Write(message);
}

DllPublic void API::Overlay::Box::SetActive(int id, bool active)
{
	CheckConnection();

	auto message = g_client->CreateMessage(PacketIdentifier::Overlay_Box_SetActive);
	message->GetInput().WriteInteger(id);
	message->GetInput().WriteBoolean(active);
	g_client->Write(message);
}

DllPublic int API::Overlay::Text::Create()
{
	CheckConnection(-1);

	return g_client->FastReadInteger(PacketIdentifier::Overlay_Text_Create);
}

DllPublic void API::Overlay::Text::Delete(int id)
{
	CheckConnection();

	g_client->FastWriteInteger(PacketIdentifier::Overlay_Text_Delete, id);
}

DllPublic void API::Overlay::Text::SetColor(int id, unsigned long color)
{
	CheckConnection();

	auto message = g_client->CreateMessage(PacketIdentifier::Overlay_Text_SetColor);
	message->GetInput().WriteInteger(id);
	message->GetInput().WriteInteger(color);
	g_client->Write(message);
}

DllPublic void API::Overlay::Text::SetX(int id, int x)
{
	CheckConnection();

	auto message = g_client->CreateMessage(PacketIdentifier::Overlay_Text_SetX);
	message->GetInput().WriteInteger(id);
	message->GetInput().WriteInteger(x);
	g_client->Write(message);
}

DllPublic void API::Overlay::Text::SetY(int id, int y)
{
	CheckConnection();

	auto message = g_client->CreateMessage(PacketIdentifier::Overlay_Text_SetY);
	message->GetInput().WriteInteger(id);
	message->GetInput().WriteInteger(y);
	g_client->Write(message);
}

DllPublic void API::Overlay::Text::SetMaxWidth(int id, int maxWidth)
{
	CheckConnection();

	auto message = g_client->CreateMessage(PacketIdentifier::Overlay_Text_SetMaxWidth);
	message->GetInput().WriteInteger(id);
	message->GetInput().WriteInteger(maxWidth);
	g_client->Write(message);
}

DllPublic void API::Overlay::Text::SetMaxHeight(int id, int maxHeight)
{
	CheckConnection();

	auto message = g_client->CreateMessage(PacketIdentifier::Overlay_Text_SetMaxHeight);
	message->GetInput().WriteInteger(id);
	message->GetInput().WriteInteger(maxHeight);
	g_client->Write(message);
}

DllPublic void API::Overlay::Text::SetActive(int id, bool active)
{
	CheckConnection();

	auto message = g_client->CreateMessage(PacketIdentifier::Overlay_Text_SetActive);
	message->GetInput().WriteInteger(id);
	message->GetInput().WriteBoolean(active);
	g_client->Write(message);
}

DllPublic void API::Overlay::Text::SetText(int id, const wchar_t * text)
{
	CheckConnection();

	auto message = g_client->CreateMessage(PacketIdentifier::Overlay_Text_SetText);
	message->GetInput().WriteInteger(id);
	message->GetInput().WriteString(text);
	g_client->Write(message);
}

DllPublic void API::Overlay::Text::SetSize(int id, int size)
{
	CheckConnection();

	auto message = g_client->CreateMessage(PacketIdentifier::Overlay_Text_SetSize);
	message->GetInput().WriteInteger(id);
	message->GetInput().WriteInteger(size);
	g_client->Write(message);
}

DllPublic void API::Overlay::Text::SetUseMaxWidth(int id, bool state)
{
	CheckConnection();

	auto message = g_client->CreateMessage(PacketIdentifier::Overlay_Text_SetUseMaxWidth);
	message->GetInput().WriteInteger(id);
	message->GetInput().WriteBoolean(state);
	g_client->Write(message);
}

DllPublic void API::Overlay::Text::SetUseMaxHeight(int id, bool state)
{
	CheckConnection();

	auto message = g_client->CreateMessage(PacketIdentifier::Overlay_Text_SetUseMaxHeight);
	message->GetInput().WriteInteger(id);
	message->GetInput().WriteBoolean(state);
	g_client->Write(message);
}

DllPublic void API::Overlay::Text::GetTextExtent(int id, int & width, int & height)
{
	CheckConnection();

	auto message = g_client->CreateMessage(PacketIdentifier::Overlay_Text_GetTextExtent);
	message->GetInput().WriteInteger(id);
	g_client->Write(message);

	CheckValidation(message);

	width = message->GetResponse()->ReadInteger();
	height = message->GetResponse()->ReadInteger();
}

DllPublic void API::SAMP::Chat::Send(const wchar_t * message)
{
	CheckConnection();

	g_client->FastWriteString(PacketIdentifier::SAMP_Chat_Send, message);
}

DllPublic void API::SAMP::Chat::AddMessage(const wchar_t * message)
{
	CheckConnection();

	g_client->FastWriteString(PacketIdentifier::SAMP_Chat_AddMessage1, message);
}

DllPublic void API::SAMP::Chat::AddMessage(const wchar_t * message, unsigned long color)
{
	CheckConnection();

	auto m = g_client->CreateMessage(PacketIdentifier::SAMP_Chat_AddMessage2);
	m->GetInput().WriteString(message);
	m->GetInput().WriteInteger(color);
	g_client->Write(m);
}

DllPublic void API::SAMP::Chat::GetText(wchar_t * text, int length)
{
	CheckConnection();

	auto t = g_client->FastReadString(PacketIdentifier::SAMP_Chat_GetText);
	wcscpy_s(text, length, t.c_str());
}

DllPublic void API::SAMP::Chat::SetText(const wchar_t * text)
{
	CheckConnection();

	g_client->FastWriteString(PacketIdentifier::SAMP_Chat_SetText, text);
}

DllPublic void API::SAMP::Chat::GetBufferMessage(int index, wchar_t * message, int length)
{
	CheckConnection();

	auto m = g_client->CreateMessage(PacketIdentifier::SAMP_Chat_GetBufferMessage);
	m->GetInput().WriteInteger(index);
	g_client->Write(m);

	CheckValidation(m);

	auto text = m->GetResponse()->ReadString();
	wcscpy_s(message, length, text.c_str());
}

DllPublic void API::SAMP::Chat::AddBufferMessage(const wchar_t * message)
{
	CheckConnection();

	g_client->FastWriteString(PacketIdentifier::SAMP_Chat_AddBufferMessage, message);
}

DllPublic void API::SAMP::Chat::SetCursorPosition(int index)
{
	CheckConnection();

	g_client->FastWriteInteger(PacketIdentifier::SAMP_Chat_SetCursorPosition1, index);
}

DllPublic void API::SAMP::Chat::SetCursorPosition(int indexStart, int indexEnd)
{
	CheckConnection();

	auto message = g_client->CreateMessage(PacketIdentifier::SAMP_Chat_SetCursorPosition2);
	message->GetInput().WriteInteger(indexStart);
	message->GetInput().WriteInteger(indexEnd);
	g_client->Write(message);
}

DllPublic void API::SAMP::Chat::Toggle(bool state)
{
	CheckConnection();

	g_client->FastWriteBoolean(PacketIdentifier::SAMP_Chat_Toggle, state);
}

DllPublic void API::SAMP::Chat::Clear()
{
	CheckConnection();

	g_client->FastWriteVoid(PacketIdentifier::SAMP_Chat_Clear);
}

DllPublic bool API::SAMP::Chat::IsOpen()
{
	CheckConnection(false);

	return g_client->FastReadBoolean(PacketIdentifier::SAMP_Chat_IsOpen);
}

DllPublic void API::SAMP::Dialog::GetText(wchar_t * text, int length)
{
	CheckConnection();

	auto t = g_client->FastReadString(PacketIdentifier::SAMP_Dialog_GetText);
	wcscpy_s(text, length, t.c_str());
}

DllPublic void API::SAMP::Dialog::SetText(const wchar_t * text)
{
	CheckConnection();

	g_client->FastWriteString(PacketIdentifier::SAMP_Dialog_SetText, text);
}

DllPublic void API::SAMP::Dialog::ClearText()
{
	CheckConnection();

	g_client->FastWriteVoid(PacketIdentifier::SAMP_Dialog_Clear);
}

DllPublic void API::SAMP::Dialog::SetCursorPosition(int index)
{
	CheckConnection();

	g_client->FastWriteInteger(PacketIdentifier::SAMP_Dialog_SetCursorPosition1, index);
}

DllPublic void API::SAMP::Dialog::SetCursorPosition(int indexStart, int indexEnd)
{
	CheckConnection();

	auto message = g_client->CreateMessage(PacketIdentifier::SAMP_Dialog_SetCursorPosition2);
	message->GetInput().WriteInteger(indexStart);
	message->GetInput().WriteInteger(indexEnd);
	g_client->Write(message);
}

DllPublic int API::SAMP::Dialog::GetID()
{
	CheckConnection(-1);

	return g_client->FastReadInteger(PacketIdentifier::SAMP_Dialog_GetId);
}

DllPublic int API::SAMP::Dialog::GetStringCount()
{
	CheckConnection(-1);

	return g_client->FastReadInteger(PacketIdentifier::SAMP_Dialog_GetStringCount);
}

DllPublic void API::SAMP::Dialog::GetStringByIndex(int index, wchar_t * text, int length)
{
	CheckConnection();

	auto message = g_client->CreateMessage(PacketIdentifier::SAMP_Dialog_GetStringByIndex);
	message->GetInput().WriteInteger(index);
	g_client->Write(message);

	CheckValidation(message);

	auto t = message->GetResponse()->ReadString();
	wcscpy_s(text, length, t.c_str());
}

DllPublic bool API::SAMP::Dialog::IsOpen()
{
	CheckConnection(false);

	return g_client->FastReadBoolean(PacketIdentifier::SAMP_Dialog_IsOpen);
}

DllPublic void API::SAMP::Dialog::Close(int reason)
{
	CheckConnection();

	g_client->FastWriteInteger(PacketIdentifier::SAMP_Dialog_Close, reason);
}

DllPublic void API::SAMP::Dialog::SelectIndex(int index)
{
	CheckConnection();

	g_client->FastWriteInteger(PacketIdentifier::SAMP_Dialog_SelectIndex, index);
}

DllPublic void API::SAMP::Dialog::BlockGetCaption(wchar_t * caption, int length)
{
	CheckConnection();

	auto message = g_client->FastReadString(PacketIdentifier::SAMP_Dialog_BlockGetCaption);
	wcscpy_s(caption, length, message.c_str());
}

DllPublic void API::SAMP::Dialog::BlockGetText(wchar_t * text, int length)
{
	CheckConnection();

	auto message = g_client->FastReadString(PacketIdentifier::SAMP_Dialog_BlockGetText);
	wcscpy_s(text, length, message.c_str());
}

DllPublic void API::SAMP::Dialog::Block(int id, const wchar_t * text)
{
	CheckConnection();

	auto message = g_client->CreateMessage(PacketIdentifier::SAMP_Dialog_Block);
	message->GetInput().WriteInteger(id);
	message->GetInput().WriteString(text);
	g_client->Write(message);
}

DllPublic bool API::SAMP::Dialog::BlockHasBlockedDialog()
{
	CheckConnection(false);

	return g_client->FastReadBoolean(PacketIdentifier::SAMP_Dialog_BlockHasBlockedDialog);
}

DllPublic bool API::SAMP::Dialog::BlockHasNeedBlocking()
{
	CheckConnection(false);

	return g_client->FastReadBoolean(PacketIdentifier::SAMP_Dialog_BlockHasNeedBlocking);
}

DllPublic void API::SAMP::Player::GetLocalName(wchar_t * name, int length)
{
	CheckConnection();

	auto message = g_client->FastReadString(PacketIdentifier::SAMP_Player_GetLocalName);
	if (message.length() > 0)
		wcscpy_s(name, length, message.c_str());
}

DllPublic int API::SAMP::Player::GetLocalId()
{
	CheckConnection(-1);

	return g_client->FastReadInteger(PacketIdentifier::SAMP_Player_GetLocalId);
}

DllPublic void API::SAMP::Player::GetNameById(int id, wchar_t * name, int length)
{
	CheckConnection();

	auto message = g_client->CreateMessage(PacketIdentifier::SAMP_Player_GetNameById);
	message->GetInput().WriteInteger(id);
	g_client->Write(message);

	CheckValidation(message);

	auto s = message->GetResponse()->ReadString();
	if (s.length() > 0)
		wcscpy_s(name, length, s.c_str());
}

DllPublic int API::SAMP::Player::GetIdByName(const wchar_t * name)
{
	CheckConnection(-1);

	auto message = g_client->CreateMessage(PacketIdentifier::SAMP_Player_GetIdByName);
	message->GetInput().WriteString(name);
	g_client->Write(message);

	CheckValidation(message, -1);

	return message->GetResponse()->ReadInteger();
}

DllPublic void API::SAMP::Player::GetFullName(const wchar_t * name, wchar_t * fullName, int length)
{
	CheckConnection();

	auto message = g_client->CreateMessage(PacketIdentifier::SAMP_Player_GetFullName);
	message->GetInput().WriteString(name);
	g_client->Write(message);

	CheckValidation(message);

	auto s = message->GetResponse()->ReadString();
	if (s.length() > 0)
		wcscpy_s(fullName, length, s.c_str());
}

DllPublic void API::SAMP::Vehicle::GetNumberplate(wchar_t * numberplate, int length)
{
	auto s = g_client->FastReadString(PacketIdentifier::SAMP_Vehicle_GetNumberplate);
	if (s.length() > 0)
		wcscpy_s(numberplate, length, s.c_str());
}

DllPublic void API::SAMP::Vehicle::ToggleSiren(bool state)
{
	CheckConnection();

	g_client->FastWriteBoolean(PacketIdentifier::SAMP_Vehicle_ToggleSiren, state);
}

DllPublic int API::Player::GetHealth()
{
	CheckConnection(-1);

	return g_client->FastReadInteger(PacketIdentifier::Player_GetHealth);
}

DllPublic int API::Player::GetArmor()
{
	CheckConnection(-1);

	return g_client->FastReadInteger(PacketIdentifier::Player_GetArmor);
}

DllPublic int API::Player::GetInteriorId()
{
	CheckConnection(-1);

	return g_client->FastReadInteger(PacketIdentifier::Player_GetInteriorId);
}

DllPublic void API::Player::GetCity(wchar_t * city, int length)
{
	CheckConnection();

	float x = g_client->FastReadFloat(PacketIdentifier::Player_GetX);
	float y = g_client->FastReadFloat(PacketIdentifier::Player_GetY);
	float z = g_client->FastReadFloat(PacketIdentifier::Player_GetZ);

	auto c = ZoneManager::GetInstance().FindCity(x, y, z);
	if (c.length() > 0)
		wcscpy_s(city, length, c.c_str());
}

DllPublic void API::Player::GetDistrict(wchar_t * district, int length)
{
	CheckConnection();

	float x = g_client->FastReadFloat(PacketIdentifier::Player_GetX);
	float y = g_client->FastReadFloat(PacketIdentifier::Player_GetY);
	float z = g_client->FastReadFloat(PacketIdentifier::Player_GetZ);

	auto d = ZoneManager::GetInstance().FindZone(x, y, z);
	if (d.length() > 0)
		wcscpy_s(district, length, d.c_str());
}

DllPublic void API::Player::GetPosition(float & x, float & y, float & z)
{
	CheckConnection();

	x = g_client->FastReadFloat(PacketIdentifier::Player_GetX);
	y = g_client->FastReadFloat(PacketIdentifier::Player_GetY);
	z = g_client->FastReadFloat(PacketIdentifier::Player_GetZ);
}

DllPublic float API::Player::GetYaw()
{
	CheckConnection(0.0f);

	return g_client->FastReadFloat(PacketIdentifier::Player_GetYaw);
}

DllPublic bool API::Player::InInterior()
{
	CheckConnection(false);

	return g_client->FastReadBoolean(PacketIdentifier::Player_InInterior);
}

DllPublic bool API::Player::InVehicle()
{
	CheckConnection(false);

	return g_client->FastReadBoolean(PacketIdentifier::Player_InVehicle);
}

DllPublic bool API::Player::IsDriver()
{
	CheckConnection(false);

	return g_client->FastReadBoolean(PacketIdentifier::Player_IsDriver);
}

DllPublic bool API::Player::IsFreezed()
{
	CheckConnection(false);

	return g_client->FastReadBoolean(PacketIdentifier::Player_IsFreezed);
}

DllPublic float API::Vehicle::GetSpeed()
{
	CheckConnection(0.0f);

	return g_client->FastReadFloat(PacketIdentifier::Vehicle_GetSpeed);
}

DllPublic float API::Vehicle::GetHealth()
{
	CheckConnection(0.0f);

	return g_client->FastReadFloat(PacketIdentifier::Vehicle_GetHealth);
}

DllPublic int API::Vehicle::GetModelID()
{
	CheckConnection(-1);

	return g_client->FastReadInteger(PacketIdentifier::Vehicle_GetModelId);
}

DllPublic bool API::Vehicle::IsLightActive()
{
	CheckConnection(false);

	return g_client->FastReadBoolean(PacketIdentifier::Vehicle_IsLightActive);
}

DllPublic bool API::Vehicle::IsLocked()
{
	CheckConnection(false);

	return g_client->FastReadBoolean(PacketIdentifier::Vehicle_IsLocked);
}

DllPublic bool API::Vehicle::IsEngineRunning()
{
	CheckConnection(false);

	return g_client->FastReadBoolean(PacketIdentifier::Vehicle_IsEngineRunning);
}

DllPublic bool API::Vehicle::UseHorn()
{
	CheckConnection(false);

	return g_client->FastReadBoolean(PacketIdentifier::Vehicle_UseHorn);
}

DllPublic bool API::Vehicle::UseSiren()
{
	CheckConnection(false);

	return g_client->FastReadBoolean(PacketIdentifier::Vehicle_UseSiren);
}