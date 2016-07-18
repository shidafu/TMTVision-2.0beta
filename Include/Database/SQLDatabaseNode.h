///////////////////////////////////////////////////
/** \file SQLDatabaseNode.h
 *  \brief Implement database operations in INode object 
 *  \author Leon Contact: towanglei@163.com
 *  \copyright TMTeam
 *  \version 1.0
 *  \History:
 *     Leon 2016/06/17 8:14 build\n
 */
///////////////////////////////////////////////////
#pragma once
#include "DatabaseNode.h"
#include "Container\ObjectArray.h"
// import ado lib
#pragma   warning   (disable:4146)  
#import ".\..\Lib\DataBase\Ado\msado15.dll" no_namespace /*named_guids rename_namespace("ADOCG")*/  rename("EOF","adoEOF") rename("BOF","adoBOF")
#pragma warning(default:4146)
//using namespace ADOCG;

//////////////////////////////////////////////////
/** \struct TableField
 *  \brief 
 *  \author Leon Contact: towanglei@163.com
 *  \version 1.0
 *  \date 2016/06/18 13:21
 */
///////////////////////////////////////////////////
struct TableField
{
	SHORTSTR strFieldName = "";
	int nFieldType = 0;
	int nFieldLength = 0;
};

///////////////////////////////////////////////////
/** \class SQLDatabaseNode:
*  \brief Implement database operations in INode object
*  \author Leon Contact: towanglei@163.com
*  \version 1.0
*  \date 2016/06/17 8:14
*/
///////////////////////////////////////////////////
template <typename T1 = DBIOFormat, typename T2 = DBIOFormat >
class SQLDatabaseNode: public DatabaseNode<T1, T2>
{
private:
	_ConnectionPtr  m_pConnection;
	_ConnectionPtr  m_pConnectionSearch;
	_CommandPtr		m_pCommand;
	_RecordsetPtr   m_pRecordsetAdd;
	LONGSTR         m_pRecordsetAddStr = "SELECT * from %s where %s";
	_RecordsetPtr   m_pRecordsetSearch;
	LONGSTR         m_pRecordsetSearchStr = "SELECT * from %s where %s";
	ObjectArray<SHORTSTR>  m_tableName;
	ObjectArray<TableField>  m_tableFields;
public:
	/// Default constructor
	SQLDatabaseNode();
	/** \fn  ~SQLDatabaseNode
	*  \brief virtual destruct function, avoid delete wrong object
	*/
	virtual ~SQLDatabaseNode();
	/** \fn  Initial
	*  \brief Initial DataBuffer
	*  \param[in] char* setting as json string
	*  \return bool
	*/
	virtual bool Initial(char* setting, long settingLen);
	/** \fn  Unitial
	*  \brief Unitial DataBuffer
	*/
	virtual void Unitial();

public:
	/** \fn  Process
	*  \brief Process data and return.
	*  \param[in] T1& dataIn, T1 must has deep copy operator=
	*  \param[out] T2& dataOut, T2 must has deep copy operator=
	*  \return False if failed.
	*/
	virtual bool Process(DBIOFormat & dataIn, DBIOFormat & dataOut);
};
