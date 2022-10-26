#include "job_pool.h"

job_pool::job_pool(Menu m)
{
	_menu = m;
}

void job_pool::add_job(LPFIBER_START_ROUTINE job)
{
	lock_guard lock(m_mutex);
	m_jobs.push_back(job);
}

void job_pool::remove_all_jobs()
{
	lock_guard lock(m_mutex);
	m_jobs.clear();
}

void job_pool::execute(LPFIBER_START_ROUTINE job)
{
	HANDLE fiber = CreateFiber(NULL, job, job_fiber);
	if (!fiber)
		return;
	SwitchToFiber(fiber);
	cout << "Return from job to job_pool::execute" << endl;
}

void job_pool::execute_internal()
{
	if (!IsThreadAFiber())
		job_fiber = ConvertThreadToFiber(nullptr);
	else
		job_fiber = GetCurrentFiber();
	lock_guard lock(m_mutex);
	if (m_jobs.size() <= 0)
		return;
	for(auto const job : m_jobs)
	{
		cout << "got job" << endl;
		execute(job);
	}
}
