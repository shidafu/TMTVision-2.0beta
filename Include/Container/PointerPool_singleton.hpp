/* \file PointerPool_singleton.hpp
   \brief Pointer pool for store and find pointer
   \author Leon Contact: towanglei@163.com
   \copyright TMTeam
   \version 2.0
   \History:
      Leon 2016/06/18 16:57  Use template assigned static array\n
      1.0 Leon 2016/06/12 8:07 build\n
*/
#pragma once
#ifndef POINTERPOOL_SINGLETON
#define POINTERPOOL_SINGLETON
#include "PointerPool_templated.hpp"

#ifndef NULL_POINTER
#define NULL_POINTER        (void*)0
#endif
#ifndef MIN
#define MIN(i,j)            i<j?i:j
#endif
#ifndef MAX
#define MAX(i,j)            i<j?j:i
#endif

/* \class PointerPool_singleton :
\brief singleton PointerPool.
\author Leon Contact: towanglei@163.com
\version 1.0
\date 2016/06/20 16:57
*/
class PointerPool_singleton
{
private:
	/// Pointer stored as array.
	PointerPool_templated<256> pointerPool;
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