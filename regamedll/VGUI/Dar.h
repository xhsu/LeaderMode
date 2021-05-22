#ifndef DAR_H
#define DAR_H

#ifdef _WIN32
#pragma once
#endif

#include <stdlib.h>
#include <string.h>
#include <VGUI/VGUI.h>
#include <tier1/utlvector.h>

namespace vgui
{

template<class ELEMTYPE> class Dar : public CUtlVector<ELEMTYPE>
{
	typedef CUtlVector<ELEMTYPE> BaseClass;
	
public:
	Dar(void)
	{
	}

	Dar(int initialCapacity) : BaseClass(0, initialCapacity)
	{
	}

public:
	void SetCount(int count)
	{
		BaseClass::EnsureCount(count);
	}

	int GetCount(void)
	{
		return BaseClass::Count();
	}

	int AddElement(ELEMTYPE elem)
	{
		return AddToTail(elem);
	}

	void MoveElementToEnd(ELEMTYPE elem)
	{
		if (BaseClass::Count() == 0)
			return;

		if (Element(BaseClass::Count() - 1) == elem)
			return;

		int idx = Find(elem);

		if (idx == BaseClass::InvalidIndex())
			return;

		Remove(idx);
		AddToTail(elem);
	}

	int FindElement(ELEMTYPE elem)
	{
		return Find(elem);
	}

	bool HasElement(ELEMTYPE elem)
	{
		if (FindElement(elem) != BaseClass::InvalidIndex())
			return true;

		return false;
	}

	int PutElement(ELEMTYPE elem)
	{
		int index = FindElement(elem);

		if (index >= 0)
			return index;

		return AddElement(elem);
	}

	void InsertElementAt(ELEMTYPE elem, int index)
	{
		InsertBefore(index, elem);
	}

	void SetElementAt(ELEMTYPE elem, int index)
	{
		BaseClass::EnsureCount(index + 1);
		BaseClass::Element(index) = elem;
	}

	void RemoveElementAt(int index)
	{
		BaseClass::Remove(index);
	} 

	void RemoveElementsBefore(int index)
	{
		if (index <= 0)
			return;

		BaseClass::RemoveMultiple(0, index - 1);
	}

	void RemoveElement(ELEMTYPE elem)
	{
		FindAndRemove(elem);
	}

	void *GetBaseData(void)
	{
		return BaseClass::Base();
	}

	void CopyFrom(Dar<ELEMTYPE> &dar)
	{
		CoypArray(dar.Base(), dar.Count());
	}
};
}

#endif
