/** \file Macros.hpp
   \brief Definition some macro keywords, can form the interface class
   \author Leon Contact: towanglei@163.com
   \copyright TMTeam
   \version 1.0
   \History:
      Leon 2016/06/04 16:36 build\n
*/

#pragma once
//#ifndef INTERFACE_H
//#define INTERFACE_H
//#define interface class
////#define DeclareInterface(name) interface name { \
////public: \
////	virtual ~name() {}
////
////#define DeclareBasedInterface(name, base) class name : \
////public base { \
////public: \
////		virtual ~name() {}
////
////#define EndInterface };
//#define implements          public
//#endif

#ifndef HANDLE
typedef void *HANDLE;
//#define HANDLE              void*
#define INVALID_HANDLE      (HANDLE)-1
#define NULL_HANDLE         (HANDLE)0
#endif

#ifndef NULL_POINTER
#define NULL_POINTER        (void*)0
#endif

#ifndef DWORD
#define DWORD               unsigned long
#endif
#ifndef STR_LEN
#define STR_LEN(x)          (sizeof(x)/sizeof(x[0]))
#endif
#ifndef MaxULongInt
#define MaxULongInt         ((ULongInt)0xFFFFFFFF)
#endif
#ifndef MaxLongInt
#define MaxLongInt          ((LongInt) 0x7FFFFFFF)
#endif
#ifndef MIN
#define MIN(i,j)            i<j?i:j
#endif
#ifndef MAX
#define MAX(i,j)            i<j?j:i
#endif
#ifndef MOD
#define MOD(i,j)            i%j==j?0:i%j
#endif
#ifndef NULL
#define NULL                0
#endif

#ifndef TINYSTRLEN

#define TINYSTRLEN	        32
#define SHORTSTRLEN	        128
#define LONGSTRLEN	        256
#define PATHSTRLEN	        512
#define HUGESTRLEN	        1024
#define MEGASTRLEN	        4096
#define IPSTRLEN	        16

typedef wchar_t				TINYWSTR[TINYSTRLEN];
typedef wchar_t				SHORTWSTR[SHORTSTRLEN];
typedef wchar_t				LONGWSTR[LONGSTRLEN];
typedef wchar_t				PATHWSTR[PATHSTRLEN];
typedef wchar_t				HUGEWSTR[HUGESTRLEN];
typedef wchar_t				MEGAWSTR[MEGASTRLEN];
typedef wchar_t				NetIPW[IPSTRLEN];
typedef wchar_t*			WStrPointer;
typedef wchar_t				WStrChar;

typedef char				TINYSTR[TINYSTRLEN];
typedef char				SHORTSTR[SHORTSTRLEN];
typedef char				LONGSTR[LONGSTRLEN];
typedef char				PATHSTR[PATHSTRLEN];
typedef char				HUGESTR[HUGESTRLEN];
typedef char				MEGASTR[MEGASTRLEN];
typedef char				NetIP[IPSTRLEN];
typedef char*			    StrPointer;
typedef char				StrChar;

#endif