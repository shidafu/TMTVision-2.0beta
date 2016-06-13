///////////////////////////////////////////////////
/** \file Macros.h
 *  \brief Definition some macro keywords, can form the interface class
 *  \author Leon Contact: towanglei@163.com
 *  \copyright TMTeam
 *  \version 1.0
 *  \History:
 *     Leon 2016/06/04 16:36 build\n
 */
///////////////////////////////////////////////////
#ifndef INTERFACE_H
#define INTERFACE_H
#define interface class
//#define DeclareInterface(name) interface name { \
//public: \
//	virtual ~name() {}
//
//#define DeclareBasedInterface(name, base) class name : \
//public base { \
//public: \
//		virtual ~name() {}
//
//#define EndInterface };
#define implements public
#endif

#ifndef HANDLE
#define HANDLE void*
#define INVALID_HANDLE  (HANDLE)-1
#define NULL_HANDLE     (HANDLE)0
#endif

#ifndef NULL_POINTER
#define NULL_POINTER  (void*)0
#endif

#ifndef DWORD
#define DWORD unsigned long
#endif
#ifndef STR_LEN
#define STR_LEN(x)  (sizeof(x)/sizeof(x[0]))
#endif
#ifndef MaxULongInt
#define MaxULongInt   ((ULongInt)0xFFFFFFFF)
#endif
#ifndef MaxLongInt
#define MaxLongInt    ((LongInt) 0x7FFFFFFF)
#endif
#ifndef MIN
#define MIN(i,j) i<j?i:j
#endif
#ifndef MAX
#define MAX(i,j) i<j?j:i
#endif
#ifndef MOD
#define MOD(i,j) i%j==j?0:i%j
#endif
#ifndef NULL
#define NULL 0
#endif