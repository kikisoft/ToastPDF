/**
	@file
	@brief 
*/

/*
 * Copyright (C) 2007 Guy Hachlili <hguy@cogniview.com>
 * 
 * Used with permission in CC PDF Converter
 *
 */


#include "precomp.h"
#include "SQLiteDB.h"

namespace SQLite
{

class Realset;
class RealStatement;

/**
    @brief Class handling a single SQLite record (row)
*/
class RealRecord : protected STRARRAY
{
public:
	// Ctors/Dtor
	/**
		@brief Constructor from a query result
		@param pSet Record set this record belongs to
		@param pData The data of the record
		@param nColumns Number of fields in the record
	*/
	RealRecord(Realset* pSet, char** pData, int nColumns) : m_pSet(pSet) {for (int i=0;i<nColumns;i++) push_back((pData[i] != NULL) ? MakeTString(pData[i]) : _T(""));};
	/// Constructor from a statement (used while stepping through it)
	RealRecord(Realset* pSet, RealStatement* pStatement, int nColumns);
	/**
		Destructor
	*/
	virtual ~RealRecord() {};
	friend class Record;
	friend class Realset;

protected:
	// Members
	/// Pointer to the record set this record belongs to
	Realset*	m_pSet;

public:
	// Data Access
	/**
		@brief Retrieves a field value (by field order)
		@param nColumn The field to retrieve the value of
		@return The contents of the field
	*/
	std::tstring GetField(int nColumn) const {return STRARRAY::operator[](nColumn);};
	/// Retrieves a field value (by field name)
	std::tstring GetField(const std::tstring& sName) const;
};

/**
    @brief Class handling an SQLite record set (read only!)
*/
class Realset : public RefObj
{
public:
	// Ctors/Dtor
	/**
		@brief Constructor from an SQL statement
		@param pDB The database to retrieve the query results from
		@param pSQL The statement to run
	*/
	Realset(sqlite3* pDB, LPCTSTR pSQL) {m_bValid = (sqlite3_exec(pDB, MakeAnsiString(pSQL).c_str(), QueryCallback, (void*)this, NULL) == SQLITE_OK); if (!IsValid()) Clean();};
	/// Constructor of record set by stepping through the statement and retrieving all the records
	Realset(RealStatement* pStatement);
protected:
	/**
		Destructor
	*/
	~Realset() {Clean();};

protected:
	// Definions
	/// Array of SQLite records definion
	typedef std::vector<RealRecord*> RECORDARRAY;

protected:
	// Members
	/// Map of column names to ordinals
	STR2INT		m_mColumns;
	/// Array of records
	RECORDARRAY m_arRecords;
	/// Flag of recordset validity
	bool		m_bValid;

public:
	// Data Access
	/**
		@param sName Name of field to to get ordinal of
		@return The ordinal number of the field
	*/
	int			GetColumn(const std::tstring& sName) const {STR2INT::const_iterator i = m_mColumns.find(sName); if (i == m_mColumns.end()) return -1; return (*i).second;};
	/**
		@brief Retrieves a record by index
		@param nNum The index of the record
		@return A pointer to the record object, or NULL if no such record exists
	*/
	RealRecord* GetRecord(int nNum) const {if ((nNum < 0) || (nNum >= m_arRecords.size())) return NULL; return m_arRecords[nNum];};
	/**
		@brief Retrieves the number of records in the set
		@return The number of records
	*/
	int			GetRecordCount() const {return m_arRecords.size();};
	/**
		@brief Tests if this records set is valid
		@return true if the record set is valid, false otherwise
	*/
	bool		IsValid() const {return m_bValid;};
	/**
		@brief Retrieves the number of fields in the record set
		@return The number of fields
	*/
	int			GetColumnCount() const {return m_mColumns.size();};
	/**
		@brief Retrieves the name of the field with the specified ordinal
		@param nNum The ordinal number of the field
		@return The name of the requested field
	*/
	std::tstring GetColumnName(int nNum) const {for (STR2INT::const_iterator i = m_mColumns.begin(); i != m_mColumns.end(); i++) if ((*i).second == nNum) return (*i).first; return _T("");};

private:
	// Helpers
	/// Add a record to the record set (used from the callback function)
	int			AddRecord(int nColumns, char** pData, char** pColumns);
	/// Clean the object by removing all the records
	void		Clean();

	// Callbacks
	/**
		@param pThis Pointer to the calling record set object
		@param nColumns The number of columns in the query
		@param pData Array containing a record contents
		@param pColumns Array containing the names of the columns in the query
		@return 0 to continue to the next record, 1 to stop
	*/
	static int	QueryCallback(void* pThis, int nColumns, char** pData, char** pColumns) {return ((Realset*)pThis)->AddRecord(nColumns, pData, pColumns);};
};

/**
	
*/
void Realset::Clean()
{
	ASSERT(m_nRef == 0);
	for (int i=0;i<m_arRecords.size();i++) 
		delete m_arRecords[i];
}

/**
	@param nColumns Number of fields in the record
	@param pData Array containing the contents of the record
	@param pColumns Array containing the names of the query columns
	@return 0 to continue to the next record, 1 to stop
*/
int Realset::AddRecord(int nColumns, char** pData, char** pColumns)
{
	// Make sure this is a valid record:
	int i;
	for (i=0;i<nColumns;i++)
		if (pData[i] != NULL)
			break;
	if (i == nColumns)
		// Didn't find any values, just move on
		return 0;

	// Is this the first record?
	if (!m_mColumns.empty())
	{
		// No, make sure we have the same number of columns!
		if (m_mColumns.size() != nColumns)
		{
			ASSERT(FALSE);
			return 1;
		}
	}
	else
	{
		// OK, remember the names of the columns
		ASSERT(m_arRecords.empty());
		for (int i=0;i<nColumns;i++)
			m_mColumns[MakeTString(pColumns[i])] = i;
	}

	// Add a new record
	m_arRecords.push_back(new RealRecord(this, pData, nColumns));
	return 0;
}



/**
	@param nNum The index of the record to retrieve
	@return A wrapper object of the requested record
*/
Record Recordset::GetRecord(int nNum) const 
{
	// Is this record set valid?
	if (!IsValid()) 
		// No, just return an empty record
		return Record(); 
	// Retrieve the record
	return Record(m_pSet->GetRecord(nNum));
}

/**
	@return true if the actual record set is valid, false if not
*/
bool Recordset::IsValid() const 
{
	return (m_pSet != NULL) && m_pSet->IsValid();
}

/**
	@return The number of records in the wrapped record set
*/
int Recordset::GetRecordCount() const 
{
	if (!IsValid())
		// No records in an invalid set
		return 0;
	return m_pSet->GetRecordCount();
}

/**
	
*/
void Recordset::AddRef() const
{
	if (m_pSet != NULL) 
		m_pSet->AddRef();
}

/**
	
*/
void Recordset::DecRef() const
{
	if (m_pSet != NULL)
		m_pSet->DecRef();
}

/**
	@return The number of fields in the wrapped record set
*/
int	Recordset::GetColumnCount() const
{
	if (!IsValid())
		// Not valid - no fields
		return 0;
	return m_pSet->GetColumnCount();
}

/**
	@param nNum The ordinal of the field
	@return The name of the requested field
*/
std::tstring Recordset::GetColumnName(int nNum) const
{
	if (!IsValid())
		// Not valid - no name
		return _T("");
	return m_pSet->GetColumnName(nNum);
}



/**
	@param sName The name of the field to retrieve value of
	@return The contents of the requested field
*/
std::tstring RealRecord::GetField(const std::tstring& sName) const
{
	// Get the ordinal of the column
	int nColumn = m_pSet->GetColumn(sName);
	if (nColumn == -1)
		// Not found, empty value!
		return _T("");
	return STRARRAY::operator[](nColumn);
}





/**
	@param nColumn The ordinal number of the field
	@return The contents of the requested field
*/
std::tstring Record::operator[](int nColumn) const 
{
	return m_pRecord->GetField(nColumn);
}

/**
	@param sName The name of the field
	@return The contents of the requested field
*/
std::tstring Record::operator[](const std::tstring& sName) const 
{
	return m_pRecord->GetField(sName);
}

/**
	@param nColumn The ordinal number of the field
	@return The contents of the requested field
*/
std::tstring Record::GetField(int nColumn) const 
{
	return m_pRecord->GetField(nColumn);
}

/**
	@param sName The name of the field
	@return The contents of the requested field
*/
std::tstring Record::GetField(const std::tstring& sName) const 
{
	return m_pRecord->GetField(sName);
}

/**
	
*/
void Record::AddRef() const
{
	if (m_pRecord != NULL) 
		// We keep the set (individual records do not have references)
		m_pRecord->m_pSet->AddRef();
}

/**
	
*/
void Record::DecRef() const
{
	if (m_pRecord != NULL) 
		// We keep the set (individual records do not have references)
		m_pRecord->m_pSet->DecRef();
}


/**
    @brief Class handling SQLite database operations
*/
class RealDB : public RefObj
{
public:
	// Ctor/Dtor
	/**
		@brief Database object constructor
		@param pFilename Path of database file to open, or NULL not to open
	*/
	RealDB(LPCTSTR pFilename) : m_pDB(NULL) {if (pFilename != NULL) Open(pFilename);};
protected:
	/**
		Destructor
	*/
	virtual ~RealDB() {Close();};

protected:
	// Members
	/// Pointer to the SQLite database structure
	sqlite3*	m_pDB;

public:
	// Data Access
	/**
		@brief Tests if this object has an open database
		@return true if the object has an open database, false if not
	*/
	bool		IsOpen() const {return (m_pDB != NULL);};
	/**
		@return Retrieves the internal SQLite database object
	*/
	sqlite3*	GetSQLiteDB() const {return m_pDB;};

public:
	// Methods
	/**
		@brief Open a database file
		@param pFilename Path to file to open (or create)
		@return true if the database file was opened successfully, false if failed
	*/
	bool		Open(LPCTSTR pFilename) {if (IsOpen()) if (!Close()) return false; if (sqlite3_open(MakeAnsiString(pFilename).c_str(), &m_pDB) != SQLITE_OK) return false; return true;};
	/**
		@brief Close the currently open database file
		@return true if closed successfully, false if failed
	*/
	bool		Close() {if (!IsOpen()) return true; if (sqlite3_close(m_pDB) != SQLITE_OK) return false; m_pDB = NULL; return true;};
	/**
		@brief Runs an SQL statement on the open database
		@param pSQL The SQL statement to run
		@return true if run successfully, false if failed
	*/
	bool		Execute(LPCTSTR pSQL) {return Execute(MakeTString(pSQL));};
	/**
		@brief Runs an SQL statement on the open database
		@param sSQL The SQL statement to run
		@return true if run successfully, false if failed
	*/
	bool		Execute(const std::tstring& sSQL) {if (!IsOpen()) return false; return (sqlite3_exec(m_pDB, MakeAnsiString(sSQL).c_str(), NULL, NULL, NULL) == SQLITE_OK);};

	/**
		@brief Retrieves the ID of the last row inserted into the database
		@return ID of the last row inserted into the database
	*/
	sqlite_int64 GetLastInsertRowid() const {if (!IsOpen()) return 0; return sqlite3_last_insert_rowid(m_pDB);};

	/**
		@brief Retrieves the description of the last error that occured while working with the database
		@return The description of the last error that occured while working with the database
	*/
	std::tstring GetLastError() {if (m_pDB == NULL) return _T(""); return MakeTString(sqlite3_errmsg(m_pDB));};
};

/**
    @brief Class handling SQLite transactions
*/
class RealTransaction : public RefObj
{
public:
	// Ctor/Dtor
	/**
		@brief Constructor from a database and type
		@param db Database wrapper of the database this transaction object belongs to
		@param eType Type of the transaction
		@param sName Name of the transaction (optional, and currenntly not used in SQLite)
	*/
	RealTransaction(const DB& db, Transaction::TransactionType eType, const std::tstring& sName = _T("")) : m_db(db), m_sName(sName), m_bActive(false) {m_bActive = StartTransaction(eType);};
protected:
	/**
		@brief Destructor; closes the transaction if still active
	*/
	virtual ~RealTransaction() {if (m_bActive) Close();};

protected:
	// Members
	/// Database this transaction belongs to
	DB			m_db;
	/// Name of transaction
	std::tstring m_sName;
	/// true if the transaction is 'live', i.e., has uncommited data
	bool		m_bActive;
	/// Description of last error that occured while executing statements
	std::tstring m_sLastError;

public:
	// Data Access
	/**
		@brief Tests if the transaction is live (i.e., started)
		@return true if the transaction is live, false otherwise
	*/
	bool		IsValid() const {return m_bActive;};
	/**
		@return The description of the last error that occured while performing the transaction
	*/
	const std::tstring&	GetLastError() const {return m_sLastError;};

public:
	// Methods
	/// Commit the transaction and end it
	bool		Close();
	/**
		@brief Commit the transaction, i.e., make the changes up to now permanent
		@return true if commited successfully, false if failed
	*/
	bool		Commit() {if (!IsValid()) return false; bool bRet = m_db.Execute(_T("COMMIT TRANSACTION ") + m_sName); if (!bRet) m_sLastError = m_db.GetLastError(); return bRet;};
	/**
		@brief Roll back the changes the transaction performed in the database from the last commit
		@return true if rolled back successfully, false if failed
	*/
	bool		Rollback() {if (!IsValid()) return false; if (!m_db.Execute(_T("ROLLBACK TRANSACTION ") + m_sName)) {m_sLastError = m_db.GetLastError(); return false;} m_bActive = false; return true;};

	/**
		@brief Runs an SQL action on the database (in the transaction)
		@param pSQL SQL statement to perform
		@return true if the SQL statement was performed successfully, false if failed
	*/
	bool		Execute(LPCTSTR pSQL) {if (!IsValid()) return false; bool bRet = m_db.Execute(pSQL); if (!bRet) m_sLastError = m_db.GetLastError(); return bRet;};
	/**
		@brief Runs an SQL action on the database (in the transaction)
		@param sSQL SQL statement to perform
		@return true if the SQL statement was performed successfully, false if failed
	*/
	bool		Execute(const std::tstring& sSQL) {return m_db.Execute(sSQL.c_str());};
	/**
		@brief Runs a prepared SQL statement on the database (in the transaction)
		@param statement Statement to perform
		@return true if the statement was performed successfully, false if failed
	*/
	bool		Execute(Statement& statement) {if (!IsValid() || (statement.GetDB() != m_db)) return false; return statement.Execute();};
	/**
		@brief Performs an SQL query on the database (in the transaction)
		@param pSQL SQL query to perform
		@return The results of the query (empty if there was an error)
	*/
	Recordset	Query(LPCTSTR pSQL) {if (!IsValid()) return Recordset(); Recordset ret = m_db.Query(pSQL); if (!ret.IsValid()) m_sLastError = m_db.GetLastError(); return ret;};
	/**
		@brief Performs an SQL query on the database (in the transaction)
		@param sSQL SQL query to perform
		@return The results of the query (empty if there was an error)
	*/
	Recordset	Query(const std::tstring& sSQL) {if (!IsValid()) return Recordset(); return m_db.Query(sSQL.c_str());};
	/**
		@brief Performs a prepared SQL query on the database (in the transaction)
		@param statement The query to perform
		@return The results of the query (empty if there was an error)
	*/
	Recordset	Query(Statement& statement) {if (!IsValid() || (statement.GetDB() != m_db)) return Recordset(); return statement.Query();};

protected:
	// Helpers
	/// Starts the transaction
	bool		StartTransaction(Transaction::TransactionType eType);
};

/// SQLite keywords for the transaction types
LPCTSTR TRANSACTION_TYPE[] =
{
	_T("DEFERRED"),
	_T("IMMEDIATE"),
	_T("EXCLUSIVE"),
	NULL
};

/**
	@param eType The type of the transaction to start
	@return true if the transaction was started successfully, false if failed to start
*/
bool RealTransaction::StartTransaction(Transaction::TransactionType eType)
{
	// Make sure we have valid parameters
	ASSERT((eType >= Transaction::TTDeferred) && (eType <= Transaction::TTExclusive));
	// And that we haven't started the transaction already!
	if (IsValid())
		return false;
	// Create a transaction start query
	std::tstring sSQL = _T("BEGIN ") + std::tstring(TRANSACTION_TYPE[eType]) + _T(" TRANSACTION ") + m_sName;
	// And run it
	if (m_db.Execute(sSQL))
		m_bActive = true;
	return m_bActive;
}

/**
	@return true if the transaction was closed successfully, false if failed
*/
bool RealTransaction::Close()
{
	// Do we have a valid transaction?
	if (!IsValid())
		// Nope
		return false;
	// Close it
	if (!m_db.Execute(_T("END TRANSACTION ") + m_sName))
		// Failed?!
		return false;

	// OK, finished here
	m_bActive = false;
	return true;
}




/**
    @brief Class handling the abstruction of SQLite prepared SQL statements
*/
class RealStatement : public RefObj
{
public:
	// Ctors/Dtor
#ifdef _DEBUG
	/**
		@brief Constructor from a database and SQL statement
		@param db The database to prepare the statement for
		@param pStatement The SQL statement to perpare
	*/
	RealStatement(const DB& db, LPCTSTR pStatement) : m_sOriginalSQL(pStatement), m_db(db), m_pStatement(NULL), m_bExecuted(false), m_nLastInsertRowid(0) {const char* pTail; if (sqlite3_prepare(m_db.m_pDB->GetSQLiteDB(), MakeAnsiString(m_sOriginalSQL).c_str(), -1, &m_pStatement, &pTail) != SQLITE_OK) {m_sLastError = m_db.m_pDB->GetLastError(); if (m_pStatement != NULL) {sqlite3_finalize(m_pStatement); m_pStatement = NULL;}}};
#else
	/**
		@brief Constructor from a database and SQL statement
		@param db The database to prepare the statement for
		@param pStatement The SQL statement to perpare
	*/
	RealStatement(const DB& db, LPCTSTR pStatement) : m_db(db), m_pStatement(NULL), m_bExecuted(false), m_nLastInsertRowid(0) {const char* pTail; if (sqlite3_prepare(m_db.m_pDB->GetSQLiteDB(), MakeAnsiString(pStatement).c_str(), -1, &m_pStatement, &pTail) != SQLITE_OK) {m_sLastError = m_db.m_pDB->GetLastError(); if (m_pStatement != NULL) {sqlite3_finalize(m_pStatement); m_pStatement = NULL;}}};
#endif
	/**
		@brief Destructor
	*/
	~RealStatement() {if (m_pStatement != NULL) sqlite3_finalize(m_pStatement);};
	friend class Realset;
	friend class RealRecord;

protected:
	// Members
	/// Wrapper object of the database this statement belongs to
	DB			m_db;
	/// Description of the last error that occured while creating or running the statement
	std::tstring m_sLastError;
	/// Pointer to the internal SQLite statement structure
	sqlite3_stmt* m_pStatement;
	/// true while executing the statement, false after the statement is finished
	bool		m_bExecuted;
	/// The ID of the last row inserted by the statement
	sqlite_int64 m_nLastInsertRowid;
#ifdef _DEBUG
	/// The original SQL statement (for debugging purposes)
	std::tstring m_sOriginalSQL;
#endif

public:
	// Data Access
	/**
		@brief Retrieves the database this statement is perpared for
		@return A database wrapper object for the requested database
	*/
	const DB	GetDB() const {return m_db;};
	/**
		@brief Tests if the statement is valid (prepared successfully)
		@return true if the statement is valid, false otherwise
	*/
	bool		IsValid() const {return (m_pStatement != NULL);};
	/**
		@brief Retrieves the description of the last error occured while preparing or performing the statement
		@return The description of the last error
	*/
	const std::tstring&	GetLastError() const {return m_sLastError;};
	/**
		@brief Tests if a parameter of the requested name exists in the statement
		@param pParam The name of the parameter to check
		@return true if the statement contains this named parameter, false if not
	*/
	bool		HasParameter(LPCTSTR pParam) const {if (!IsValid()) return false; return sqlite3_bind_parameter_index(m_pStatement, MakeAnsiString(pParam).c_str()) != 0;};

public:
	// Methods
	/**
		@brief Sets a parameter's value in the statement (by name)
		@param pParam The parameter's name
		@param pValue The parameter's new value
		@return true if set successfully, false if failed
	*/
	bool		BindParameter(LPCTSTR pParam, LPCTSTR pValue) {int nParam = ParameterIndex(pParam); if (nParam == 0) return false; return BindParameter(nParam, pValue);};
	/**
		@brief Sets a parameter's value in the statement (by name)
		@param pParam The parameter's name
		@param nValue The parameter's new value
		@return true if set successfully, false if failed
	*/
	bool		BindParameter(LPCTSTR pParam, int nValue) {int nParam = ParameterIndex(pParam); if (nParam == 0) return false; return BindParameter(nParam, nValue);};
	/**
		@brief Sets a parameter's value in the statement (by index)
		@param nParam The parameter's index
		@param pValue The parameter's new value
		@return true if set successfully, false if failed
	*/
	bool		BindParameter(int nParam, LPCTSTR pValue) {if (!IsValid()) return false; if (m_bExecuted) if (!Reset()) return false; std::string sValue = MakeAnsiString(pValue); if (sqlite3_bind_text(m_pStatement, nParam, sValue.c_str(), sValue.size(), SQLITE_TRANSIENT) == SQLITE_OK) return true; m_sLastError = m_db.GetLastError(); return false;};
	/**
		@brief Sets a parameter's value in the statement (by index)
		@param nParam The parameter's index
		@param nValue The parameter's new value
		@return true if set successfully, false if failed
	*/
	bool		BindParameter(int nParam, int nValue) {if (!IsValid()) return false; if (m_bExecuted) if (!Reset()) return false; if (sqlite3_bind_int(m_pStatement, nParam, nValue) == SQLITE_OK) return true; m_sLastError = m_db.GetLastError(); return false;};

	/**
		@brief Executes the statement
		@return true if the statement was performed successfully, false if failed
	*/
	bool		Execute() {if (!IsValid()) return false; m_bExecuted = true; if (sqlite3_step(m_pStatement) == SQLITE_DONE) {m_nLastInsertRowid = m_db.GetLastInsertRowid(); return true;} m_sLastError = m_db.GetLastError(); m_nLastInsertRowid = 0; return false;};
	/// Runs the statement and returns the result set
	Recordset	Query();
#ifdef _DEBUG
//	void		Dump(CDumpContext& dc) const;
#endif
	/**
		@brief Retrieves the ID of the last row inserted by the statement
		@return The ID of the last row inserted by the statement
	*/
	sqlite_int64 GetLastInsertRowid() const {if (!IsValid()) return 0; return m_nLastInsertRowid;};

protected:
	// Helpers
	/**
		@brief Resets the statement (allowing it to run again)
		@return true if the statement was reset successfully, false if failed for some reason
	*/
	bool		Reset() {m_bExecuted = false; if (sqlite3_reset(m_pStatement) == SQLITE_OK) return true; m_sLastError = m_db.GetLastError(); return false;};
	/**
		@brief Retrieves the index of a named parameter
		@param pParam The parameter to find
		@return The 1-based index of the named parameter (0 if not found)
	*/
	int			ParameterIndex(LPCTSTR pParam) {ASSERT(IsValid()); return sqlite3_bind_parameter_index(m_pStatement, MakeAnsiString(pParam).c_str());};
};

/**
	@return The record set that is the result of the query statement
*/
Recordset RealStatement::Query()
{
	// The statement should be prepared:
	if (!IsValid())
		// Not!
		return Recordset();
	// Queries will not be an INSERT, so:
	m_nLastInsertRowid = 0;
	// OK, run the query
	return Recordset(new Realset(this));
}

/**
	@param pStatement Pointer to the query statement object to create the result set from
*/
Realset::Realset(RealStatement* pStatement)
{
	// Initialize the result set
	int nRes;
	m_bValid = false;
	
	// This is the main loop for retrieving the results: step through one row at a time
	while ((nRes = sqlite3_step(pStatement->m_pStatement)) == SQLITE_ROW)
	{
		// Got a row, how many columns?
		int nColumns = sqlite3_data_count(pStatement->m_pStatement);
		if (!m_mColumns.empty())
		{
			// We already have columns, make sure there's no problem with the count
			if (m_mColumns.size() != nColumns)
			{
				// What?
				ASSERT(FALSE);
				nRes = SQLITE_ERROR;
				break;
			}
		}
		else
		{
			// This should be the first round, so save the column names
			ASSERT(m_arRecords.empty());
			for (int i=0;i<nColumns;i++)
				m_mColumns[MakeTString(sqlite3_column_name(pStatement->m_pStatement, i))] = i;
		}

		// Add a new record object from the data
		m_arRecords.push_back(new RealRecord(this, pStatement, nColumns));
	}

	// Is everything all right?
	if (nRes == SQLITE_DONE)
		// Yeah
		m_bValid = true;
	else
	{
		// No, get the last error
		pStatement->m_sLastError = pStatement->m_db.GetLastError();
		Clean();
	}

	// Make sure we can use the statement again
	pStatement->Reset();
}

/**
	@param pSet The result set this record belongs to
	@param pStatement The statement that this record was created from
	@param nColumns The number of columns in the result
*/
RealRecord::RealRecord(Realset* pSet, RealStatement* pStatement, int nColumns) : m_pSet(pSet)
{
	// Go over the columns
	for (int i=0;i<nColumns;i++)
	{
		// Get the valud for each
		const char* pValue = (const char*)sqlite3_column_text(pStatement->m_pStatement, i);
		if (pValue == NULL)
			// Empty...
			push_back(_T(""));
		else
			// Value...
			push_back(MakeTString(pValue));
	}
}

#ifdef _DEBUG
/*
void RealStatement::Dump(CDumpContext& dc) const
{
	if (m_pStatement == NULL)
	{
		dc << "Error: " << m_sLastError.c_str() << "\n";
		return;
	}
	int nParams = sqlite3_bind_parameter_count(m_pStatement);
	dc << "Dumping statement with " << nParams << " parameters:\n";
	for (int i=1;i<=nParams;i++)
		dc << "\t" << i << ". " << sqlite3_bind_parameter_name(m_pStatement, i) << "\n";
}
*/
#endif




/**
	@return true if the statement is valid, false if not
*/
bool Statement::IsValid() const
{
	if (m_pReal == NULL)
		return false;
	return m_pReal->IsValid();
}

/**
	@return true if the statement was executed successfully, false if failed
*/
bool Statement::Execute()
{
	if (!IsValid())
		return false;
	return m_pReal->Execute();
}

/**
	@return The result of the query (empty record set if failed or invalid)
*/
Recordset Statement::Query()
{
	if (!IsValid())
		return Recordset();
	return m_pReal->Query();
}

/**
	@param pParam The parameter to find
	@return true if there is such a named parameter in the prepared statement, false if not
*/
bool Statement::HasParameter(LPCTSTR pParam) const
{
	if (!IsValid())
		return false;
	return m_pReal->HasParameter(pParam);
}

/**
	@param pParam The parameter to set the value of
	@param pValue The value to set
	@return true if value was set successfully, false if failed
*/
bool Statement::BindParameter(LPCTSTR pParam, LPCTSTR pValue)
{
	if (!IsValid())
		return false;
	return m_pReal->BindParameter(pParam, pValue);
}

/**
	@param pParam The parameter to set the value of
	@param nValue The value to set
	@return true if value was set successfully, false if failed
*/
bool Statement::BindParameter(LPCTSTR pParam, int nValue)
{
	if (!IsValid())
		return false;
	return m_pReal->BindParameter(pParam, nValue);
}

/**
	@param nParam Index of the parameter to set the value of
	@param pValue The value to set
	@return true if value was set successfully, false if failed
*/
bool Statement::BindParameter(int nParam, LPCTSTR pValue)
{
	if (!IsValid())
		return false;
	return m_pReal->BindParameter(nParam, pValue);
}

/**
	@param nParam Index of the parameter to set the value of
	@param nValue The value to set
	@return true if value was set successfully, false if failed
*/
bool Statement::BindParameter(int nParam, int nValue)
{
	if (!IsValid())
		return false;
	return m_pReal->BindParameter(nParam, nValue);
}

/**
	@return The description of the last error that occured while performing the statement
*/
std::tstring Statement::GetLastError()
{
	if (m_pReal == NULL)
		return _T("");
	return m_pReal->GetLastError();
}

/**
	
*/
void Statement::AddRef() const
{
	if (m_pReal != NULL)
		m_pReal->AddRef();
}

/**
	
*/
void Statement::DecRef() const
{
	if (m_pReal != NULL)
		m_pReal->DecRef();
}

#ifdef _DEBUG
/*
void Statement::Dump(CDumpContext& dc) const
{
	if (m_pReal == NULL)
		dc << "Not attached to a statement\n";
	else
		m_pReal->Dump(dc);
}
*/
#endif

/**
	@return The SQLite internal ID of the last row inserted by the statement (0 if none)
*/
sqlite_int64 Statement::GetLastInsertRowid() const
{
	if (m_pReal == NULL)
		return 0;
	return m_pReal->GetLastInsertRowid();
}

/**
	@return The database wrapper object for the database this statement was perpared for
*/
DB Statement::GetDB() const
{
	if (m_pReal == NULL)
		return DB();
	return m_pReal->GetDB();
}




/**
	@param pFilename File to open
*/
DB::DB(LPCTSTR pFilename /* = NULL */) 
{
	m_pDB = new RealDB(pFilename); 
	AddRef();
}

/**
	@return true if the database was opened successfully
*/
bool DB::IsOpen() const 
{
	return m_pDB->IsOpen();
}

/**
	@param pFilename File to open
	@return true if the database was opened successfully
*/
bool DB::Open(LPCTSTR pFilename)
{
	return m_pDB->Open(pFilename);
}

/**
	@return true if the database was closed successfully
*/
bool DB::Close() 
{
	return m_pDB->Close();
}

/**
	@param pSQL SQL statement to perform
	@return true if the statement was performed successfully, false if failed
*/
bool DB::Execute(LPCTSTR pSQL)
{
	return m_pDB->Execute(pSQL);
}

/**
	@param pSQL SQL query to perform
	@return The result of the query (empty result set if failed)
*/
Recordset DB::Query(LPCTSTR pSQL) 
{
	if (!IsOpen()) 
		return Recordset(); 
	return Recordset(new Realset(m_pDB->GetSQLiteDB(), pSQL));
}

/**
	@param eType Type of transaction to start
	@param sName The name of the transaction (can be NULL)
	@return A transaction object wrapping the initialized transaction
*/
Transaction	DB::BeginTransaction(Transaction::TransactionType eType, const std::tstring& sName /* = _T("") */) 
{
	return Transaction(new RealTransaction(m_pDB, eType, sName));
}

/**
	@param pStatement The SQL statement to prepare for the database
	@return A prepared statement wrapper object
*/
Statement DB::CreateStatement(LPCTSTR pStatement)
{
	return Statement(new RealStatement(m_pDB, pStatement));
}

/**
	@return The description of the last error that occured while performing operations on the database
*/
std::tstring DB::GetLastError() 
{
	return m_pDB->GetLastError();
}

/**
	
*/
void DB::AddRef() const
{
	if (m_pDB != NULL) 
		m_pDB->AddRef();
}

/**
	
*/
void DB::DecRef() const
{
	if (m_pDB != NULL) 
		m_pDB->DecRef();
}

/**
	@param pString The definition string
	@return A prepared, SQLite-safe statement string
*/
std::string DB::printf(const char* pString, ...)
{
	// Retrieve the variable list
	va_list vlist;
	va_start(vlist, pString);
	// Get the result from the SQLite print function
	char* pResult = sqlite3_vmprintf(pString, vlist);
	va_end(vlist);

	// Now create a copy
	std::string sRet;
	if (pResult != NULL)
	{
		sRet = pResult;
		sqlite3_free(pResult);
	}
	return sRet;
}

/**
	@return The SQLite internal ID of the last row inserted into the database (0 if none)
*/
sqlite_int64 DB::GetLastInsertRowid() const
{
	if (m_pDB == NULL)
		return 0;
	return m_pDB->GetLastInsertRowid();
}




/**
	@return true if the transaction was successfuly closed, false otherwise
*/
bool Transaction::Close() 
{
	return m_pReal->Close();
}

/**
	@return true if the transaction was commited successfully, false otherwise
*/
bool Transaction::Commit() 
{
	return m_pReal->Commit();
}

/**
	@return true if the transaction was rolled back successfully, false otherwise
*/
bool Transaction::Rollback() 
{
	return m_pReal->Rollback();
}

/**
	@param pSQL SQL statement to perform
	@return true if the statement was performed successfully, false if failed
*/
bool Transaction::Execute(LPCTSTR pSQL) 
{
	return m_pReal->Execute(pSQL);
}

/**
	@param statment SQL statement wrapper object to perform
	@return true if the statement was performed successfully, false if failed
*/
bool Transaction::Execute(Statement& statment) 
{
	return m_pReal->Execute(statment);
}

/**
	@param pSQL SQL query to perform
	@return A query result set wrapper objectc
*/
Recordset Transaction::Query(LPCTSTR pSQL) 
{
	return m_pReal->Query(pSQL);
}

/**
	@param statement SQL query statement to perform
	@return A query result set wrapper objectc
*/
Recordset Transaction::Query(Statement& statement) 
{
	return m_pReal->Query(statement);
}

/**
	
*/
void Transaction::AddRef() const 
{
	m_pReal->AddRef();
}

/**
	
*/
void Transaction::DecRef() const 
{
	m_pReal->DecRef();
}

/**
	@return The description of the last error that occured while performing the transaction
*/
const std::tstring& Transaction::GetLastError() 
{
	return m_pReal->GetLastError();
}




}
