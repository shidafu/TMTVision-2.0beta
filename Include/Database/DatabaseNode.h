///////////////////////////////////////////////////
/** \file DatabaseNode.h
 *  \brief Implement database operations in INode object 
 *  \author Leon Contact: towanglei@163.com
 *  \copyright TMTeam
 *  \version 1.0
 *  \History:
 *     Leon 2016/06/17 8:14 build\n
 */
///////////////////////////////////////////////////
#pragma once
#include "..\Macros.h"
#include "..\INode.h"
#include "..\DebugTools.h"

//////////////////////////////////////////////////
/** \struct DBIOFormat
 *  \brief SOL format ask string, json format answer result.
 *  \author Leon Contact: towanglei@163.com
 *  \version 1.0
 *  \date 2016/06/17 8:17
 */
struct DBIOFormat
{ 
	/// Database options
	enum DBOption
	{
		DBSearch = (int)1,
		DBAdd = (int)2,
		DBUpdate = (int)3,
		DBDelete = (int)4
	};
	DBOption dbOption = DBOption::DBSearch;
	bool returnValue = false;
	HUGESTR askSqlStr = ""; ///< SOL format ask string.
	MEGASTR answerJsonStr = ""; ///< json format answer result.
	DBIOFormat() {};
	DBIOFormat& operator=(const DBIOFormat& dbIOFormat)
	{
		try
		{
			strcpy_s(askSqlStr, sizeof(askSqlStr), dbIOFormat.askSqlStr);
			strcpy_s(answerJsonStr, sizeof(answerJsonStr), dbIOFormat.answerJsonStr);
			returnValue = dbIOFormat.returnValue;
		}
		catch (...)
		{
			OutputDebugStringEx("DBIOFormat::Operator=().", LogLevel::debugLevel);
		}
		return *this;
	}
};
///////////////////////////////////////////////////

//////////////////////////////////////////////////
/** \enum DBERRCODE
 *  \brief 
 *  \author Leon Contact: towanglei@163.com
 *  \version 1.0
 *  \date 2016/06/18 19:07
 */
enum DBERRCODE
{
	DBOK = (int)0 ,
	DBERR = (int)1
};
//////////////////////////////////////////////////

///////////////////////////////////////////////////
/** \class DatabaseNode : 
 *  \brief Implement database operations in INode object
 *  \author Leon Contact: towanglei@163.com
 *  \version 1.0
 *  \date 2016/06/17 8:14
 */
 ///////////////////////////////////////////////////
template <typename T1 = DBIOFormat, typename T2 = DBIOFormat >
class DatabaseNode : public IProcessor<T1, T2>
{
protected:
	MEGASTR dbConnectStr = "";
	TINYSTR dbProvider = "";
	PATHSTR dbServerPath = "";
	SHORTSTR dbName = "";
	SHORTSTR dbUser = "";
	TINYSTR dbPassword = "";
	bool m_bConnected = false;
public:
	/// Default constructor
	DatabaseNode();
	/** \fn  ~DatabaseNode
	*  \brief virtual destruct function, avoid delete wrong object
	*/
	virtual ~DatabaseNode();
	/** \fn  Set
	*  \brief Set para without stop
	*  \param[in] char* setting as json string
	*  \return bool
	*/
	virtual bool Set(char* setting, long settingLen);
	/** \fn  Get
	*  \brief Get para without stop
	*  \param[in] char* setting as json string
	*  \return bool
	*/
	virtual bool Get(char* setting, long settingLen);
//public:
//	/** \fn  Click
//	*  \brief Click node, run one time
//	*  \return bool
//	*/
//	bool Click(bool isSync);
};