#pragma once
#include "stdafx.h"
#include "Menu.h"
#include "job_pool.h"

extern unique_ptr<job_pool> g_job_pool;
extern "C" uintptr_t sge_return_addy;
extern "C" void __fastcall SGE1();

extern Menu menu;
extern PlayerList player_list;

class GameHooks
{
public:
	GameHooks(Menu m);
	~GameHooks();
	void enable_rid0_hook();
	void disable_rid0_hook();
	void enable_pje_hook();
	void disable_pje_hook();
	void enable_ple_hook();
	void disable_ple_hook();
	void enable_rde_hook();
	void disable_rde_hook();
	void enable_sge_hook();
	void disable_sge_hook();
	void enable_neh_hook();
	void disable_neh_hook();
	void enable_dlc_hook();
	void disable_dlc_hook();
private:
	Menu _menu;
};