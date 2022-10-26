#pragma once
#include "stdafx.h"
#include "Menu.h"
#include <mutex>

//extern Menu menu;

class job_pool
{
public:
	HANDLE job_fiber;
	job_pool(Menu m);
	~job_pool() = default;
	void add_job(LPFIBER_START_ROUTINE job);
	void remove_all_jobs();
	void execute(LPFIBER_START_ROUTINE job);
	void execute_internal();
private:
	Menu _menu;
	recursive_mutex m_mutex;
	vector<LPFIBER_START_ROUTINE> m_jobs;
};