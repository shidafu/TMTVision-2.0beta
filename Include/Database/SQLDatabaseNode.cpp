#include "SQLDatabaseNode.h"
#include "..\DebugTools.h"
#include <boost\lexical_cast.hpp>
#include <boost\format.hpp>

template <typename T1 = DBIOFormat, typename T2 = DBIOFormat >
SQLDatabaseNode<T1, T2>::SQLDatabaseNode()
{
}

template <typename T1 = DBIOFormat, typename T2 = DBIOFormat >
SQLDatabaseNode<T1, T2>::~SQLDatabaseNode()
{
}

template <typename T1 = DBIOFormat, typename T2 = DBIOFormat >
bool SQLDatabaseNode<T1, T2>::Initial(char * setting, long settingLen)
{
	bool rtVal = INode::Initial(setting, settingLen);
	try
	{
		m_pConnection->Close();
		m_tableName.objectNum = 0;
		m_tableFields.objectNum = 0;
		//Connect DB.
		strcpy_s(dbProvider, sizeof(dbProvider), "SQLOLEDB");
		format fm("Provider=SQLOLEDB.1;Password=%s;Persist Security Info=True; User ID=%s;Initial Catalog=%s;Data Source=%s");
		fm % dbPassword % dbUser % dbName % dbServerPath;
		strcpy_s(dbConnectStr, sizeof(dbConnectStr), fm.str().c_str());
		m_bConnected = false;
		m_pConnection->CursorLocation = adUseClient;
		HRESULT hr = m_pConnection->Open(_bstr_t(dbConnectStr), L"", L"", -1);
		if (!SUCCEEDED(hr)) throw(DBERRCODE::DBERR);

		//Get table list.
		_RecordsetPtr pSet;
		pSet = m_pConnection->OpenSchema(adSchemaTables);
		while (!(pSet->adoEOF))
		{
			//Get table.
			_bstr_t table_name = pSet->Fields->GetItem("TABLE_NAME")->Value;
			//Get table type.
			_bstr_t table_type = pSet->Fields->GetItem("TABLE_TYPE")->Value;
			//Filter to leave table name.
			if (strcmp(((LPCSTR)table_type), "TABLE") == 0)
			{
				strcpy_s(m_tableName.objects[m_tableName.objectNum],
					sizeof(m_tableName.objects[m_tableName.objectNum]), (LPCSTR)table_name);
				m_tableName.objectNum++;
			}
			pSet->MoveNext();
		}
		pSet->Close();

		//Create instance
		m_pConnectionSearch.CreateInstance(__uuidof(Connection));
		m_pConnectionSearch->CursorLocation = adUseClient;
		hr = m_pConnectionSearch->Open(_bstr_t(dbConnectStr), L"", L"", -1);
		if (!SUCCEEDED(hr))
		{
			throw(DBERRCODE::DBERR);
		}
		m_pCommand.CreateInstance(__uuidof(Command));
		m_pCommand->ActiveConnection = m_pConnection;
		m_pCommand->CommandType = adCmdText;
		m_pRecordsetAdd.CreateInstance(__uuidof(Recordset));
		m_pRecordsetSearch.CreateInstance(__uuidof(Recordset));
		m_bConnected = true;
	}
	catch (DBERRCODE errCode)
	{
		if (m_pRecordsetAdd)
		{
			if (m_pRecordsetAdd->GetState()) m_pRecordsetAdd->Close();
		}
		if (m_pRecordsetSearch)
		{
			if (m_pRecordsetSearch->GetState()) m_pRecordsetSearch->Close();
		}
		m_pConnection->Close();
		m_tableName.objectNum = 0;
		m_tableFields.objectNum = 0;
		OutputDebugStringEx("SQLDatabaseNode::Initial().", LogLevel::debugLevel);
		return false;
	}
	catch (...)
	{
		if (m_pRecordsetAdd)
		{
			if(m_pRecordsetAdd->GetState()) m_pRecordsetAdd->Close();
		}		
		if (m_pRecordsetSearch)
		{
			if (m_pRecordsetSearch->GetState()) m_pRecordsetSearch->Close();
		}
		m_pConnection->Close();
		m_tableName.objectNum = 0;
		m_tableFields.objectNum = 0;
		OutputDebugStringEx("SQLDatabaseNode::Initial().", LogLevel::debugLevel);
		return false;
	}
	return rtVal;
}

template <typename T1 = DBIOFormat, typename T2 = DBIOFormat >
void SQLDatabaseNode<T1, T2>::Unitial()
{
	try
	{
		if (m_pRecordsetAdd)
		{
			if (m_pRecordsetAdd->GetState()) m_pRecordsetAdd->Close();
		}
		if (m_pRecordsetSearch)
		{
			if (m_pRecordsetSearch->GetState()) m_pRecordsetSearch->Close();
		}
		m_pConnection->Close();
		m_tableName.objectNum = 0;
		m_tableFields.objectNum = 0;
		m_bConnected = false;
	}
	catch (...)
	{
		OutputDebugStringEx("SQLDatabaseNode::Unitial().", LogLevel::debugLevel);
		return;
	}
	INode::Unitial();
}

template <typename T1 = DBIOFormat, typename T2 = DBIOFormat >
bool SQLDatabaseNode<T1, T2>::Process(DBIOFormat & dataIn, DBIOFormat & dataOut)
{
	if (!m_bConnected) return false;
	try
	{
		dataOut = dataIn;
		if (dataIn.dbOption == DBIOFormat::DBSearch)
		{
			long state = m_pRecordsetSearch->GetState();
			if (state)
			{
				m_pRecordsetSearch->Close();
			}
			_variant_t vra;
			VARIANT *vt1 = NULL;
			m_pRecordsetSearch->Open(dataIn.askSqlStr, _variant_t((IDispatch *)m_pConnectionSearch, true), adOpenDynamic, adLockPessimistic, adCmdText);
			long lRecordCount = 0;
			lRecordCount = m_pRecordsetSearch->GetRecordCount();
			if (lRecordCount <= 0)
			{
				m_pRecordsetSearch->Close();
				dataOut.returnValue = false;
				return false;
			}

			// Get table fields.
			FieldsPtr fields = m_pRecordsetSearch->GetFields();
			for (long i = 0; i < fields->Count; i++)
			{
				FieldPtr field = fields->Item[i];
				TableField curField;
				strcpy_s(curField.strFieldName, sizeof(curField.strFieldName), field->Name);
				curField.nFieldType = field->Type;
				curField.nFieldLength = field->NumericScale;
				m_tableFields.objects[m_tableFields.objectNum] = curField;
				m_tableFields.objectNum++;
			}

			dataOut.answerJsonStr[0] = 0;
			m_pRecordsetSearch->MoveFirst();
			int iRecordIndex = 0;
			while (!m_pRecordsetSearch->adoEOF)
			{
				for (int i = 0; i < m_tableFields.objectNum; i++)
				{
					_variant_t Value;
					Value = m_pRecordsetSearch->GetCollect(m_tableFields.objects[i].strFieldName);
					//switch (m_tableFields.ObjectArray[i].nFieldType)
					//{
					//case:
					//	break;
					//case:
					//	break;
					//case:
					//	break;
					//case:
					//	break;
					//case:
					//	break;
					//default:
					//	break;
					//}
				}

				//Record.iSequeceNo = Value.intVal;
				//Value = m_pRecordSetSearch->GetCollect("ImageNo");
				//Record.iImageNo = Value.intVal;
				//Value = m_pRecordSetSearch->GetCollect("SteelID");
				//CCommonFunc::SafeStringPrintf(Record.strID, STR_LEN(Record.strID), L"%s", Value.bstrVal);

				//Value = m_pRecordSetSearch->GetCollect("TopImageError");
				//Record.iTopImageError = Value.iVal;
				//Value = m_pRecordSetSearch->GetCollect("TopLen");
				//Record.iTopLen = Value.intVal;
				//Value = m_pRecordSetSearch->GetCollect("TopWidth");
				//Record.iTopWidth = Value.iVal;
				//Value = m_pRecordSetSearch->GetCollect("TopDefectNum");
				//Record.iTopDefectNum = Value.iVal;
				//Value = m_pRecordSetSearch->GetCollect("TopDetectTime");
				//SYSTEMTIME tm;
				//VariantTimeToSystemTime(Value.date, &tm);
				//Record.TopDetectTime.iYear = tm.wYear;
				//Record.TopDetectTime.iMonth = (TinyInt)tm.wMonth;
				//Record.TopDetectTime.iDay = (TinyInt)tm.wDay;
				//Record.TopDetectTime.iHour = (TinyInt)tm.wHour;
				//Record.TopDetectTime.iMinute = (TinyInt)tm.wMinute;
				//Record.TopDetectTime.iSecond = (TinyInt)tm.wSecond;
				//Record.TopDetectTime.iMillSecond = tm.wMilliseconds;

				//Value = m_pRecordSetSearch->GetCollect("BottomImageError");
				//Record.iBottomImageError = Value.iVal;
				//Value = m_pRecordSetSearch->GetCollect("BottomLen");
				//Record.iBottomLen = Value.intVal;
				//Value = m_pRecordSetSearch->GetCollect("BottomWidth");
				//Record.iBottomWidth = Value.iVal;
				//Value = m_pRecordSetSearch->GetCollect("BottomDefectNum");
				//Record.iBottomDefectNum = Value.iVal;
				//Value = m_pRecordSetSearch->GetCollect("BottomDetectTime");
				//VariantTimeToSystemTime(Value.date, &tm);
				//Record.BottomDetectTime.iYear = tm.wYear;
				//Record.BottomDetectTime.iMonth = (TinyInt)tm.wMonth;
				//Record.BottomDetectTime.iDay = (TinyInt)tm.wDay;
				//Record.BottomDetectTime.iHour = (TinyInt)tm.wHour;
				//Record.BottomDetectTime.iMinute = (TinyInt)tm.wMinute;
				//Record.BottomDetectTime.iSecond = (TinyInt)tm.wSecond;
				//Record.BottomDetectTime.iMillSecond = tm.wMilliseconds;
				//iRecordIndex++;
				//if (iRecordIndex >= iMaxRecordNum)
				//{
				//	break;
				//}
				m_pRecordsetSearch->MoveNext();
			}
			m_pRecordsetSearch->Close();
		}
		else if (dataIn.dbOption == DBIOFormat::DBAdd)
		{
		}
		else if (dataIn.dbOption == DBIOFormat::DBUpdate)
		{
		}
		else if (dataIn.dbOption == DBIOFormat::DBDelete)
		{
		}
		else
		{
		}
	}
	catch (...)
	{
		long state = m_pRecordsetSearch->GetState();
		if (state)
		{
			m_pRecordsetSearch->Close();
		}
		OutputDebugStringEx("SQLDatabaseNode::Process().", LogLevel::debugLevel);
		return false;
	}
	return true;
}