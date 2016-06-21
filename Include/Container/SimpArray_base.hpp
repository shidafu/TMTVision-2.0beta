/* \file SimpArray_base.h
   \brief Array of objects.
   \author Leon Contact: towanglei@163.com
   \copyright TMTeam
   \version 2.0
   \History:
      Leon 2016/06/18 16:57  Use template assigned static array\n
      1.0 Leon 2016/06/18 12:56 build\n
*/
#pragma once

#ifndef OBJECTARRAY_BASE
#define OBJECTARRAY_BASE

/* \class SimpArray
   \brief 
   \author Leon Contact: towanglei@163.com
   \version 1.0
   \date 2016/06/18 12:56
*/
template <typename T,int MAXARRAYSIZE = 256>
class SimpArray
{ 
public:
	T objects[MAXARRAYSIZE];///< Setting data
	int objectNum = 0; ///< Valid setting data number
	//SimpArray();
	//SimpArray(const T& initalVal);
	//SimpArray<T>& operator= (const SimpArray<T>& objectArray);
}; 
#endif
///////////////////////////////////////////////////