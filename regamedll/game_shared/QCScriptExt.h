/*
* 
Created Date: Jul 13 2020

Modern Warfare Dev Team
	Code - Luna the Reborn

*/

#pragma once
#ifndef _QC_SCRIPT_EXT_
#define _QC_SCRIPT_EXT_

#include <list>

namespace QCScript
{
	using namespace std;

	struct CBaseOperation
	{
		virtual bool Act(void) { return false; }
	};

	using OperationSet = list<CBaseOperation*>;

	inline void Run(OperationSet& lst)
	{
		for (auto& op : lst)
		{
			op->Act();
		}
	}

	template<typename T>
	struct CValueAssignment : public CBaseOperation
	{
		CValueAssignment(T* ptr, T value) :
			m_pValueAddr(ptr),
			m_ConstTargetValue(value)
		{
		}

		bool Act(void) final
		{
			*m_pValueAddr = m_ConstTargetValue;
			return true;
		}

		T* m_pValueAddr{ nullptr };
		T m_ConstTargetValue;
	};
};










































#endif // !_QC_SCRIPT_EXT_
