/* \file PointerPool_base.hpp
   \brief Pointer pool for store and find pointer
   \author Leon Contact: towanglei@163.com
   \copyright TMTeam
   \version 2.0
   \History:
      Leon 2016/06/18 16:57  Use template assigned static array\n
      1.0 Leon 2016/06/12 8:07 build\n
*/
#pragma once
#ifndef POINTERPOOL_BASE
#define POINTERPOOL_BASE
//#include "..\Macros.hpp"

#ifndef NULL_POINTER
#define NULL_POINTER        (void*)0
#endif
#ifndef MIN
#define MIN(i,j)            i<j?i:j
#endif
#ifndef MAX
#define MAX(i,j)            i<j?j:i
#endif

/* \class PointerPool_base : 
   \brief PointerPool_base use static array.
   \author Leon Contact: towanglei@163.com
   \version 1.0
   \date 2016/06/18 16:57
*/
class PointerPool_base
{
public:
	enum { MAXPOOLSIZE = 256 };
private:
	/// Pointer stored as array.
	void* pointerArray[MAXPOOLSIZE] = { NULL_POINTER };
	/// Stored pointer num.
	int pointerNum = 0;
public:
	/// Default constructor.
	PointerPool_base() 
	{
	};
	/// Add pointer in null position of array.
	bool Add(void* pointer)
	{
		if (pointerNum >= MAXPOOLSIZE) return false;
		int rtVal = Find(pointer);
		if (rtVal != -1) return true;//Already included.
		for (int i = 0; i < MAXPOOLSIZE; i++)
		{
			if (pointerArray[i] == NULL_POINTER)
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
		int rtVal = Find(pointer);
		if (rtVal == -1) return false;
		pointerArray[rtVal] = NULL_POINTER;
		if (rtVal == beginIndex)
		{
			beginIndex = Next(beginIndex);
		}
		if (rtVal == endIndex)
		{
			endIndex = Previous(endIndex);
		}
		pointerNum--;
		return true;
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
	/// Clear pointer array,careful use.
	void Clear()
	{
		if (pointerNum <= 0) return;
		for (int i = beginIndex; i <= endIndex; i++)
		{
			pointerArray[i] = NULL_POINTER;
		}
		pointerNum = 0;
	}

private:
	/// Iterator start index.
	int beginIndex = MAXPOOLSIZE;
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
		if (pointerNum <= 1) return MAXPOOLSIZE;
		for (int i = MAX(index, beginIndex); i <= endIndex; i++)
		{
			if (pointerArray[i] != NULL_POINTER)
			{
				return i;
			}
		}
		return MAXPOOLSIZE;
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

/* \class PointerPool_singleton :
\brief PointerPool_singleton use static array.
\author Leon Contact: towanglei@163.com
\version 1.0
\date 2016/06/20 16:57
*/
class PointerPool_singleton
{
private:
	/// Pointer stored as array.
	PointerPool_base pointerPool;
	/// Default constructor.
	PointerPool_singleton() 
	{
	};
public:
	/// Add pointer in null position of array.
	bool Add(void* pointer)
	{
		return pointerPool.Add(pointer);
	}
	/// Find and delete pointer from array.
	bool Del(void* pointer)
	{
		return pointerPool.Del(pointer);
	}
	/// Find pointer from array, and return index.
	int Find(void* pointer)
	{
		return pointerPool.Find(pointer);
	}
	/// Clear pointer array,careful use.
	void Clear()
	{
		pointerPool.Clear();
	}
	/// Get pointer from index.
	void* At(int index)
	{
		return pointerPool.At(index);
	}
	/// Get Iterator start index.
	int Begin()
	{
		return pointerPool.Begin();
	}
	/// Get Iterator end index.
	int End()
	{
		return pointerPool.End();
	}
	/// Foreword Iterator.
	int Next(int index)
	{
		return pointerPool.Next(index);
	}
	/// Backward Iterator.
	int Previous(int index)
	{
		return pointerPool.Previous(index);
	}
public:
	static PointerPool_singleton * GetInstance()
	{
		static PointerPool_singleton instance;
		return &instance;
	}
};

#endif