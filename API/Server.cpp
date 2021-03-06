#include "stdafx.h"
#include "Server.h"
#include "OverlayManager.h"
#include "SerializeableQueue.h"
#include "ServerAPIGeneral.h"
#include "ServerAPIPlayer.h"
#include "ServerAPIVehicle.h"
#include "ServerAPIOverlayText.h"
#include "ServerAPIOverlayBox.h"
#include "ServerAPISAMPPlayer.h"
#include "ServerAPISAMPVehicle.h"
#include "ServerAPISAMPChat.h"
#include "ServerAPISAMPDialog.h"

#define AddFunction(identifier, func) _functions.push_back(std::make_pair<PacketIdentifier, std::function<void(int, SerializeableQueue&, SerializeableQueue&)>>(identifier, &func))

Server::Server()
{
	RegisterFunctions();

	_readThreads = std::vector<std::unique_ptr<std::thread>>();
	_incomingThread = std::thread(&Server::WaitForClient, this);

	_clientId = 0;
}

Server::~Server()
{
}

void Server::RegisterFunctions()
{
	AddFunction(PacketIdentifier::Useless, ServerAPIGeneral::Useless);
	AddFunction(PacketIdentifier::Useless1, ServerAPIGeneral::Useless1);
	AddFunction(PacketIdentifier::General_GetWeatherId, ServerAPIGeneral::GetWeatherId);
	AddFunction(PacketIdentifier::General_SetKey, ServerAPIGeneral::SetKey);

	// Player
	AddFunction(PacketIdentifier::Player_GetHealth, ServerAPIPlayer::GetHealth);
	AddFunction(PacketIdentifier::Player_GetArmor, ServerAPIPlayer::GetArmor);
	AddFunction(PacketIdentifier::Player_GetInteriorId, ServerAPIPlayer::GetInteriorId);
	AddFunction(PacketIdentifier::Player_GetYaw, ServerAPIPlayer::GetYaw);
	AddFunction(PacketIdentifier::Player_GetX, ServerAPIPlayer::GetX);
	AddFunction(PacketIdentifier::Player_GetY, ServerAPIPlayer::GetY);
	AddFunction(PacketIdentifier::Player_GetZ, ServerAPIPlayer::GetZ);
	AddFunction(PacketIdentifier::Player_GetPosition, ServerAPIPlayer::GetPosition);
	AddFunction(PacketIdentifier::Player_InInterior, ServerAPIPlayer::InInterior);
	AddFunction(PacketIdentifier::Player_InVehicle, ServerAPIPlayer::InVehicle);
	AddFunction(PacketIdentifier::Player_IsDriver, ServerAPIPlayer::IsDriver);
	AddFunction(PacketIdentifier::Player_IsFreezed, ServerAPIPlayer::IsFreezed);

	// Vehicle
	AddFunction(PacketIdentifier::Vehicle_GetSpeed, ServerAPIVehicle::GetSpeed);
	AddFunction(PacketIdentifier::Vehicle_GetHealth, ServerAPIVehicle::GetHealth);
	AddFunction(PacketIdentifier::Vehicle_GetModelId, ServerAPIVehicle::GetModelId);
	AddFunction(PacketIdentifier::Vehicle_IsLightActive, ServerAPIVehicle::IsLightActive);
	AddFunction(PacketIdentifier::Vehicle_IsLocked, ServerAPIVehicle::IsLocked);
	AddFunction(PacketIdentifier::Vehicle_IsEngineRunning, ServerAPIVehicle::IsEngineRunning);
	AddFunction(PacketIdentifier::Vehicle_UseHorn, ServerAPIVehicle::UseHorn);
	AddFunction(PacketIdentifier::Vehicle_UseSiren, ServerAPIVehicle::UseSiren);

	// Overlay::Text
	AddFunction(PacketIdentifier::Overlay_Text_Create, ServerAPIOverlayText::Create);
	AddFunction(PacketIdentifier::Overlay_Text_Delete, ServerAPIOverlayText::Delete);
	AddFunction(PacketIdentifier::Overlay_Text_SetColor, ServerAPIOverlayText::SetColor);
	AddFunction(PacketIdentifier::Overlay_Text_SetX, ServerAPIOverlayText::SetX);
	AddFunction(PacketIdentifier::Overlay_Text_SetY, ServerAPIOverlayText::SetY);
	AddFunction(PacketIdentifier::Overlay_Text_SetMaxWidth, ServerAPIOverlayText::SetMaxWidth);
	AddFunction(PacketIdentifier::Overlay_Text_SetMaxHeight, ServerAPIOverlayText::SetMaxHeight);
	AddFunction(PacketIdentifier::Overlay_Text_SetActive, ServerAPIOverlayText::SetActive);
	AddFunction(PacketIdentifier::Overlay_Text_SetText, ServerAPIOverlayText::SetText);
	AddFunction(PacketIdentifier::Overlay_Text_SetSize, ServerAPIOverlayText::SetSize);
	AddFunction(PacketIdentifier::Overlay_Text_SetUseMaxWidth, ServerAPIOverlayText::SetUseMaxWidth);
	AddFunction(PacketIdentifier::Overlay_Text_SetUseMaxHeight, ServerAPIOverlayText::SetUseMaxHeight);
	AddFunction(PacketIdentifier::Overlay_Text_GetTextExtent, ServerAPIOverlayText::GetExtent);

	// Overlay::Box
	AddFunction(PacketIdentifier::Overlay_Box_Create, ServerAPIOverlayBox::Create);
	AddFunction(PacketIdentifier::Overlay_Box_Delete, ServerAPIOverlayBox::Delete);
	AddFunction(PacketIdentifier::Overlay_Box_SetColor, ServerAPIOverlayBox::SetColor);
	AddFunction(PacketIdentifier::Overlay_Box_SetX, ServerAPIOverlayBox::SetX);
	AddFunction(PacketIdentifier::Overlay_Box_SetY, ServerAPIOverlayBox::SetY);
	AddFunction(PacketIdentifier::Overlay_Box_SetWidth, ServerAPIOverlayBox::SetWidth);
	AddFunction(PacketIdentifier::Overlay_Box_SetHeight, ServerAPIOverlayBox::SetHeight);
	AddFunction(PacketIdentifier::Overlay_Box_SetActive, ServerAPIOverlayBox::SetActive);

	// SAMP::Player
	AddFunction(PacketIdentifier::SAMP_Player_GetLocalName, ServerAPISAMPPlayer::GetLocalName);
	AddFunction(PacketIdentifier::SAMP_Player_GetLocalId, ServerAPISAMPPlayer::GetLocalId);
	AddFunction(PacketIdentifier::SAMP_Player_GetNameById, ServerAPISAMPPlayer::GetNameById);
	AddFunction(PacketIdentifier::SAMP_Player_GetIdByName, ServerAPISAMPPlayer::GetIdByName);
	AddFunction(PacketIdentifier::SAMP_Player_GetFullName, ServerAPISAMPPlayer::GetFullName);

	// SAMP::Vehicle
	AddFunction(PacketIdentifier::SAMP_Vehicle_GetNumberplate, ServerAPISAMPVehicle::GetNumberplate);
	AddFunction(PacketIdentifier::SAMP_Vehicle_ToggleSiren, ServerAPISAMPVehicle::ToggleSiren);

	// SAMP::Chat
	AddFunction(PacketIdentifier::SAMP_Chat_Send, ServerAPISAMPChat::Send);
	AddFunction(PacketIdentifier::SAMP_Chat_AddMessage1, ServerAPISAMPChat::AddMessage1);
	AddFunction(PacketIdentifier::SAMP_Chat_AddMessage2, ServerAPISAMPChat::AddMessage2);
	AddFunction(PacketIdentifier::SAMP_Chat_GetText, ServerAPISAMPChat::GetText);
	AddFunction(PacketIdentifier::SAMP_Chat_SetText, ServerAPISAMPChat::SetText);
	AddFunction(PacketIdentifier::SAMP_Chat_GetBufferMessage, ServerAPISAMPChat::GetBufferMessage);
	AddFunction(PacketIdentifier::SAMP_Chat_AddBufferMessage, ServerAPISAMPChat::AddBufferMessage);
	AddFunction(PacketIdentifier::SAMP_Chat_SetCursorPosition1, ServerAPISAMPChat::SetCursorPosition);
	AddFunction(PacketIdentifier::SAMP_Chat_SetCursorPosition2, ServerAPISAMPChat::SetCursorPosition2);
	AddFunction(PacketIdentifier::SAMP_Chat_Toggle, ServerAPISAMPChat::Toggle);
	AddFunction(PacketIdentifier::SAMP_Chat_Clear, ServerAPISAMPChat::Clear);
	AddFunction(PacketIdentifier::SAMP_Chat_IsOpen, ServerAPISAMPChat::IsOpen);

	// SAMP::Dialog
	AddFunction(PacketIdentifier::SAMP_Dialog_GetText, ServerAPISAMPDialog::GetText);
	AddFunction(PacketIdentifier::SAMP_Dialog_SetText, ServerAPISAMPDialog::SetText);
	AddFunction(PacketIdentifier::SAMP_Dialog_SetCursorPosition1, ServerAPISAMPDialog::SetCursorPosition1);
	AddFunction(PacketIdentifier::SAMP_Dialog_SetCursorPosition2, ServerAPISAMPDialog::SetCursorPosition2);
	AddFunction(PacketIdentifier::SAMP_Dialog_GetId, ServerAPISAMPDialog::GetId);
	AddFunction(PacketIdentifier::SAMP_Dialog_GetStringCount, ServerAPISAMPDialog::GetStringCount);
	AddFunction(PacketIdentifier::SAMP_Dialog_GetStringByIndex, ServerAPISAMPDialog::GetStringByIndex);
	AddFunction(PacketIdentifier::SAMP_Dialog_Clear, ServerAPISAMPDialog::Clear);
	AddFunction(PacketIdentifier::SAMP_Dialog_Close, ServerAPISAMPDialog::Close);
	AddFunction(PacketIdentifier::SAMP_Dialog_SelectIndex, ServerAPISAMPDialog::SelectIndex);
	AddFunction(PacketIdentifier::SAMP_Dialog_IsOpen, ServerAPISAMPDialog::IsOpen);
	AddFunction(PacketIdentifier::SAMP_Dialog_BlockGetCaption, ServerAPISAMPDialog::BlockGetCaption);
	AddFunction(PacketIdentifier::SAMP_Dialog_BlockGetText, ServerAPISAMPDialog::BlockGetText);
	AddFunction(PacketIdentifier::SAMP_Dialog_Block, ServerAPISAMPDialog::Block);
	AddFunction(PacketIdentifier::SAMP_Dialog_BlockHasBlockedDialog, ServerAPISAMPDialog::BlockHasBlockedDialog);
	AddFunction(PacketIdentifier::SAMP_Dialog_BlockHasNeedBlocking, ServerAPISAMPDialog::BlockHasNeedBlocking);
}

void Server::ReadThread(HANDLE pipe)
{
	int clientId = GenerateClientId();
	while (true)
	{
		BYTE buffer[512 * 128] = {};
		DWORD bytesRead = 0;

		bool success = ReadFile(pipe, buffer, 512 * 128, &bytesRead, 0);

		if (!success || bytesRead == 0)
		{
			if (GetLastError() == ERROR_MORE_DATA)
				continue;

			if (GetLastError() == ERROR_BROKEN_PIPE)
			{
				FlushFileBuffers(pipe);
				DisconnectNamedPipe(pipe);
				CloseHandle(pipe);
				
				OverlayManager::GetInstance().Cleanup(clientId);
				return;
			}
		}

		std::vector<BYTE> bytes;
		for (size_t i = 0; i < bytesRead; i++)
			bytes.push_back(buffer[i]);

		auto in = SerializeableQueue(bytes);
		auto out = SerializeableQueue();

		int id = in.ReadInteger();
		PacketIdentifier identifier = (PacketIdentifier)in.ReadInteger();

		out.WriteInteger(id);

		// Call API function
		for (size_t i = 0; i < _functions.size(); i++)
		{
			auto pair = _functions[i];
			if (pair.first == identifier)
			{
				pair.second(clientId, in, out);
				break;
			}
		}

		// Return
		auto data = out.GetData();
		DWORD bytesWritten = 0;
		WriteFile(pipe, data.data(), data.size(), &bytesWritten, 0);
	}
}

void Server::WaitForClient()
{
	while (true)
	{
		bool connected = false;
		while (!connected)
		{
			HANDLE pipe = CreateNamedPipeW(L"\\\\.\\pipe\\EBIP0", PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, PIPE_UNLIMITED_INSTANCES, 512 * 128, 512 * 128, 0, 0);
			connected = ConnectNamedPipe(pipe, 0) ? true : (GetLastError() == ERROR_PIPE_CONNECTED);
			if (connected)
				_readThreads.push_back(std::make_unique<std::thread>(&Server::ReadThread, this, pipe));
		}
	}
}

int Server::GenerateClientId()
{
	std::lock_guard<std::mutex> guard(_clientMutex);
	return _clientId++;
}
