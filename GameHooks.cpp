#include "GameHooks.h"
#include "MinHook/include/MinHook.h"
#include <sstream>
#include <ctime>

extern ofstream log_file;
extern string to_upper(string& in);
template <class T>
string to_string(T t, ios_base& (*f)(ios_base&))
{
	ostringstream oss;
	oss << f << t;
	return oss.str();
}

typedef int64_t(__fastcall* RID)(int64_t rcx, netPlayerData* rdx);
RID og_rid;
int64_t __fastcall hk_rid(int64_t rcx, netPlayerData* rdx)
{
	const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

	try
	{
		if (!rdx)
			return og_rid(rcx, rdx);
		if (rdx->m_rockstar_id == 0)
		{
			rdx->m_rockstar_id = 1337;
			SetConsoleTextAttribute(h_console, FOREGROUND_RED);
			fmt::print("Blocked RID 0 crash from {}\n", rdx->m_name);
			SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}

		return og_rid(rcx, rdx);
	}
	catch(...)
	{
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		fmt::print(
			"Exception/crash occurred in {}, will attempt to call original function\nFunc args: {}, {}\n",
			__FUNCTION__, rcx, reinterpret_cast<void*>(rdx));
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		return og_rid(rcx, rdx);
	}
}

typedef void(__fastcall* PJE)(__int64 rcx, CNetGamePlayer* net_player);
PJE og_pje;
void __fastcall hk_pje(__int64 rcx, CNetGamePlayer* net_player)
{
	if (net_player == nullptr)
		return og_pje(rcx, net_player);
	if(net_player->GetNetData() == nullptr)
		return og_pje(rcx, net_player);
	//player_list.add_net_player(net_player);
	const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);
	try
	{
		time_t now = time(0);
		tm* ltm = localtime(&now);
		SetConsoleTextAttribute(h_console, FOREGROUND_BLUE | FOREGROUND_RED);
		auto str = fmt::format("[{}:{}:{}] Joining player: {}:{} taking slot {}:{}\n",
			ltm->tm_hour, ltm->tm_min, ltm->tm_sec, net_player->GetName(), net_player->GetNetData()->m_rockstar_id, net_player->m_ActivePlayerID, net_player->m_PlayerID);
		log_file << str;
		log_file.flush();
		fmt::print("[{}:{}:{}] Joining player: {}:{} taking slot {}:{}\n",
			ltm->tm_hour, ltm->tm_min, ltm->tm_sec, net_player->GetName(), net_player->GetNetData()->m_rockstar_id, net_player->m_ActivePlayerID, net_player->m_PlayerID);
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		return og_pje(rcx, net_player);
	}
	catch (...)
	{
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		fmt::print(
			"Exception/crash occurred in {}, will attempt to call original function\nFunc args: {}, {}\n",
			__FUNCTION__, rcx, reinterpret_cast<void*>(net_player));
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		return og_pje(rcx, net_player);
	}
}

typedef void(__fastcall* PLE)(__int64 rcx, CNetGamePlayer* net_player);
PLE og_ple;
void __fastcall hk_ple(__int64 rcx, CNetGamePlayer* net_player)
{
	if (net_player == nullptr)
		return og_ple(rcx, net_player);
	if (net_player->GetNetData() == nullptr)
		return og_ple(rcx, net_player);
	const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);
	try
	{
		//player_list.remove_net_player(net_player);
		time_t now = time(0);
		tm* ltm = localtime(&now);
		SetConsoleTextAttribute(h_console, FOREGROUND_BLUE | FOREGROUND_RED);
		auto str = fmt::format("[{}:{}:{}] Leaving player: {}:{} freeing slot {}:{}\n",
			ltm->tm_hour, ltm->tm_min, ltm->tm_sec, net_player->GetName(), net_player->GetNetData()->m_rockstar_id, net_player->m_ActivePlayerID, net_player->m_PlayerID);
		log_file << str;
		log_file.flush();
		fmt::print("[{}:{}:{}] Leaving player: {}:{} freeing slot {}:{}\n",
			ltm->tm_hour, ltm->tm_min, ltm->tm_sec, net_player->GetName(), net_player->GetNetData()->m_rockstar_id, net_player->m_ActivePlayerID, net_player->m_PlayerID);
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		return og_ple(rcx, net_player);
	}
	catch (...)
	{
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		fmt::print(
			"Exception/crash occurred in {}, will attempt to call original function\nFunc args: {}, {}\n",
			__FUNCTION__, rcx, reinterpret_cast<void*>(net_player));
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		return og_ple(rcx, net_player);
	}
}

typedef void(__fastcall* SWS)(void* thisptr, datBitBuffer* buffer, CNetGamePlayer* sender, CNetGamePlayer* receiver);
SWS og_sws;
void __fastcall hk_sws(void* thisptr, datBitBuffer* buffer, CNetGamePlayer* sender, CNetGamePlayer* receiver)
{
	const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

	try
	{
		if (sender == nullptr || receiver == nullptr)
			return og_sws(thisptr, buffer, sender, receiver);

		if(buffer == nullptr)
			return og_sws(thisptr, buffer, sender, receiver);
		
		DWORD event_48{};
		if(!buffer->ReadDword(&event_48, 4))
		{
			SetConsoleTextAttribute(h_console, FOREGROUND_BLUE | FOREGROUND_RED);
			fmt::print("Failed to read args from buffer\n");
			SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			return og_sws(thisptr, buffer, sender, receiver);
		}
		
		if (!event_48)
			return;
		DWORD arg{};
		if(!buffer->ReadDword(&arg, 1))
		{
			SetConsoleTextAttribute(h_console, FOREGROUND_BLUE | FOREGROUND_RED);
			fmt::print("Failed to read args from buffer\n");
			SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			return og_sws(thisptr, buffer, sender, receiver);
		}
		
		SetConsoleTextAttribute(h_console, FOREGROUND_BLUE | FOREGROUND_RED);
		fmt::print("Received net event SCRIPT_WORLD_STATE_EVENT ({}) from {}:{} (PID: {}) with arg {}\n",
			(static_cast<bool>(arg) ? "changing" : "reverting"),
			sender->GetName(), sender->GetNetData()->m_rockstar_id, sender->m_PlayerID, static_cast<bool>(arg));
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		return og_sws(thisptr, buffer, sender, receiver);
	}
	catch (...)
	{
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		fmt::print(
			"Exception/crash occurred in {}, will attempt to call original function\nFunc args: {}, {}, {}\n",
			__FUNCTION__, reinterpret_cast<void*>(buffer), reinterpret_cast<void*>(sender), reinterpret_cast<void*>(receiver));
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		return og_sws(thisptr, buffer, sender, receiver);
	}
}

typedef bool(__fastcall* RDE)(__int64 thisptr, CNetGamePlayer* sender, CNetGamePlayer* receiver);
RDE og_rde;
bool __fastcall hk_rde(__int64 thisptr, CNetGamePlayer* sender, CNetGamePlayer* receiver)
{
	const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

	try
	{
		if (sender == nullptr || receiver == nullptr)
			return og_rde(thisptr, sender, receiver);
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		fmt::print(
			"Received net event {} from {}:{} (PID: {}) - event + 48: {} - event + 52: {} - event + 120: {} - event + 121: {}\n", reinterpret_cast<rage::netGameEvent*>(thisptr)->get_name(), sender->GetName(),
			sender->GetNetData()->m_rockstar_id, sender->m_PlayerID,
			*reinterpret_cast<DWORD*>(thisptr + 48), *reinterpret_cast<DWORD*>(thisptr + 52), *reinterpret_cast<BYTE*>(thisptr + 120), *reinterpret_cast<BYTE*>(thisptr + 121));
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		return og_rde(thisptr, sender, receiver);
	}
	catch (...)
	{
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		fmt::print(
			"Exception/crash occurred in {}, will attempt to call original function\nFunc args: {}, {}\n",
			__FUNCTION__, reinterpret_cast<void*>(sender), reinterpret_cast<void*>(receiver));
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		return og_rde(thisptr, sender, receiver);
	}
}

typedef bool(__fastcall* CPT)(__int64 thisptr, CNetGamePlayer* sender, CNetGamePlayer* receiver);
CPT og_cpt;
bool __fastcall hk_cpt(__int64 thisptr, CNetGamePlayer* sender, CNetGamePlayer* receiver)
{
	const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

	try
	{
		if(sender == nullptr || receiver == nullptr)
			return og_cpt(thisptr, sender, receiver);
		if (menu.clear_ped_tasks)
		{
			SetConsoleTextAttribute(h_console, FOREGROUND_RED);
			fmt::print(
				"Received net event CLEAR_PED_TASK from {}:{} (PID: {})", sender->GetName(),
				sender->GetNetData()->m_rockstar_id, sender->m_PlayerID);
			SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			return true;
		}

		return og_cpt(thisptr, sender, receiver);
	}
	catch (...)
	{
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		fmt::print(
			"Exception/crash occurred in {}, will attempt to call original function\nFunc args: {}, {}\n",
			__FUNCTION__, reinterpret_cast<void*>(sender), reinterpret_cast<void*>(receiver));
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		return og_cpt(thisptr, sender, receiver);
	}
}

typedef void(__fastcall* RWE)(__int64 thisptr, datBitBuffer* buffer, CNetGamePlayer* sender, CNetGamePlayer* receiver);
RWE og_rwe;
void __fastcall hk_rwe(__int64 thisptr, datBitBuffer* buffer, CNetGamePlayer* sender, CNetGamePlayer* receiver)
{
	const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

	try
	{
		DWORD event_48{};
		if(!buffer->ReadDword(&event_48, 13))
		{
			SetConsoleTextAttribute(h_console, FOREGROUND_BLUE | FOREGROUND_RED);
			fmt::print("Failed to read args from buffer\n");
			SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			return og_rwe(thisptr, buffer, sender, receiver);
		}
		
		DWORD weapon_hash{};
		if(!buffer->ReadDword(&weapon_hash, 32))
		{
			SetConsoleTextAttribute(h_console, FOREGROUND_BLUE | FOREGROUND_RED);
			fmt::print("Failed to read args from buffer\n");
			SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			return og_rwe(thisptr, buffer, sender, receiver);
		}
		
		if (static_cast<int32_t>(weapon_hash) == NULL)
		{
			fmt::print("Received net event REMOVE_WEAPON_EVENT crash from {}:{} (PID: {}), tried removing invalid weapon\n",
				sender->GetName(), sender->GetNetData()->m_rockstar_id, sender->m_PlayerID);
			return;
		}

		if(menu.remove_weapon)
		{
			SetConsoleTextAttribute(h_console, FOREGROUND_RED);
			fmt::print("Received net event REMOVE_WEAPON_EVENT from {}:{} (PID: {}), tried removing {}\n",
				sender->GetName(), sender->GetNetData()->m_rockstar_id, sender->m_PlayerID, static_cast<int32_t>(weapon_hash));
			SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			return;
		}

		buffer->Seek(0);
		return og_rwe(thisptr, buffer, sender, receiver);
	}
	catch (...)
	{
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		fmt::print(
			"Exception/crash occurred in {}, will attempt to call original function\nFunc args: {}, {}, {}\n",
			__FUNCTION__, reinterpret_cast<void*>(buffer), reinterpret_cast<void*>(sender), reinterpret_cast<void*>(receiver));
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		return hk_rwe(thisptr, buffer, sender, receiver);
	}
}

typedef bool(__fastcall* CSGE)(datBitBuffer* bitbuffer, CNetGamePlayer* sender);
CSGE og_csge;
bool __fastcall hk_csge(datBitBuffer* buffer, CNetGamePlayer* sender)
{
	const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

	try
	{
		if (!sender)
			return og_csge(buffer, sender);

		// Read amount of args
		DWORD number_of_args{};
		if(!buffer->ReadDword(&number_of_args, 0x20))
		{
			SetConsoleTextAttribute(h_console, FOREGROUND_BLUE | FOREGROUND_RED);
			fmt::print("Failed to read amount of args from buffer\n");
			SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			return og_csge(buffer, sender);
		}

		if (number_of_args > 0x1AF)
			return true;
		int number_of_args_normalised = number_of_args / 8;
		// Grab args list
		vector<DWORD> args_list;
		for(int i = 0; i < number_of_args_normalised; i++)
		{
			DWORD tmp_arg{};
			if (!buffer->ReadDword(&tmp_arg, 32))
			{
				SetConsoleTextAttribute(h_console, FOREGROUND_BLUE | FOREGROUND_RED);
				fmt::print("Failed to read amount of args from buffer\n");
				SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				//return og_csge(buffer, sender);
				break;
			}

			args_list.emplace_back(tmp_arg);
		}

		DWORD id = args_list.at(0);
		string args_list_string;
		time_t now = time(0);
		tm* ltm = localtime(&now);
		args_list_string += fmt::format("[{}:{}:{}] Script Event {}:0x{} From {}:{} (PID: {})\n",
			(ltm->tm_hour < 10 ? string("0" + to_string(ltm->tm_hour)) : to_string(ltm->tm_hour)),
			(ltm->tm_min < 10 ? string("0" + to_string(ltm->tm_min)) : to_string(ltm->tm_min)),
			(ltm->tm_sec < 10 ? string("0" + to_string(ltm->tm_sec)) : to_string(ltm->tm_sec)),
			static_cast<int32_t>(id), to_upper(to_string<long>(static_cast<int32_t>(id), hex)), sender->GetName(),
			sender->GetNetData()->m_rockstar_id, sender->m_PlayerID);
		for (int i = 1; i < args_list.size(); ++i)
			args_list_string += fmt::format("Arg[{}]: {}\n", i, args_list.at(i));
		if (menu.log_to_console)
		{
			SetConsoleTextAttribute(h_console, FOREGROUND_BLUE | FOREGROUND_RED);
			fmt::print(args_list_string);
			SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}

		if (menu.se_logging)
		{
			log_file << args_list_string << "\n";
			log_file.flush();
		}

		// Block bad script events
		switch (static_cast<int32_t>(id))
		{
			// Block (invalid) apartment invites
		case -1253256204:
			if (menu.apartment_invite)
			{
				SetConsoleTextAttribute(h_console, FOREGROUND_RED);
				fmt::print("Blocked apartment invite from {}", sender->GetName());
				SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				return true;
			}

			if (menu.invalid_apartment)
			{
				if (args_list.at(5) < 1 || args_list.at(5) > 114)
				{
					SetConsoleTextAttribute(h_console, FOREGROUND_RED);
					fmt::print("Blocked invalid apartment invite from {} with invalid arg {}", sender->GetName(), args_list.at(5));
					SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
					return true;
				}
			}

			//break;
		// Block transaction error screens
		case -881249731:
			if (menu.transaction_error)
			{
				SetConsoleTextAttribute(h_console, FOREGROUND_RED);
				fmt::print("Blocked transaction error screen from {}", sender->GetName());
				SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				return true;
			}

			//break;
		// Block remote kill vehicle engine
		case 472658729:
			if (menu.kill_engine)
			{
				SetConsoleTextAttribute(h_console, FOREGROUND_RED);
				fmt::print("Blocked remote  kill vehicle engine from {}", sender->GetName());
				SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				return true;
			}

			//break;
		// Block remote vehicle kick
		case -130330100:
			if (menu.vehicle_kick)
			{
				SetConsoleTextAttribute(h_console, FOREGROUND_RED);
				fmt::print("Blocked remote vehicle kick from {}", sender->GetName());
				SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				return true;
			}

			//break;
		// Block remote off radar
		case -1363050950:
			if (menu.off_radar)
			{
				SetConsoleTextAttribute(h_console, FOREGROUND_RED);
				fmt::print("Blocked remote off radar from {}", sender->GetName());
				SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				return true;
			}

			//break;
		// Block remote CEO kick
		case -1775205375:
			if (menu.ceo)
			{
				SetConsoleTextAttribute(h_console, FOREGROUND_RED);
				fmt::print("Blocked remote CEO kick from {}", sender->GetName());
				SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				return true;
			}

			//break;
		// Block remote CEO ban
		case -1028825138:
			if (menu.ceo)
			{
				SetConsoleTextAttribute(h_console, FOREGROUND_RED);
				fmt::print("Blocked remote CEO ban from {}", sender->GetName());
				SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				return true;
			}

			//break;
		// Block sound spam 1
		case 1337731455:
			if (menu.sound_spam)
			{
				SetConsoleTextAttribute(h_console, FOREGROUND_RED);
				fmt::print("Blocked sound spam 1 from {}", sender->GetName());
				SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				return true;
			}

			//break;
		// Block sound spam 2
		case 323449272:
			if (menu.sound_spam)
			{
				SetConsoleTextAttribute(h_console, FOREGROUND_RED);
				fmt::print("Blocked sound spam 2 from {}", sender->GetName());
				SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				return true;
			}

			//break;
		// Block remote wanted level
		case 301606138:
			if (menu.wanted_level)
			{
				SetConsoleTextAttribute(h_console, FOREGROUND_RED);
				fmt::print("Blocked remote wanted level from {}", sender->GetName());
				SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				return true;
			}

			//break;
		// Block force to mission
		case 1764541627:
			if (menu.mission)
			{
				SetConsoleTextAttribute(h_console, FOREGROUND_RED);
				fmt::print("Blocked force to mission from {}", sender->GetName());
				SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				return true;
			}

			//break;
		// Block (modded) bounties
		case 544453591:
			if (menu.bounties)
			{
				SetConsoleTextAttribute(h_console, FOREGROUND_RED);
				fmt::print("Blocked bounty from {} with value ${}", sender->GetName(), args_list.at(4));
				SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				return true;
			}

			if (menu.modded_bounty)
			{
				switch (args_list.at(4))
				{
				case 1000:
					return og_csge(buffer, sender);
					//break;
				case 3000:
					return og_csge(buffer, sender);
					//break;
				case 5000:
					return og_csge(buffer, sender);
					//break;
				case 7000:
					return og_csge(buffer, sender);
					//break;
				case 9000:
					return og_csge(buffer, sender);
					//break;
				default:
					SetConsoleTextAttribute(h_console, FOREGROUND_RED);
					fmt::print("Blocked modded bounty from {} with value ${}\n", sender->GetName(), args_list.at(4));
					SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
					return true;
				}
			}

			//break;
		default:
			return og_csge(buffer, sender);
		}
	}
	catch(...)
	{
		const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		fmt::print(
			"Exception/crash occurred in {}, will attempt to call original function\nFunc args: {}, {}\n",
			__FUNCTION__, reinterpret_cast<void*>(buffer), reinterpret_cast<void*>(sender));
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		return og_csge(buffer, sender);
	}
}

typedef char(__fastcall* DLC)(unsigned int dlc_hash);
DLC og_dlc;
char __fastcall hk_dlc(unsigned int dlc_hash)
{
	if (!IsThreadAFiber())
	{
		menu.dlc_fiber = ConvertThreadToFiber(nullptr);
		if (!menu.dlc_fiber)
			return og_dlc(dlc_hash);
	}

	g_job_pool->execute_internal();
	if (dlc_hash == 2532323046 && menu.rockstar_dev)
	{
		//*reinterpret_cast<int8_t*>(0x7FF7596D6D22) = 1; // set the global to true so the dlc check returns true
		memset(reinterpret_cast<void*>(menu.d_addy + (0x1ab9a7e + 0x06)), 1, 1);
		dlc_hash = 185704301; // change dlc_hash so the function checks a hash that will return true
	}

	return og_dlc(dlc_hash);
}

typedef void(__fastcall* NEH)(void* event_mgr,
	CNetGamePlayer* source_player, CNetGamePlayer* target_player, uint16_t event_id, int event_index, int event_handled_bitset, uint16_t bitbuffer_size, void* bitbuffer_data);
NEH og_neh;
bool __fastcall hk_neh(void* event_mgr, CNetGamePlayer* source_player, CNetGamePlayer* target_player, uint16_t event_id, int event_index, int event_handled_bitset, uint16_t bitbuffer_size, void* bitbuffer_data)
{
	const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

	try
	{
		auto _buffer = datBitBuffer(bitbuffer_data, bitbuffer_size); // create new bitbuffer instance for ease, using function's args for size and data
		auto* buffer = &_buffer;
		
		typedef void(__fastcall* NEA)(void* event_mgr, CNetGamePlayer* source_player, CNetGamePlayer* target_player, unsigned int event_index, unsigned int event_handled_bitset);
		if (event_id < 0 || event_id >= 91)
			return false;
		const char* event_name = *reinterpret_cast<const char**>(reinterpret_cast<uintptr_t>(event_mgr) + 8 * event_id + 241328);
		if (event_name == nullptr || source_player == nullptr)
		{
			spoof_call<void, void*, CNetGamePlayer*, CNetGamePlayer*, unsigned int, unsigned int>
				(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<NEA>(menu.nea_addy), event_mgr, source_player, target_player, event_index, event_handled_bitset);
			return false;
		}

		if(buffer == nullptr)
		{
			spoof_call<void, void*, CNetGamePlayer*, CNetGamePlayer*, unsigned int, unsigned int>
				(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<NEA>(menu.nea_addy), event_mgr, source_player, target_player, event_index, event_handled_bitset);
			return false;
		}
		//log_file.seekp(log_file.eof());
		//log_file << fmt::format("{}\n", event_name);
		//log_file.flush();
		switch (event_id)
		{
		default:
			{
				if(source_player == target_player)
				{
					spoof_call<void, void*, CNetGamePlayer*, CNetGamePlayer*, unsigned int, unsigned int>
						(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<NEA>(menu.nea_addy), event_mgr, source_player, target_player, event_index, event_handled_bitset);
					return false;
				}

				return true;
			}
		case 28: // scripted game event
		{
			DWORD args_count{}, args{};
			buffer->ReadDword(&args_count, 32);
			if (/*args_count > 0 && */args_count <= 0x1AF)
				buffer->ReadDword(&args, 8 * args_count);
			if (source_player == target_player)
			{
				spoof_call<void, void*, CNetGamePlayer*, CNetGamePlayer*, unsigned int, unsigned int>
					(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<NEA>(menu.nea_addy), event_mgr, source_player, target_player, event_index, event_handled_bitset);
				return false;
			}

			buffer->Seek(0);
			return true;
		}
		case 33: // script world state event
		{
			DWORD ev_1{}, ev_2{};
			fmt::print("{}\n", buffer->GetDataLength());
			if(!buffer->GetDataLength())
			{
				SetConsoleTextAttribute(h_console, FOREGROUND_BLUE | FOREGROUND_RED);
				fmt::print("Received {} from {}:{}\n",
					event_name, source_player->GetName(), source_player->GetNetData()->m_rockstar_id);
				SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

				log_file << fmt::format("Received {} from {}:{}\n",
					event_name, source_player->GetName(), source_player->GetNetData()->m_rockstar_id);
				log_file.flush();
				spoof_call<void, void*, CNetGamePlayer*, CNetGamePlayer*, unsigned int, unsigned int>
					(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<NEA>(menu.nea_addy), event_mgr, source_player, target_player, event_index, event_handled_bitset);
				return false;
			}
				
			buffer->ReadDword(&ev_1, 4);
			buffer->ReadDword(&ev_2, 1);
			SetConsoleTextAttribute(h_console, FOREGROUND_BLUE | FOREGROUND_RED);
			fmt::print("Received {} from {}:{}, with arg 1: {}, event + 56: {}\n",
				event_name, source_player->GetName(), source_player->GetNetData()->m_rockstar_id, ev_1, ev_2);
			SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

			log_file << fmt::format("Received {} from {}:{}, with arg 1: {}, event + 56: {}\n",
				event_name, source_player->GetName(), source_player->GetNetData()->m_rockstar_id, ev_1, ev_2);
			log_file.flush();
			spoof_call<void, void*, CNetGamePlayer*, CNetGamePlayer*, unsigned int, unsigned int>
				(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<NEA>(menu.nea_addy), event_mgr, source_player, target_player, event_index, event_handled_bitset);
			return false;
		}
		/*case 57: // train report event
		{
			DWORD event_48{}, event_52{};
			buffer->ReadDword(&event_48, 4);
			buffer->ReadDword(&event_52, 1);
			SetConsoleTextAttribute(h_console, FOREGROUND_BLUE | FOREGROUND_RED);
			fmt::print("Received {} from {}:{} with event + 48: {}, event + 52: {}\n",
				event_name, source_player->GetName(), source_player->GetNetData()->m_rockstar_id, event_48, event_52);
			SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

			log_file << fmt::format("Received {} from {}:{} with event + 48: {}, event + 52: {}\n",
				event_name, source_player->GetName(), source_player->GetNetData()->m_rockstar_id, event_48, event_52);
			log_file.flush();

			if(event_48 < 0 || event_48 > 27)
			{
				spoof_call<void, void*, CNetGamePlayer*, CNetGamePlayer*, unsigned int, unsigned int>
					(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<NEA>(menu.nea_addy), event_mgr, source_player, target_player, event_index, event_handled_bitset);
				return false;
			}
				
			buffer->Seek(0);
			return true;
		}*/
		case 59: // increment stat event
		{
			DWORD stat{}, stat_amount{};
			buffer->ReadDword(&stat, 32);
			buffer->ReadDword(&stat_amount, 32);
			SetConsoleTextAttribute(h_console, FOREGROUND_BLUE | FOREGROUND_RED);
			fmt::print("Received {} from {}:{} with stat {}\n",
				event_name, source_player->GetName(), source_player->GetNetData()->m_rockstar_id, stat);
			SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

			log_file << fmt::format("Received {} from {}:{} with stat {}\n",
				event_name, source_player->GetName(), source_player->GetNetData()->m_rockstar_id, stat);
			log_file.flush();
			// block report stat increments here
			buffer->Seek(0);
			return true;
		}
		case 65: // give pickup rewards event
		{
			DWORD reward_amount{}, reward_hash{};
			buffer->ReadDword(&reward_amount, 3);
			buffer->ReadDword(&reward_hash, 32);
			SetConsoleTextAttribute(h_console, FOREGROUND_BLUE | FOREGROUND_RED);
			fmt::print("Received {} from {}:{} with hash {} and {} amount {}\n",
				event_name, source_player->GetName(), source_player->GetNetData()->m_rockstar_id, reward_hash, reward_amount);
			SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

			log_file << fmt::format("Received {} from {}:{} with hash {} and {} amount {}\n",
				event_name, source_player->GetName(), source_player->GetNetData()->m_rockstar_id, reward_hash, reward_amount);
			log_file.flush();
			// check for crash reward hashes and block here
			buffer->Seek(0);
			return true;
		}
		case 17: // explosion event
		{
			DWORD explosion_owner{}, explosion_flags{}, explosion_type{};
			buffer->Seek(29);
			buffer->ReadDword(&explosion_owner, 13);
			buffer->Seek(55);
			buffer->ReadDword(&explosion_type, 8);
			buffer->Seek(149);
			buffer->ReadDword(&explosion_flags, 32);
			if (explosion_owner != 0)
			{
				SetConsoleTextAttribute(h_console, FOREGROUND_BLUE | FOREGROUND_RED);
				fmt::print("Received {} from {}:{} with type {}\n",
					event_name, source_player->GetName(), source_player->GetNetData()->m_rockstar_id, explosion_type);
				SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

				log_file.seekp(log_file.eof());
				log_file << fmt::format("Received {} from {}:{} with type {}\n",
					event_name, source_player->GetName(), source_player->GetNetData()->m_rockstar_id, explosion_type);
				log_file.flush();
				/*auto ped_owner_id = -1;
				if (auto net_obj = (*g_pointers->m_networked_object_manager)->find_object_by_id(explosion_owner, FALSE)) // TODO
					ped_owner_id = net_obj->m_owner_id;
				if ((explosion_flags & AddOwnedExplosionFlag) && source_player->player != ped_owner_id && ped_owner_id != -1)
				{
					BlockedEvent("ADD_OWNED_EXPLOSION", source_player);
					if (g_settings.options["log network events"] > 0)
						LOG(EVENT) << "Blocked Network Event from: " << source_player->player_info->m_name << " Type: " << eventName << " explosion_owner: " << explosion_owner << " explosion_flags: " << HEX_TO_UPPER(explosion_flags) << " explosion_type: " << explosion_type;
					g_pointers->m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
					return false;
				}*/

				buffer->Seek(0);
				return true;
			}

			spoof_call<void, void*, CNetGamePlayer*, CNetGamePlayer*, unsigned int, unsigned int>
				(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<NEA>(menu.nea_addy), event_mgr, source_player, target_player, event_index, event_handled_bitset);
			return false;
		}
		case 13: // remove weapon event
		{
			DWORD network_id, weapon_hash{};
			buffer->ReadDword(&network_id, 13);
			buffer->ReadDword(&weapon_hash, 32);
			SetConsoleTextAttribute(h_console, FOREGROUND_BLUE | FOREGROUND_RED);
			fmt::print("Received {} from {}:{} with net id {}, hash {}\n",
				event_name, source_player->GetName(), source_player->GetNetData()->m_rockstar_id, network_id, weapon_hash);
			SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

			log_file << fmt::format("Received {} from {}:{} with net id {}, hash {}\n",
				event_name, source_player->GetName(), source_player->GetNetData()->m_rockstar_id, network_id, weapon_hash);
			log_file.flush();
			if (target_player->m_PlayerID == network_id)
			{
				spoof_call<void, void*, CNetGamePlayer*, CNetGamePlayer*, unsigned int, unsigned int>
					(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<NEA>(menu.nea_addy), event_mgr, source_player, target_player, event_index, event_handled_bitset);
				return false;
			}

			buffer->Seek(0);
			return true;
		}
		case 43: // clear ped tasks event
		{
			DWORD network_id{};
			buffer->ReadDword(&network_id, 13);
			SetConsoleTextAttribute(h_console, FOREGROUND_BLUE | FOREGROUND_RED);
			fmt::print("Received {} from {}:{} with net id {}\n",
				event_name, source_player->GetName(), source_player->GetNetData()->m_rockstar_id, network_id);
			SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

			log_file.seekp(log_file.eof());
			log_file << fmt::format("Received {} from {}:{} with net id {}\n",
				event_name, source_player->GetName(), source_player->GetNetData()->m_rockstar_id, network_id);
			log_file.flush();
			if (target_player->m_PlayerID == network_id)
			{
				spoof_call<void, void*, CNetGamePlayer*, CNetGamePlayer*, unsigned int, unsigned int>
					(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<NEA>(menu.nea_addy), event_mgr, source_player, target_player, event_index, event_handled_bitset);
				return false;
			}

			buffer->Seek(0);
			return true;
		}
		case 4: // request control event
		{
			DWORD network_id{};
			buffer->ReadDword(&network_id, 13);
			SetConsoleTextAttribute(h_console, FOREGROUND_BLUE | FOREGROUND_RED);
			fmt::print("Received {} from {}:{} with net id {}\n",
				event_name, source_player->GetName(), source_player->GetNetData()->m_rockstar_id, network_id);
			SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

			log_file << fmt::format("Received {} from {}:{} with net id {}\n",
				event_name, source_player->GetName(), source_player->GetNetData()->m_rockstar_id, network_id);
			log_file.flush();
			buffer->Seek(0);
			return true;
		}
		case 10: // game weather event
		{
			DWORD propogate{}, target{}, weather{};
			buffer->ReadDword(&propogate, 1);
			buffer->ReadDword(&weather, 5);
			if (!propogate)
				buffer->ReadDword(&target, 9);
			SetConsoleTextAttribute(h_console, FOREGROUND_BLUE | FOREGROUND_RED);
			fmt::print("Received {} from {}:{} with propogate {}, weather {}, target {]\n",
				event_name, source_player->GetName(), source_player->GetNetData()->m_rockstar_id, propogate, weather, target);
			SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

			log_file << fmt::format("Received {} from {}:{} with propogate {}, weather {}, target {]\n",
				event_name, source_player->GetName(), source_player->GetNetData()->m_rockstar_id, propogate, weather, target);
			log_file.flush();
			if (weather > 13 || !propogate)
			{
				spoof_call<void, void*, CNetGamePlayer*, CNetGamePlayer*, unsigned int, unsigned int>
					(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<NEA>(menu.nea_addy), event_mgr, source_player, target_player, event_index, event_handled_bitset);
				return false;
			}

			buffer->Seek(0);
			return true;
		}
		case 64: // kick votes event
		{
			DWORD bitset{};
			buffer->ReadDword(&bitset, 32);
			if (bitset & (1 << target_player->m_PlayerID))
			{
				SetConsoleTextAttribute(h_console, FOREGROUND_BLUE | FOREGROUND_RED);
				fmt::print("Received {} from {}:{}\n",
					event_name, source_player->GetName(), source_player->GetNetData()->m_rockstar_id);
				SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

				log_file.seekp(log_file.eof());
				log_file << fmt::format("Received {} from {}:{}\n",
					event_name, source_player->GetName(), source_player->GetNetData()->m_rockstar_id);
				log_file.flush();
			}

			buffer->Seek(0);
			return true;
		}
		case 84: // report cash spawn event
		{
			DWORD money{};
			buffer->Seek(64);
			buffer->ReadDword(&money, 32);
			buffer->Seek(0);
			switch (money)
			{
			case 500:
			case 1000:
			case 1500:
			case 2000:
			case 2500:
				SetConsoleTextAttribute(h_console, FOREGROUND_BLUE | FOREGROUND_RED);
				fmt::print("{}:{} is dropping ${}\n", source_player->GetName(), source_player->GetNetData()->m_rockstar_id, money);
				SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

				log_file << fmt::format("{}:{} is dropping ${}\n", source_player->GetName(), source_player->GetNetData()->m_rockstar_id, money);
				log_file.flush();
			}

			return true;
		}
		case 9: // game clock event
		{
			DWORD hour{}, min{}, sec{};
			buffer->Seek(21);
			buffer->ReadDword(&hour, 6);
			buffer->ReadDword(&min, 7);
			buffer->ReadDword(&sec, 7);
			buffer->Seek(0);
			SetConsoleTextAttribute(h_console, FOREGROUND_BLUE | FOREGROUND_RED);
			//fmt::print("Received {} from {}:{} with hour {}, min {}, sec {}\n",
				//event_name, source_player->GetName(), source_player->GetNetData()->m_rockstar_id, hour, min, sec);
			SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			log_file << fmt::format("Received {} from {}:{} with hour {}, min {}, sec {}\n",
				event_name, source_player->GetName(), source_player->GetNetData()->m_rockstar_id, hour, min, sec);
			log_file.flush();
			if(hour > 23 || min > 59 || sec > 59)
			{
				spoof_call<void, void*, CNetGamePlayer*, CNetGamePlayer*, unsigned int, unsigned int>
					(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<NEA>(menu.nea_addy), event_mgr, source_player, target_player, event_index, event_handled_bitset);
				return false;
			}

			return true;
		}
		case 78: // check code crcs, modder beacon
		case 83: // report myself event, modder beacon
			{
				SetConsoleTextAttribute(h_console, FOREGROUND_BLUE | FOREGROUND_RED);
				fmt::print("Received {} from modder {}:{}", event_name, source_player->GetName(), source_player->GetNetData()->m_rockstar_id);
				SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

				log_file << fmt::format("Received {} from modder {}:{}", event_name, source_player->GetName(), source_player->GetNetData()->m_rockstar_id);
				log_file.flush();
				spoof_call<void, void*, CNetGamePlayer*, CNetGamePlayer*, unsigned int, unsigned int>
					(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<NEA>(menu.nea_addy), event_mgr, source_player, target_player, event_index, event_handled_bitset);
				return false;
			}
		}
	}
	catch (...)
	{
		const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		fmt::print(
			"Exception/crash occurred in {}, will attempt to call original function\n\n", __FUNCTION__);
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		og_neh(event_mgr, source_player, target_player, event_id, event_index, event_handled_bitset, bitbuffer_size, bitbuffer_data);
	}
}

typedef bool(__fastcall* CRCL)(__int64 thisptr, CNetGamePlayer* sender, CNetGamePlayer* receiver);
CRCL og_crcl;
bool __fastcall hk_crcl(__int64 thisptr, CNetGamePlayer* sender, CNetGamePlayer* receiver)
{
	const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

	try
	{
		if (sender == nullptr || receiver == nullptr)
			return og_crcl(thisptr, sender, receiver);
		if(reinterpret_cast<void*>(thisptr) == nullptr)
			return og_crcl(thisptr, sender, receiver);
		auto str = fmt::format("Received net event NETWORK_CRC_HASH_CHECK_EVENT from {}:{}(PID : {}) to PID {}:{}, rcx : {}, args : [rcx+0x28 (target PID?)]: {} [rcx + 0x31] : {}, [rcx + 0x32] : {}, [rcx + 0x33] : {}, [rcx + 0x34] : {}, [rcx + 0x38] : {}\n)", sender->GetName(),
			sender->GetNetData()->m_rockstar_id, sender->m_PlayerID, receiver->m_ActivePlayerID, receiver->m_PlayerID,
			reinterpret_cast<void*>(thisptr), *reinterpret_cast<uint8_t*>(thisptr + 0x28), *reinterpret_cast<BYTE*>(thisptr + 0x31), *reinterpret_cast<BYTE*>(thisptr + 0x32),
			*reinterpret_cast<BYTE*>(thisptr + 0x33), *reinterpret_cast<BYTE*>(thisptr + 0x34), *reinterpret_cast<DWORD*>(thisptr + 0x38));
		log_file << str;
		log_file.flush();
		SetConsoleTextAttribute(h_console, FOREGROUND_BLUE | FOREGROUND_RED);
		fmt::print(
			"Received net event NETWORK_CRC_HASH_CHECK_EVENT from {}:{}(PID: {}) to PID {}:{}, rcx: {}, args: [rcx+0x28 (target PID?)]: {} [rcx+0x31]: {}, [rcx+0x32]: {}, [rcx+0x33]: {}, [rcx+0x34]: {}, [rcx+0x38]: {}\n", sender->GetName(),
			sender->GetNetData()->m_rockstar_id, sender->m_PlayerID, receiver->m_ActivePlayerID, receiver->m_PlayerID,
			reinterpret_cast<void*>(thisptr), *reinterpret_cast<uint8_t*>(thisptr + 0x28), *reinterpret_cast<BYTE*>(thisptr + 0x31), *reinterpret_cast<BYTE*>(thisptr + 0x32),
			*reinterpret_cast<BYTE*>(thisptr + 0x33), *reinterpret_cast<BYTE*>(thisptr + 0x34), *reinterpret_cast<DWORD*>(thisptr + 0x38));
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		return og_crcl(thisptr, sender, receiver);
	}
	catch (...)
	{
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		fmt::print(
			"Exception/crash occurred in {}, will attempt to call original function\nFunc args: {}, {}\n",
			__FUNCTION__, reinterpret_cast<void*>(sender), reinterpret_cast<void*>(receiver));
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		return og_crcl(thisptr, sender, receiver);
	}
}

void TryMHInit()
{
	const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);
	switch(MH_Initialize())
	{
	case MH_ERROR_ALREADY_INITIALIZED:
		break;
	case MH_OK:
		break;
	default:
		char mh_failed[] = "Failed to initalise MH";
		HiddenText mh_failed_enc = mh_failed;
		mh_failed_enc.DecryptString();
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		cout << mh_failed_enc.GetDecryptedString() << endl;
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		//MessageBoxA(NULL, mh_failed_enc.GetDecryptedString(), "Kaizenn", MB_OK | MB_ICONERROR);
	}
}

GameHooks::GameHooks(Menu m)
{
	_menu = m;
	TryMHInit();
}

GameHooks::~GameHooks() = default;

void GameHooks::enable_rid0_hook()
{
	const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);
	
	switch (MH_CreateHook(reinterpret_cast<LPVOID*>(menu.rid_addy), &hk_rid, reinterpret_cast<LPVOID*>(&og_rid)))
	{
	case MH_ERROR_MEMORY_ALLOC:
		cout << "mem alloc" << endl;
		break;
	case MH_ERROR_ALREADY_CREATED:
		break;
	case MH_OK:
		break;
	default:
		char mh_failed[] = "Failed to create RID hook";
		HiddenText mh_failed_enc = mh_failed;
		mh_failed_enc.DecryptString();
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		cout << mh_failed_enc.GetDecryptedString() << endl;
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		return;
	}

	switch (MH_EnableHook(reinterpret_cast<LPVOID*>(menu.rid_addy)))
	{
	case MH_ERROR_ENABLED:
		break;
	case MH_OK:
		break;
	default:
		_menu.sge_enabled = false;
		char mh_failed[] = "Failed to enable RID hook";
		HiddenText mh_failed_enc = mh_failed;
		mh_failed_enc.DecryptString();
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		cout << mh_failed_enc.GetDecryptedString() << endl;
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}
}

void GameHooks::enable_pje_hook()
{
	const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

	switch (MH_CreateHook(reinterpret_cast<LPVOID*>(menu.pje_addy), &hk_pje, reinterpret_cast<LPVOID*>(&og_pje)))
	{
	case MH_ERROR_MEMORY_ALLOC:
		cout << "mem alloc" << endl;
		break;
	case MH_ERROR_ALREADY_CREATED:
		break;
	case MH_OK:
		break;
	default:
		char mh_failed[] = "Failed to create PJE hook";
		HiddenText mh_failed_enc = mh_failed;
		mh_failed_enc.DecryptString();
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		cout << mh_failed_enc.GetDecryptedString() << endl;
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		return;
	}

	switch (MH_EnableHook(reinterpret_cast<LPVOID*>(menu.pje_addy)))
	{
	case MH_ERROR_ENABLED:
		break;
	case MH_OK:
		break;
	default:
		char mh_failed[] = "Failed to enable PJE hook";
		HiddenText mh_failed_enc = mh_failed;
		mh_failed_enc.DecryptString();
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		cout << mh_failed_enc.GetDecryptedString() << endl;
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}
}

void GameHooks::disable_pje_hook()
{
	switch (MH_DisableHook(reinterpret_cast<LPVOID>(_menu.pje_addy)))
	{
	case MH_ERROR_NOT_CREATED:
		break;
	case MH_OK:
		break;
	default:
		break;
	}
}

void GameHooks::enable_ple_hook()
{
	const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

	switch (MH_CreateHook(reinterpret_cast<LPVOID*>(menu.ple_addy), &hk_ple, reinterpret_cast<LPVOID*>(&og_ple)))
	{
	case MH_ERROR_MEMORY_ALLOC:
		cout << "mem alloc" << endl;
		break;
	case MH_ERROR_ALREADY_CREATED:
		break;
	case MH_OK:
		break;
	default:
		char mh_failed[] = "Failed to create PLE hook";
		HiddenText mh_failed_enc = mh_failed;
		mh_failed_enc.DecryptString();
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		cout << mh_failed_enc.GetDecryptedString() << endl;
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		return;
	}

	switch (MH_EnableHook(reinterpret_cast<LPVOID*>(menu.ple_addy)))
	{
	case MH_ERROR_ENABLED:
		break;
	case MH_OK:
		break;
	default:
		char mh_failed[] = "Failed to enable PLE hook";
		HiddenText mh_failed_enc = mh_failed;
		mh_failed_enc.DecryptString();
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		cout << mh_failed_enc.GetDecryptedString() << endl;
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}
}

void GameHooks::disable_ple_hook()
{
	switch (MH_DisableHook(reinterpret_cast<LPVOID>(_menu.ple_addy)))
	{
	case MH_ERROR_NOT_CREATED:
		break;
	case MH_OK:
		break;
	default:
		break;
	}
}

void GameHooks::enable_rde_hook()
{
	const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

	switch (MH_CreateHook(reinterpret_cast<LPVOID*>(menu.rde_addy), &hk_rde, reinterpret_cast<LPVOID*>(&og_rde)))
	{
	case MH_ERROR_MEMORY_ALLOC:
		cout << "mem alloc" << endl;
		break;
	case MH_ERROR_ALREADY_CREATED:
		break;
	case MH_OK:
		break;
	default:
		char mh_failed[] = "Failed to create RDE hook";
		HiddenText mh_failed_enc = mh_failed;
		mh_failed_enc.DecryptString();
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		cout << mh_failed_enc.GetDecryptedString() << endl;
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		return;
	}

	switch (MH_EnableHook(reinterpret_cast<LPVOID*>(menu.rde_addy)))
	{
	case MH_ERROR_ENABLED:
		break;
	case MH_OK:
		break;
	default:
		char mh_failed[] = "Failed to enable RDE hook";
		HiddenText mh_failed_enc = mh_failed;
		mh_failed_enc.DecryptString();
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		cout << mh_failed_enc.GetDecryptedString() << endl;
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}
}

void GameHooks::disable_rde_hook()
{
	switch (MH_DisableHook(reinterpret_cast<LPVOID>(_menu.rde_addy)))
	{
	case MH_ERROR_NOT_CREATED:
		break;
	case MH_OK:
		break;
	default:
		break;
	}
}

void GameHooks::disable_rid0_hook()
{
	switch (MH_DisableHook(reinterpret_cast<LPVOID>(_menu.rid_addy)))
	{
	case MH_ERROR_NOT_CREATED:
		break;
	case MH_OK:
		break;
	default:
		break;
	}
}

void GameHooks::enable_sge_hook()
{
	const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);
	
	switch(MH_CreateHook(reinterpret_cast<LPVOID*>(menu.sge_addy), &hk_csge, reinterpret_cast<LPVOID*>(&og_csge)))
	{
	case MH_ERROR_MEMORY_ALLOC:
		//log_file.close();
		_menu.sge_enabled = false;
		break;
	case MH_ERROR_ALREADY_CREATED:
		break;
	case MH_OK:
		break;
	default:
		log_file.close();
		_menu.sge_enabled = false;
		char mh_failed[] = "Failed to create SGE hook";
		HiddenText mh_failed_enc = mh_failed;
		mh_failed_enc.DecryptString();
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		cout << mh_failed_enc.GetDecryptedString() << endl;
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		return;
	}

	switch(MH_EnableHook(reinterpret_cast<LPVOID*>(menu.sge_addy)))
	{
	case MH_ERROR_ENABLED:
		break;
	case MH_OK:
		break;
	default:
		log_file.close();
		_menu.sge_enabled = false;
		char mh_failed[] = "Failed to enable SGE hook";
		HiddenText mh_failed_enc = mh_failed;
		mh_failed_enc.DecryptString();
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		cout << mh_failed_enc.GetDecryptedString() << endl;
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}

	sge_return_addy = reinterpret_cast<uintptr_t>(og_csge);
}

void GameHooks::enable_neh_hook()
{
	const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

	switch (MH_CreateHook(reinterpret_cast<LPVOID*>(menu.neh_addy), &hk_neh, reinterpret_cast<LPVOID*>(&og_neh)))
	{
	case MH_ERROR_MEMORY_ALLOC:
		cout << "mem alloc" << endl;
		break;
	case MH_ERROR_ALREADY_CREATED:
		break;
	case MH_OK:
		break;
	default:
		char mh_failed[] = "Failed to create NEH hook";
		HiddenText mh_failed_enc = mh_failed;
		mh_failed_enc.DecryptString();
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		cout << mh_failed_enc.GetDecryptedString() << endl;
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		return;
	}

	switch (MH_EnableHook(reinterpret_cast<LPVOID*>(menu.neh_addy)))
	{
	case MH_ERROR_ENABLED:
		break;
	case MH_OK:
		break;
	default:
		char mh_failed[] = "Failed to enable NEH hook";
		HiddenText mh_failed_enc = mh_failed;
		mh_failed_enc.DecryptString();
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		cout << mh_failed_enc.GetDecryptedString() << endl;
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}
}

void GameHooks::disable_neh_hook()
{
	switch (MH_DisableHook(reinterpret_cast<LPVOID>(_menu.neh_addy)))
	{
	case MH_ERROR_NOT_CREATED:
		break;
	case MH_OK:
		break;
	default:
		break;
	}
}

void GameHooks::disable_sge_hook()
{
	log_file.close();
	switch(MH_DisableHook(reinterpret_cast<LPVOID>(_menu.sge_addy)))
	{
	case MH_ERROR_NOT_CREATED:
		break;
	case MH_OK:
		break;
	default:
		break;
	}
}

void GameHooks::enable_dlc_hook()
{
	const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

	switch (MH_CreateHook(reinterpret_cast<LPVOID*>(menu.dlc_addy), &hk_dlc, reinterpret_cast<LPVOID*>(&og_dlc)))
	{
	case MH_ERROR_MEMORY_ALLOC:
		cout << "mem alloc" << endl;
		break;
	case MH_ERROR_ALREADY_CREATED:
		break;
	case MH_OK:
		break;
	default:
		char mh_failed[] = "Failed to create DLC hook";
		HiddenText mh_failed_enc = mh_failed;
		mh_failed_enc.DecryptString();
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		cout << mh_failed_enc.GetDecryptedString() << endl;
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		return;
	}

	switch (MH_EnableHook(reinterpret_cast<LPVOID*>(menu.dlc_addy)))
	{
	case MH_ERROR_ENABLED:
		break;
	case MH_OK:
		break;
	default:
		char mh_failed[] = "Failed to enable DLC hook";
		HiddenText mh_failed_enc = mh_failed;
		mh_failed_enc.DecryptString();
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		cout << mh_failed_enc.GetDecryptedString() << endl;
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}
}


void GameHooks::disable_dlc_hook()
{
	switch (MH_DisableHook(reinterpret_cast<LPVOID>(_menu.dlc_addy)))
	{
	case MH_ERROR_NOT_CREATED:
		break;
	case MH_OK:
		break;
	default:
		break;
	}
}