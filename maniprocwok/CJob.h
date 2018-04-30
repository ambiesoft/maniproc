#pragma once

class CJob
{
public:
	enum JobType {
		Job_SetPriority,
		Job_Suspend,
		Job_Resume,
		Job_Kill,
	} job_;
	
	CJob(const JobType& job) : job_(job) {

	}
	virtual ~CJob() {}

	virtual bool DoWork() = 0;
};


class CPriorityJob : public CJob
{
	DWORD priority_;
	std::vector<HANDLE> handles_;
public:
	CPriorityJob(const std::vector<HANDLE>& handles, DWORD priority) :
		CJob(Job_SetPriority),
		handles_(handles),
		priority_(priority)
	{

	}

	bool DoWork();
};