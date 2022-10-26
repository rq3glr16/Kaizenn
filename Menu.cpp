#include "stdafx.h"
#include "Menu.h"
#include "GameHooks.h"
////#include "job_pool.h"
#include <winternl.h>
#include <sstream>
#include <random>
#include <MinHook/include/MinHook.h>

using namespace ImGui;

//auto g_job_pool = make_unique<job_pool>();

float test = 1;

template <class T>
string to_string(T t, ios_base& (*f)(ios_base&))
{
	ostringstream oss;
	oss << f << t;
	return oss.str();
}
extern string to_upper(string& in);
extern ofstream log_file;

bool datBitBuffer::ReadDword(DWORD* integer, int bits)
{
	typedef bool(__fastcall* RDW)(datBitBuffer* thisptr, void* integer, int bits_to_read);
	return spoof_call<bool, datBitBuffer*, void*, int>(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<RDW>(menu.rdw_addy),
		this, integer, bits);
}


// NOTE: signed
bool datBitBuffer::WriteDword(DWORD* integer, int bits)
{
	typedef bool(__fastcall* WINT)(void* thisptr, void* integer, int bits_to_write);
	return spoof_call<bool, void*, void*, int>(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<WINT>(menu.wint_addy),
		this, integer, bits);
}

void Menu::render_misc_tab()
{
	GameHooks hooks = GameHooks(menu);
	if (ImGui::Button("close game"))
	{
		static_cast<void(__fastcall*)(int64_t, int8_t)>(reinterpret_cast<void*>(
			PatternScan(reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
				"\x48\x8b\xc4\x48\x89\x58\xcc\x48\x89\x70\xcc\x48\x89\x78\xcc\x4c\x89\x60\xcc\x55\x41\x56\x41\x57\x48\x8d\xa8\xcc\xcc\xcc\xcc\x48\x81\xec\xcc\xcc\xcc\xcc\x0f\xb7\x05",
				"xxxxxx?xxx?xxx?xxx?xxxxxxxx????xxx????xxx", 41)))(1140078713, 1); // 194586738: err net array, 2350890962: err string cache, 2292411148: err audio, 866894110: err audio, 1022726701: err gfx, 1133243449: mem pool, 3337940100: corrupt memory pool alloc, 1140078713: sys pure
		*reinterpret_cast<uintptr_t*>(0xB4DC0D3) = -1;
	}

	char rockstar_dev[] = "Enable R* dev mode";
	HiddenText rockstar_dev_enc = rockstar_dev;
	rockstar_dev_enc.DecryptString();
	ImGui::Checkbox(rockstar_dev_enc.GetDecryptedString(), &menu.rockstar_dev);
	
	char log_script_events[] = "Enable script event logging";
	HiddenText log_script_events_enc = log_script_events;
	log_script_events_enc.DecryptString();
	ImGui::Checkbox(log_script_events_enc.GetDecryptedString(), &menu.se_logging);

	char log_to_console[] = "Log script events to console";
	HiddenText log_to_console_enc = log_to_console;
	log_to_console_enc.DecryptString();
	ImGui::Checkbox(log_to_console_enc.GetDecryptedString(), &menu.log_to_console);

	char dealloc_console[] = "Deallocate console (wil be safe to close)";
	HiddenText dealloc_console_enc = dealloc_console;
	dealloc_console_enc.DecryptString();
	if(ImGui::Button(dealloc_console_enc.GetDecryptedString()))
		FreeConsole();
}

void Menu::render_protections_tab()
{
	GameHooks hooks = GameHooks(menu);
	char script_events[] = "Script Events";
	HiddenText script_events_enc = script_events;
	script_events_enc.DecryptString();
	if (ImGui::CollapsingHeader(script_events_enc.GetDecryptedString()))
	{
		char invalid_apartment_invite[] = "Block invalid apartment invites";
		HiddenText invalid_apartment_invite_enc = invalid_apartment_invite;
		invalid_apartment_invite_enc.DecryptString();
		ImGui::Checkbox(invalid_apartment_invite_enc.GetDecryptedString(), &menu.invalid_apartment);

		char transaction_error[] = "Block transaction error screen";
		HiddenText transaction_error_enc = transaction_error;
		transaction_error_enc.DecryptString();
		ImGui::Checkbox(transaction_error_enc.GetDecryptedString(), &menu.transaction_error);

		char kill_engine[] = "Block remote kill vehicle engine";
		HiddenText kill_engine_enc = kill_engine;
		kill_engine_enc.DecryptString();
		ImGui::Checkbox(kill_engine_enc.GetDecryptedString(), &menu.kill_engine);

		char vehicle_kick[] = "Block remote vehicle kick";
		HiddenText vehicle_kick_enc = vehicle_kick;
		vehicle_kick_enc.DecryptString();
		ImGui::Checkbox(vehicle_kick_enc.GetDecryptedString(), &menu.vehicle_kick);

		char off_radar[] = "Block remote off radar";
		HiddenText off_radar_enc = off_radar;
		off_radar_enc.DecryptString();
		ImGui::Checkbox(off_radar_enc.GetDecryptedString(), &menu.off_radar);

		char ceo[] = "Block remote CEO kick/ban";
		HiddenText ceo_enc = ceo;
		ceo_enc.DecryptString();
		ImGui::Checkbox(ceo_enc.GetDecryptedString(), &menu.ceo);

		char apartment_invite[] = "Block apartment invites";
		HiddenText apartment_invite_enc = apartment_invite;
		apartment_invite_enc.DecryptString();
		ImGui::Checkbox(apartment_invite_enc.GetDecryptedString(), &menu.apartment_invite);

		char sound_spam[] = "Block sound spam";
		HiddenText sound_spam_enc = sound_spam;
		sound_spam_enc.DecryptString();
		ImGui::Checkbox(sound_spam_enc.GetDecryptedString(), &menu.sound_spam);

		char wanted_level[] = "Block remote wanted level";
		HiddenText wanted_level_enc = wanted_level;
		wanted_level_enc.DecryptString();
		ImGui::Checkbox(wanted_level_enc.GetDecryptedString(), &menu.wanted_level);

		char mission[] = "Block force to mission";
		HiddenText mission_enc = mission;
		mission_enc.DecryptString();
		ImGui::Checkbox(mission_enc.GetDecryptedString(), &menu.mission);

		char modded_bounty[] = "Block modded bounties";
		HiddenText modded_bounty_enc = modded_bounty;
		modded_bounty_enc.DecryptString();
		ImGui::Checkbox(modded_bounty_enc.GetDecryptedString(), &menu.modded_bounty);

		char bounties[] = "";
		HiddenText bounties_enc = bounties;
		bounties_enc.DecryptString();
		ImGui::Checkbox(bounties_enc.GetDecryptedString(), &menu.bounties);
	}

	char net_events[] = "Net Events";
	HiddenText net_events_enc = net_events;
	net_events_enc.DecryptString();
	if(ImGui::CollapsingHeader(net_events_enc.GetDecryptedString()))
	{
		char clear_ped_tasks[] = "Freeze";
		HiddenText clear_ped_tasks_enc = clear_ped_tasks;
		clear_ped_tasks_enc.DecryptString();
		ImGui::Checkbox(clear_ped_tasks_enc.GetDecryptedString(), &menu.clear_ped_tasks);

		char remove_weapon[] = "Remove Weapon";
		HiddenText remove_weapon_enc = remove_weapon;
		remove_weapon_enc.DecryptString();
		ImGui::Checkbox(remove_weapon_enc.GetDecryptedString(), &menu.remove_weapon);
	}
}

void fiber_desync(LPVOID lpParam)
{
	const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

	try
	{
		typedef void(__fastcall* tse)(int, DWORD64[], int, int);
		int sent = 0;
		for (int i = 0; i <= 32; i++)
		{
			typedef CNetGamePlayer* (__fastcall* get_net_player)(unsigned int pid);
			auto net_player = spoof_call<CNetGamePlayer*,
				unsigned int>(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<get_net_player>(menu.gnp_addy), i);
			if (net_player == nullptr)
				continue;
			if (sent < 2)
			{
				default_random_engine gen;
				uniform_int_distribution<int> distrib(-120834823, -1);
				int arg1 = distrib(gen);
				int arg2 = distrib(gen);
				DWORD64 desync[4] = { -1822234181, arg1, arg2, 0 };
				DWORD64 desync2[4] = { -1822234181, arg1, arg2, -1 };
				spoof_call<void>(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<tse>(menu.tse_addy), 1, desync, 4, 1 << i);
				spoof_call<void>(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<tse>(menu.tse_addy), 1, desync2, 4, 1 << i);
				//cout << "desync sent with args: " << arg1 << ", " << arg2 << " to player: " << net_player->GetName() << endl;
				fmt::print("desync sent with args: {}, {} to player: {}\n", arg1, arg2, net_player->GetName());
				sent++;
				continue;
			}
		}

		SwitchToFiber(g_job_pool->job_fiber);
	}
	catch (...)
	{
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		fmt::print(
			"Exception/crash occurred in {}\n",
			__FUNCTION__);
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		SwitchToFiber(g_job_pool->job_fiber);
	}
}

void fiber_request_door(LPVOID lpParam)
{
	const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

	try
	{
		uintptr_t create_space_global = reinterpret_cast<uintptr_t>(PatternScan(reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
			"\x48\x8b\x0d\xcc\xcc\xcc\xcc\x48\x8b\xd7\xe8\xcc\xcc\xcc\xcc\x48\x8b\x5c\x24\xcc\x48\x8b\x6c\x24", "xxx????xxxx????xxxx?xxxx", 24));
		if (create_space_global == NULL)
			SwitchToFiber(g_job_pool->job_fiber);
		typedef bool(__fastcall* CSFGE)(uintptr_t, int);
		if (!reinterpret_cast<uintptr_t*>(create_space_global + 0x17416bd + 7))
			SwitchToFiber(g_job_pool->job_fiber);
		if (!*reinterpret_cast<uintptr_t*>(create_space_global + 0x17416bd + 7))
			SwitchToFiber(g_job_pool->job_fiber);

		uintptr_t global_event_global = reinterpret_cast<uintptr_t>(PatternScan(reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
			"\x48\x8b\x0d\xcc\xcc\xcc\xcc\xe8\xcc\xcc\xcc\xcc\x48\x8b\xf0\x48\x85\xc0\x74\xcc\x48\x8b\x13", "xxx????x????xxxxxxx?xxx", 23));
		if (global_event_global == NULL)
			SwitchToFiber(g_job_pool->job_fiber);
		typedef void* (__fastcall* GEC)(uintptr_t);
		if (!reinterpret_cast<uintptr_t*>(global_event_global + 0x1ce3eed + 7))
			SwitchToFiber(g_job_pool->job_fiber);
		if (!*reinterpret_cast<uintptr_t*>(global_event_global + 0x1ce3eed + 7))
			SwitchToFiber(g_job_pool->job_fiber);
		
		CNetGamePlayer* net_player = nullptr;
		CNetGamePlayer* self = nullptr;
		for (int i = 0; i <= 32; i++)
		{
			typedef CNetGamePlayer* (__fastcall* get_net_player)(unsigned int pid);
			net_player = spoof_call<CNetGamePlayer*,
				unsigned int>(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<get_net_player>(menu.gnp_addy), i);
			if (net_player == nullptr)
				continue;
			if (string(net_player->GetName()) == string("self")) // what if self is not first == self == null
			{
				self = net_player;
				continue;
			}

			if (net_player != nullptr)
				fmt::print("Name {} from PID {}\n", net_player->GetName(), net_player->m_PlayerID);
			else
				continue;
			bool result = spoof_call<bool, uintptr_t, int>(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<CSFGE>(menu.cev_addy),
				*reinterpret_cast<uintptr_t*>(create_space_global + 0x17416bd + 7), 1);
			if (!result)
				SwitchToFiber(g_job_pool->job_fiber);

			void* global_event = spoof_call(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<GEC>(menu.gev_addy),
				*reinterpret_cast<uintptr_t*>(global_event_global + 0x1ce3eed + 7));
			if (global_event == nullptr)
				SwitchToFiber(g_job_pool->job_fiber);
			#pragma pack(push, 1)
			class rde_a4
			{
			public:
				char pad_0000[8]; //0x0000
				uint32_t N00000050; //0x0008
				uint8_t* N0000017E; //0x000C
			};
			#pragma pack(pop)
			static_assert(offsetof(rde_a4, N00000050) == 0x08);
			static_assert(offsetof(rde_a4, N0000017E) == 0x0C);
			rde_a4* a4 = new rde_a4;
			a4->N00000050 = -9786234683476;
			BYTE* a = new BYTE(1);
			//a4->N0000017E = a;
			a4->N0000017E = nullptr;
			typedef void* (__fastcall* RDEC)(void*, int32_t, int32_t, void*);
			void* rde_event = spoof_call<void*, void*, int32_t, int32_t, void*>(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<RDEC>(menu.rdee_addy),
				global_event, -14294967295, -14294967295, a4);
			if (rde_event == nullptr)
				SwitchToFiber(g_job_pool->job_fiber);
			*reinterpret_cast<CNetGamePlayer**>(reinterpret_cast<uintptr_t>(rde_event) + 0x10) = self; // change source player
			*reinterpret_cast<CNetGamePlayer**>(reinterpret_cast<uintptr_t>(rde_event) + 0x18) = net_player; // change target player
			*reinterpret_cast<BYTE*>(reinterpret_cast<uintptr_t>(rde_event) + 120) = 1;
			*reinterpret_cast<BYTE*>(reinterpret_cast<uintptr_t>(rde_event) + 122) = 256;
			delete a;
			fmt::print("netGameEvent::handle address: {}\n", 
				reinterpret_cast<void*>(*reinterpret_cast<uintptr_t*>(*static_cast<uintptr_t*>(rde_event) + 0x38)));
			/*fmt::print("netGameEvent::handle result: {}\n", static_cast<bool (__fastcall*)(void*, CNetGamePlayer*, CNetGamePlayer*)>
			(reinterpret_cast<void*>(*reinterpret_cast<uintptr_t*>(*static_cast<uintptr_t*>(rde_event) + 0x38)))
				(rde_event, self, net_player));*/
			fmt::print("netGameEvent::needs_reply address: {}\n",
				reinterpret_cast<void*>(*reinterpret_cast<uintptr_t*>(*static_cast<uintptr_t*>(rde_event) + 0x20)));
			fmt::print("netGameEvent::needs_reply result: {}\n", static_cast<bool(__fastcall*)(void*)>
				(reinterpret_cast<void*>(*reinterpret_cast<uintptr_t*>(*static_cast<uintptr_t*>(rde_event) + 0x20)))(rde_event));
			if (*reinterpret_cast<rage::netPlayer**>(reinterpret_cast<uintptr_t>(rde_event) + 0x18) != nullptr)
				fmt::print("Target player: {}\n", (*reinterpret_cast<rage::netPlayer**>(reinterpret_cast<uintptr_t>(rde_event) + 0x18))->GetName());
			typedef void(__fastcall* AEQ)(uintptr_t, void*);
			spoof_call(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<AEQ>(menu.aeq_addy),
				*reinterpret_cast<uintptr_t*>(create_space_global + 0x17416bd + 7), rde_event);
			fmt::print("RDE #1 sent\n");
			delete a4;
		}
		
		SwitchToFiber(g_job_pool->job_fiber);
	}
	catch (...)
	{
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		fmt::print(
			"Exception/crash occurred in {}\n",
			__FUNCTION__);
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		SwitchToFiber(g_job_pool->job_fiber);
	}
}

bool __fastcall is_applicable_to_player(void* thisptr, rage::netPlayer* player)
{
	if (string(player->GetName()) == string("self"))
		return false;
	
	/* FOR SCRIPT ARRAY DATA VERIFY EVENT */
	/*void* v5 = reinterpret_cast<void*(__fastcall*)(void*)>(*static_cast<uintptr_t*>(thisptr) + 168)(thisptr);
	fmt::print("{}\n", v5);
	if (v5 == nullptr)
		return true;
	evp = reinterpret_cast<CNetGamePlayer*(__fastcall*)(*static_cast<uintptr_t*>(thisptr) + 264)(void*)>(v5);
	fmt::print("{}\n", evp);
	if(evp == nullptr)
		return true;*/
	
	//*reinterpret_cast<rage::netPlayer**>(reinterpret_cast<uintptr_t>(thisptr) + 0x18) = player;
	fmt::print("event::is_applicable_to_player for player: {} : {} - returning true\n",
		*reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(thisptr) + 0x30), player->GetName());
	return true;
}

bool __fastcall needs_reply(void* thisptr)
{
	fmt::print("event::needs_reply called - returning true\n");
	return true;
}

void* __fastcall serialise(rage::netGameEvent* event, datBitBuffer* buffer)
{
	fmt::print("event::serialise called - returning buffer\n");
	return buffer;
}

void fiber_script_event_crash(LPVOID lpparam)
{
	const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

	try
	{
		/*if(menu.selected_player < 0 || menu.selected_player > 32)
			SwitchToFiber(g_job_pool->job_fiber);

		CNetGamePlayer* net_player = nullptr;
		CNetGamePlayer* self = nullptr;
		typedef CNetGamePlayer* (__fastcall* get_net_player)(unsigned int pid);
		net_player = spoof_call<CNetGamePlayer*,
			unsigned int>(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<get_net_player>(menu.gnp_addy), menu.selected_player);
		if (net_player == nullptr)
			SwitchToFiber(g_job_pool->job_fiber);
		if (string(net_player->GetName()) == string("self"))
		{
			self = net_player;
			SwitchToFiber(g_job_pool->job_fiber);
		}

		if (net_player != nullptr)
			fmt::print("Name {} from PID {}\n", net_player->GetName(), net_player->m_PlayerID);
		else
			SwitchToFiber(g_job_pool->job_fiber);*/

		/*// allocate rage buffer
		uint8_t packetBuf[1024];

		// prepare event with buffer
		datBitBuffer buffer(packetBuf, sizeof(packetBuf));
		static_cast<rage::netGameEvent*>(sadv_event)->serialize(&buffer, net_player, net_player); // change source player?
		static_cast<rage::netGameEvent*>(sadv_event)->deserialize_extra_data(&buffer, false, net_player, net_player); // change player1?

		DWORD target_players_size{1};
		buffer.WriteDword(&target_players_size, 8);

		buffer.WriteDword(reinterpret_cast<DWORD*>(&net_player->m_PlayerID), 16);

		buffer.WriteDword(reinterpret_cast<DWORD*>(&static_cast<rage::netGameEvent*>(sadv_event)->m_eventId), 16);

		buffer.WriteDword(nullptr, 8);
		buffer.WriteDword(reinterpret_cast<DWORD*>(&static_cast<rage::netGameEvent*>(sadv_event)->m_id), 16);

		DWORD len = buffer.GetDataLength();
		buffer.WriteDword(&len, len);
		buffer.WriteDword(reinterpret_cast<DWORD*>(buffer.m_data), len);*/
		
		if (menu.selected_player < 0 || menu.selected_player > 32)
			SwitchToFiber(g_job_pool->job_fiber);

		CNetGamePlayer* net_player = nullptr;
		CNetGamePlayer* self = nullptr;
		typedef CNetGamePlayer* (__fastcall* get_net_player)(unsigned int pid);
		net_player = spoof_call<CNetGamePlayer*,
			unsigned int>(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<get_net_player>(menu.gnp_addy), menu.selected_player);
		if (net_player == nullptr)
			SwitchToFiber(g_job_pool->job_fiber);
		if (string(net_player->GetName()) == string("self"))
		{
			self = net_player;
			SwitchToFiber(g_job_pool->job_fiber);
		}

		if (net_player != nullptr)
			fmt::print("Name {} from PID {}\n", net_player->GetName(), net_player->m_PlayerID);
		else
			SwitchToFiber(g_job_pool->job_fiber);
	
		CNetGamePlayer* global_player = *reinterpret_cast<CNetGamePlayer**>(*reinterpret_cast<uintptr_t*>(
			PatternScan(reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
			"\x48\x8b\x05\xcc\xcc\xcc\xcc\x48\x8b\x88\xcc\xcc\xcc\xcc\x48\x89\x4b",
			"xxx????xxx????xxx", 17) + 0x183a7e0 + 7) + 232); // backup
		
		class sws_a1_vtbl
		{
		private:
			char pad_0x00[0x08]; // 0x00
		public:
			int v12_0x08() // 0x00
			{
				fmt::print("sws_a1_vtbl::v12_0x08 called\n");
				return INT32_MAX;
			};

			int (sws_a1_vtbl::* p_v12_0x08)() = &sws_a1_vtbl::v12_0x08; // 0x08
		private:
			char pad_0x10[0x28]; // 0x10
		public:
			sws_a1_vtbl* v8_0x38() // 0x0
			{
				fmt::print("sws_a1_vtbl::v8_0x28 called\n");
				return this;
			};
			
			sws_a1_vtbl* (sws_a1_vtbl::* pv8_0x38)() = &sws_a1_vtbl::v8_0x38; // 0x38
		private:
			char pad_0x40[0x50]; // 0x40
		public:
			int64_t v5_0x90() // 0x00
			{
				fmt::print("sws_a1_vtbl::v5_0x90 called\n");
				return INT64_MAX;
			};

			int64_t(sws_a1_vtbl::* pv5_0x90)() = &sws_a1_vtbl::v5_0x90; // 0x90
		};
#pragma pack(pop)
		static_assert(offsetof(sws_a1_vtbl, p_v12_0x08) == 0x08);
		static_assert(offsetof(sws_a1_vtbl, pv8_0x38) == 0x38);
		static_assert(offsetof(sws_a1_vtbl, pv5_0x90) == 0x90);
#pragma pack(push, 1)
		class sws_a1
		{
		};
#pragma pack(pop)

		sws_a1 _a1{};
		auto* a1 = &_a1;
		typedef void(__fastcall* SWS)(sws_a1* a1, int8_t bit, CNetGamePlayer* player); // event + 56 is bit that specifies whether it's a change or revert
		spoof_call<void, sws_a1*, int8_t, CNetGamePlayer*>(reinterpret_cast<void*>(menu.jmp_addy),
			reinterpret_cast<SWS>(PatternScan(
				reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
				"\x48\x8b\xc4\x48\x89\x58\xcc\x48\x89\x68\xcc\x48\x89\x70\xcc\x48\x89\x78\xcc\x41\x54\x41\x56\x41\x57\x48\x83\xec\xcc\x33\xdb\x4d\x8b\xf0\x44\x8a\xe2",
				"xxxxxx?xxx?xxx?xxx?xxxxxxxxx?xxxxxxxx", 37)), a1, 1 /* event + 56 */, net_player);
		fmt::print("SWS #1 sent\n");
		/*void* event_mgr = reinterpret_cast<void*>(PatternScan(reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
			"\x48\x8b\x0d\xcc\xcc\xcc\xcc\x48\x8d\x54\x24\xcc\x89\x44\x24\xcc\x8b\x84\x24\xcc\xcc\xcc\xcc\x4c\x8b\xce\x89\x44\x24\xcc\x4c\x8b\xc5\x66\x89\x5c\x24\xcc\x48\x89\x7c\x24\xcc\xe8\xcc\xcc\xcc\xcc\x48\x8d\x4c\x24\xcc\xe8\xcc\xcc\xcc\xcc\x4c\x8d\x9c\x24\xcc\xcc\xcc\xcc\x49\x8b\x5b\xcc\x49\x8b\x6b\xcc\x49\x8b\x73\xcc\x49\x8b\xe3\x5f\xc3\x90\x48\x8b\xc4\x48\x89\x58\xcc\x48\x89\x68\xcc\x48\x89\x70\xcc\x57\x48\x81\xec\xcc\xcc\xcc\xcc\x48\x8b\xe9\x48\x8d\x48\xcc\x41\x0f\xb7\xd9\x49\x8b\xf8\x48\x8b\xf2\xe8\xcc\xcc\xcc\xcc\x8b\x84\x24\xcc\xcc\xcc\xcc\x48\x8b\x0d\xcc\xcc\xcc\xcc\x48\x8d\x54\x24\xcc\x89\x44\x24\xcc\x8b\x84\x24\xcc\xcc\xcc\xcc\x4c\x8b\xce\x89\x44\x24\xcc\x4c\x8b\xc5\x66\x89\x5c\x24\xcc\x48\x89\x7c\x24\xcc\xe8\xcc\xcc\xcc\xcc\x48\x8d\x4c\x24\xcc\xe8\xcc\xcc\xcc\xcc\x4c\x8d\x9c\x24\xcc\xcc\xcc\xcc\x49\x8b\x5b\xcc\x49\x8b\x6b\xcc\x49\x8b\x73\xcc\x49\x8b\xe3\x5f\xc3\xcc\x48\x89\x5c\x24\xcc\x48\x89\x6c\x24\xcc\x48\x89\x74\x24\xcc\x57\x48\x81\xec\xcc\xcc\xcc\xcc\x48\x8b\xe9\x48\x8d\x4c\x24\xcc\x41\x0f\xb7\xd9\x49\x8b\xf8\x48\x8b\xf2\xe8\xcc\xcc\xcc\xcc\x8b\x84\x24\xcc\xcc\xcc\xcc\x48\x8b\x0d\xcc\xcc\xcc\xcc\x48\x8d\x54\x24\xcc\x89\x44\x24\xcc\x8b\x84\x24\xcc\xcc\xcc\xcc\x4c\x8b\xce\x89\x44\x24\xcc\x4c\x8b\xc5\x66\x89\x5c\x24\xcc\x48\x89\x7c\x24\xcc\xe8\xcc\xcc\xcc\xcc\x48\x8d\x4c\x24", 
			"xxx????xxxx?xxx?xxx????xxxxxx?xxxxxxx?xxxx?x????xxxx?x????xxxx????xxx?xxx?xxx?xxxxxxxxxxxx?xxx?xxx?xxxx????xxxxxx?xxxxxxxxxxx????xxx????xxx????xxxx?xxx?xxx????xxxxxx?xxxxxxx?xxxx?x????xxxx?x????xxxx????xxx?xxx?xxx?xxxxxxxxxx?xxxx?xxxx?xxxx????xxxxxxx?xxxxxxxxxxx????xxx????xxx????xxxx?xxx?xxx????xxxxxx?xxxxxxx?xxxx?x????xxxx",
			325) + 0x174d6b5 + 7);
		fmt::print("num event acks: {}\n",
			*reinterpret_cast<unsigned int*>((
				1912 * (*reinterpret_cast<CNetGamePlayer**>(0x10 + reinterpret_cast<uintptr_t>(sadv_event)))->m_PlayerID)
				+ (uintptr_t)event_mgr + 179216));
		fmt::print("num event replies: {}\n",
			*reinterpret_cast<unsigned int*>((
				1912 * (*reinterpret_cast<CNetGamePlayer**>(0x10 + reinterpret_cast<uintptr_t>(sadv_event)))->m_PlayerID)
				+ (uintptr_t)event_mgr + 180168));*/
		SwitchToFiber(g_job_pool->job_fiber);
	}
	catch (...)
	{
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		fmt::print(
			"Exception/crash occurred in {}\n",
			__FUNCTION__);
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		SwitchToFiber(g_job_pool->job_fiber);
	}
}

void fiber_se_crash_all(LPVOID lpParam)
{
	const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

	try
	{
		for (int i = 0; i <= 32; i++)
		{
			typedef void(__fastcall* tse)(int, int64_t[], int, int);
			typedef CNetGamePlayer* (__fastcall* get_net_player)(unsigned int pid);
			CNetGamePlayer* net_player = nullptr;
			net_player = spoof_call<CNetGamePlayer*,
				unsigned int>(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<get_net_player>(menu.gnp_addy), i);
			if (net_player == nullptr)
				continue;
			int64_t crash2[6] = { 442672710, 0, 0, -198237643587674, 0, -1 };
			spoof_call<void>(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<tse>(menu.tse_addy), 1, crash2, 6, 1 << i);
			int64_t crash3[6] = { 911331561, 0, 0, -1, -1, 0 };
			spoof_call<void>(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<tse>(menu.tse_addy), 1, crash3, 6, 1 << i);
			int64_t crash4[20] = { 544829784, menu.selected_player, -2387429374234, -9872346748634, 1923, -1, 92, 0, 1823, 934,
				-198249, 9283, 91824, -9834, 98234, 1984, -142, 0, 1, 0 };
			spoof_call<void>(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<tse>(menu.tse_addy), 1, crash4, 20, 1 << i);
			int64_t crash5[5] = { 493760552, 0, -97234797324, 0, 1 };
			spoof_call<void>(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<tse>(menu.tse_addy), 1, crash5, 5, 1 << i);
			int64_t crash1[6] = { 1310884765, 0, -198237643587674, -198237643587674, 0, -1 };
			spoof_call<void>(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<tse>(menu.tse_addy), 1, crash1, 6, 1 << i);
			fmt::print("Sent script event crashes to {}\n", net_player->GetName());
		}

		SwitchToFiber(g_job_pool->job_fiber);
	}
	catch (...)
	{
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		fmt::print(
			"Exception/crash occurred in {}\n",
			__FUNCTION__);
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		SwitchToFiber(g_job_pool->job_fiber);
	}
}

void fiber_crash_script_host(LPVOID lpParam)
{
	const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

	try
	{
		if (menu.selected_player < 0 || menu.selected_player > 32)
			SwitchToFiber(g_job_pool->job_fiber);
		typedef void(__fastcall* tse)(int, int64_t[], int, int);
		int64_t crash1[6] = { -1370208168, -97394273497, -1, -198237643587674, 0, -1 };
		spoof_call<void>(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<tse>(menu.tse_addy), 1, crash1, 6, 1 << menu.selected_player);
		int64_t crash2[4] = { -577731549, 8764238746942345, -982364962341, -198237643587674 };
		spoof_call<void>(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<tse>(menu.tse_addy), 1, crash2, 4, 1 << menu.selected_player);
		typedef CNetGamePlayer* (__fastcall* get_net_player)(unsigned int pid);
		CNetGamePlayer* net_player = nullptr;
		net_player = spoof_call<CNetGamePlayer*,
			unsigned int>(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<get_net_player>(menu.gnp_addy), menu.selected_player);
		if (net_player == nullptr)
			SwitchToFiber(g_job_pool->job_fiber);
		fmt::print("Sent script host crashes to {}\n", net_player->GetName());
		SwitchToFiber(g_job_pool->job_fiber);
	}
	catch (...)
	{
		SetConsoleTextAttribute(h_console, FOREGROUND_RED);
		fmt::print(
			"Exception/crash occurred in {}\n",
			__FUNCTION__);
		SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		SwitchToFiber(g_job_pool->job_fiber);
	}
}

//0x7FF62F89ECEB [C6 05 ? ? ? ? ? 48 8B CB E8 ? ? ? ? 84 C0 75 08]
//std::uintptr_t GTA_SMALL_HOST_TOKEN_PATCH = 0x7FF62F89ECEB;
//(bool)(GTA_SMALL_HOST_TOKEN_PATCH) = true;

void Menu::render_all_tab()
{
	char desync[] = "Desync";
	HiddenText desync_enc = desync;
	desync_enc.DecryptString();
	if (ImGui::Button(desync_enc.GetDecryptedString()))
		g_job_pool->add_job(fiber_desync);

	char se_crash_all[] = "Script Event Crash all players";
	HiddenText se_crash_all_enc = se_crash_all;
	se_crash_all_enc.DecryptString();
	if (ImGui::Button(se_crash_all_enc.GetDecryptedString()))
		g_job_pool->add_job(fiber_se_crash_all);

	char rde[] = "RDE";
	HiddenText rde_enc = rde;
	rde_enc.DecryptString();
	if (ImGui::Button(rde_enc.GetDecryptedString()))
		g_job_pool->add_job(fiber_request_door);
}

int current_tick = 0;
void render_players_tab()
{
	// update current player list every 100 ticks
	if(current_tick == 100)
	{
		player_list.net_player.clear();
		typedef CNetGamePlayer* (__fastcall* get_net_player)(unsigned int pid);
		CNetGamePlayer* net_player = nullptr;
		for (int i = 0; i <= 32; i++)
		{
			net_player = spoof_call<CNetGamePlayer*,
				unsigned int>(reinterpret_cast<void*>(menu.jmp_addy), reinterpret_cast<get_net_player>(menu.gnp_addy), i);
			if (net_player == nullptr)
				continue;
			player_list.add_net_player(net_player);
		}

		current_tick = 0;
	}
	else
		current_tick++;
	for(vector<int>::size_type i = 0; i != player_list.net_player.size(); i++)
	{
		if(ImGui::Button(string(to_string(i) + ". " + player_list.net_player[i]->GetName()).c_str()))
		{
			menu.selected_player = player_list.net_player[i]->m_PlayerID;
			g_job_pool->add_job(fiber_script_event_crash);
			//g_job_pool->add_job(fiber_crash_script_host);
		}
	}
}

void Menu::render()
{
	if (!m_has_hooked)
	{
		GameHooks hooks = GameHooks(menu);
		hooks.enable_rid0_hook();
		hooks.enable_pje_hook();
		hooks.enable_ple_hook();
		hooks.enable_rde_hook();
		hooks.enable_sge_hook();
		hooks.enable_neh_hook();
		hooks.enable_dlc_hook();
		m_has_hooked = true;
	}
	
	ImGui_ImplDX11_NewFrame();

	static bool no_titlebar = false;
	static bool no_border = true;
	static bool no_resize = false;
	static bool auto_resize = false;
	static bool no_move = false;
	static bool no_scrollbar = false;
	static bool no_collapse = false;
	static bool no_menu = true;
	static bool start_pos_set = false;

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.83f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.00f);
	colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.40f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.61f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.79f, 0.24f, 0.28f, 0.40f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.79f, 0.24f, 0.28f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	//colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_Separator] = ImVec4(0.250f, 0.250f, 0.255f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.250f, 0.250f, 0.255f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);

	ImGuiWindowFlags	window_flags = 0;
	if (no_titlebar)	window_flags |= ImGuiWindowFlags_NoTitleBar;
	if (no_resize)		window_flags |= ImGuiWindowFlags_NoResize;
	if (auto_resize)	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
	if (no_move)		window_flags |= ImGuiWindowFlags_NoMove;
	if (no_scrollbar)	window_flags |= ImGuiWindowFlags_NoScrollbar;
	if (no_collapse)	window_flags |= ImGuiWindowFlags_NoCollapse;
	if (!no_menu)		window_flags |= ImGuiWindowFlags_MenuBar;
	ImGui::SetNextWindowSize(ImVec2(450, 600), ImGuiSetCond_FirstUseEver);
	if (!start_pos_set) { ImGui::SetNextWindowPos(ImVec2(25, 25)); start_pos_set = true; }

	ImGui::GetIO().MouseDrawCursor = IsOpen;
	*reinterpret_cast<uint8_t*>(menu.m_addy + (0x29122a2 + 0x7)) = IsOpen;

	if (IsOpen)
	{
		ImGui::Begin("Kaizenn", &IsOpen, window_flags);

		const float ItemSpacing = ImGui::GetStyle().ItemSpacing.x;
		static float MiscButtonWidth = 100.0f;
		float pos = MiscButtonWidth + ItemSpacing;
		ImGui::SameLine(ImGui::GetWindowWidth() - pos);
		if (ImGui::Button("Misc")) currentTab = 4;
		MiscButtonWidth = ImGui::GetItemRectSize().x;

		static float ProtectionsButtonWidth = 100.0f;
		pos += ProtectionsButtonWidth + ItemSpacing;
		ImGui::SameLine(ImGui::GetWindowWidth() - pos);
		if (ImGui::Button("Protections")) currentTab = 3;
		ProtectionsButtonWidth = ImGui::GetItemRectSize().x;

		static float AllButtonWidth = 100.0f;
		pos += AllButtonWidth + ItemSpacing;
		ImGui::SameLine(ImGui::GetWindowWidth() - pos);
		if (ImGui::Button("Lobby")) currentTab = 2;
		AllButtonWidth = ImGui::GetItemRectSize().x;

		static float PlayerButtonWidth = 100.0f;
		pos += PlayerButtonWidth + ItemSpacing;
		ImGui::SameLine(ImGui::GetWindowWidth() - pos);
		if (ImGui::Button("Players")) currentTab = 1;
		PlayerButtonWidth = ImGui::GetItemRectSize().x;
		ImGui::Separator();

		switch(currentTab)
		{
		case 1:
			render_players_tab();
			break;
		case 2:
			render_all_tab();
			break;
		case 3:
			render_protections_tab();
			break;
		case 4:
			render_misc_tab();
			break;
		default:
			currentTab = 2;
		}

		ImGui::End();
	}
	
	ImGui::Render();
}