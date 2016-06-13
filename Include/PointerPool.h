///////////////////////////////////////////////////
/** \file PointerPool.h
 *  \brief Pointer pool for store and find pointer
 *  \author Leon Contact: towanglei@163.com
 *  \copyright TMTeam
 *  \version 1.0
 *  \History:
 *     Leon 2016/06/12 8:07 build\n
 */
///////////////////////////////////////////////////
#pragma once
#include "Macros.h"
class PointerPool
{
private:
	enum 
	{
		MAXPOINTERNUM=512 ///< Max pointer num in pool.
	};
	/// Pointer stored as array.
	void* pointerArray[MAXPOINTERNUM] = { NULL_POINTER };
	/// Stored pointer num.
	int pointerNum = 0;

public:
	/// Default constructor.
	PointerPool() {};
	/// Default destructor.
	~PointerPool() {};
	/// Add pointer in null position of array.
	bool Add(void* pointer)
	{
		if (pointerNum >= MAXPOINTERNUM) return false;
		for (int i=0;i<MAXPOINTERNUM;i++)
		{
			if (pointerArray[i]==NULL_POINTER)
			{
				pointerArray[i] = pointer;
				pointerNum++;
				beginIndex = MIN(beginIndex, i);
				endIndex = MAX(endIndex, i);
				return true;
			}
		}
		return false;
	}
	/// Find and delete pointer from array.
	bool Del(void* pointer)
	{
		if (pointerNum <= 0) return false;
		int findNum = 0;
		for (int i = beginIndex; i <= endIndex; i++)
		{
			if (pointerArray[i] != NULL_POINTER)
			{
				if (pointerArray[i] == pointer)
				{
					pointerArray[i] = NULL_POINTER;
					pointerNum--;
					if (i == beginIndex)
					{
						beginIndex = Next(beginIndex);
					}
					if (i == endIndex)
					{
						endIndex = Previous(endIndex);
					}
					return i;
				}
				findNum++;
			}
			if (findNum >= pointerNum) return false;
		}
		return false;
	}
	/// Find pointer from array, and return index.
	int Find(void* pointer)
	{
		if (pointerNum <= 0) return -1;
		int findNum = 0;
		for (int i = beginIndex; i <= endIndex; i++)
		{
			if (pointerArray[i] != NULL_POINTER)
			{
				if (pointerArray[i] == pointer)
				{
					return i;
				}
				findNum++;
			}
			if (findNum >= pointerNum) return -1;
		}
		return -1;
	}

private:
	/// Iterator start index.
	int beginIndex = MAXPOINTERNUM;
	/// Iterator end index.
	int endIndex = -1;

public:
	/// Get pointer from index.
	void* At(int index)
	{
		return pointerArray[index];
	}
	/// Get Iterator start index.
	int Begin()
	{
		return beginIndex;
	}
	/// Get Iterator end index.
	int End()
	{
		return endIndex;
	}
	/// Foreword Iterator.
	int Next(int index)
	{
		if (pointerNum <= 1) return MAXPOINTERNUM;
		for (int i = MAX(index,beginIndex); i <= endIndex; i++)
		{
			if (pointerArray[i] != NULL_POINTER)
			{
				return i;
			}
		}
		return MAXPOINTERNUM;
	}
	/// Backward Iterator.
	int Previous(int index)
	{
		if (pointerNum <= 1) return -1;
		for (int i = MIN(index, endIndex); i >= beginIndex; i--)
		{
			if (pointerArray[i] != NULL_POINTER)
			{
				return i;
			}
		}
		return -1;
	}
};

