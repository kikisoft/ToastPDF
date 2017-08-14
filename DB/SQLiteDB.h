/**
	@file
	@brief Header file for SQLite wrapper objects
*/

/*
 * Copyright (C) 2007 Guy Hachlili <hguy@cogniview.com>
 * 
 * Used with permission in CC PDF Converter
 *
 */


#ifndef _SQLITEDB_H_
#define _SQLITEDB_H_

#include "sqlite3.h"
#include "CCTChar.h"
#include <map>
#include <vector>
#include <set>
#include <debug.h>

/// Definition of array of strings
typedef std::vector<std::tstring>	STRARRAY;
/// Map of strings to integers
typedef std::map<std::tstring, int>	STR2INT;

namespace SQLite
{

/**
	@brief Base class for self-deleting objects (when no more references are kept)
*/
class RefObj
{
public:
	/**
		@brief Constructor
	*/
	RefObj() : m_nRef(0) {};
	/**
		@brief Destructor
	*/
	virtual ~RefObj() {ASSERT(m_nRef == 0);};

protected:
	/// Count of references
	int		m_nRef;

public:
	/**
		@brief Adds a reference to the object
	*/
	void	AddRef() {m_nRef++;};
	/**
		@brief Deletes a reference to the object, and the object itself if the reference count is zero
	*/
	void	DecRef() {m_nRef--; ASSERT(m_nRef >= 0); if (m_nRef == 0) delete this;};
};

class RealRecord;

/**
    @brief Wrapper class for referenced records
*/
class Record
{
public:
	// Ctors/Dtor
	/**
		@brief Default constructor
	*/
	Record() : m_pRecord(NULL) {};
	/**
		@brief Copy constructor
		@param record The record wrapper to copy
	*/
	Record(const Record& record) : m_pRecord(record.m_pRecord) {AddRef();};
	/**
		@brief Destructor; will delete the record if this is the last reference
	*/
	virtual ~Record() {DecRef();};
protected:
	/**
		@brief Constructor from an actual record object
		@param pRecord 
	*/
	Record(RealRecord* pRecord) : m_pRecord(pRecord) {AddRef();};

	// Definitions
	friend class Recordset;

protected:
	/// Pointer to the actual record object
	RealRecord*	m_pRecord;

public:
	// Methods
	/**
		@brief Tests if this object points to an actual record
		@return true if there's a record, false if not
	*/
	bool		IsValid() const {return (m_pRecord != NULL);};

	/// Retrieves the value of a field of the record (by order)
	std::tstring operator[](int nColumn) const;
	/// Retrieves the value of a field of the record (by name)
	std::tstring operator[](const std::tstring& sName) const;
	/// Retrieves the value of a field of the record (by order)
	std::tstring GetField(int nColumn) const;
	/// Retrieves the value of a field of the record (by name)
	std::tstring GetField(const std::tstring& sName) const;
	/**
		@brief Retrieves the value of a numeric field of the record (by order)
		@param nColumn The number of the field
		@return The numeric contents of the field
	*/
	inline int	GetNumField(int nColumn) const {return _ttoi(GetField(nColumn).c_str());};
	/**
		@brief Retrieves the value of a numeric field of the record (by name)
		@param sName The name of the field
		@return The numeric contents of the field
	*/
	inline int	GetNumField(const std::tstring& sName) const {return _ttoi(GetField(sName).c_str());};

	/**
		@brief Assignment operator
		@param other The record to assign to this one
		@return A reference to this record
	*/
	const Record& operator=(const Record& other) {if (m_pRecord == other.m_pRecord) return *this; DecRef(); m_pRecord = other.m_pRecord; AddRef(); return *this;};

protected:
	// Helpers
	/// Adds a reference to the actual record object
	void		AddRef() const;
	/// Removes a reference to the actual record object (can delete the object)
	void		DecRef() const;
};

class Realset;
class RealTransaction;

/**
    @brief Wrapper class for referenced record sets
*/
class Recordset
{
public:
	/**
		@brief Default constructor
	*/
	Recordset() : m_pSet(NULL) {};
	/**
		@brief Copy constructor
		@param set The record set wrapper to copy
	*/
	Recordset(const Recordset& set) : m_pSet(set.m_pSet) {AddRef();};
	/**
		@brief Destructor
	*/
	virtual ~Recordset() {DecRef();};
protected:
	/**
		@brief Constructor from an actual record set object
		@param pSet Pointer to the actual record set
	*/
	Recordset(Realset* pSet) : m_pSet(pSet) {AddRef();};

protected:
	// Definitions
	friend class RealTransaction;
	friend class RealStatement;
	friend class DB;

protected:
	// Members
	/// Pointer to the actual record set object
	Realset*	m_pSet;

public:
	// Data Access
	/// Tests if this record set wrapper actually points to a record set
	bool		IsValid() const;

	/// Retreives a record (by order)
	Record		GetRecord(int nNum) const;
	/// Retrieves the number of records in the set
	int			GetRecordCount() const;
	/// Retrieves the number of fields in each record
	int			GetColumnCount() const;
	/// Retrieves the name of a field in the set
	std::tstring GetColumnName(int nNum) const;

	/**
		@brief Disconnects the wrapper from the actual record set
	*/
	void		clear() {DecRef(); m_pSet = NULL;};

public:
	// Operator overloading
	/**
		@brief Assignment operator
		@param other The record set wrapper object to copy the data of
		@return A reference to this object
	*/
	virtual const Recordset& operator=(const Recordset& other) {SetRealset(other.m_pSet); return *this;};

protected:
	// Helpers
	/// Adds a reference to the actual record set object
	void		AddRef() const;
	/// Deletes a reference to the actual record set object (can delete the object)
	void		DecRef() const;

	/**
		@brief Sets the wrapped actual record set object
		@param pSet The record set object to wrap
	*/
	void		SetRealset(Realset* pSet) {if (m_pSet == pSet) return; DecRef(); m_pSet = pSet; AddRef();};
};

class RealStatement;

/**
    @brief Wrapper class for actual SQLite statement objects
*/
class Statement
{
public:
	// Ctors/Dtor
	/**
		@brief Default constructor
	*/
	Statement() : m_pReal(NULL) {};
	/**
		@brief Copy constructor
		@param statement The statement wrapper object to copy
	*/
	Statement(const Statement& statement) : m_pReal(statement.m_pReal) {AddRef();};
	/**
		@brief Destructor
	*/
	virtual ~Statement() {DecRef();};
protected:
	/**
		@brief Constructor from an actual statement object
		@param pReal The actual statement object to wrap
	*/
	Statement(RealStatement* pReal) : m_pReal(pReal) {AddRef();};

protected:
	// Definitions
	friend class DB;

protected:
	/// Pointer to the wrapped statement object
	RealStatement* m_pReal;

public:
	/// Retrieve the DB wrapper object that this statement belongs to
	DB				GetDB() const;
	/// Tests if this statement object is valid (points to an actual statement object)
	bool			IsValid() const;

	/// Runs an execution (non-SELECT) statement
	bool			Execute();
	/// Runs a SELECT statement
	Recordset		Query();
	/// Checks if the statement has a parameter with this name
	bool			HasParameter(const TCHAR* pParam) const;
	/**
		@brief Checks if the statement has a parameter with this name
		@param sParam The name to test
		@return true if the query has this parameter, false otherwise
	*/
	inline bool		HasParameter(const std::tstring& sParam) const {return HasParameter(sParam.c_str());};

	/**
		@brief Disconnects the statement wrapper from the real statment object
	*/
	void			clear() {DecRef(); m_pReal = NULL;};

	// Bind text paramenter to named parameter
	/// Sets the value of a parameter in the statement
	bool			BindParameter(const TCHAR* pParam, const TCHAR* pValue);
	/**
		@brief Sets the value of a parameter in the statement
		@param sParam The parameter to set
		@param sValue The value to set it to
		@return true if set successfully, false if failed
	*/
	inline bool		BindParameter(const std::tstring& sParam, const std::tstring& sValue) {return BindParameter(sParam.c_str(), sValue.c_str());};
	/**
		@brief Sets the value of a parameter in the statement
		@param sParam The parameter to set
		@param pValue The value to set it to
		@return true if set successfully, false if failed
	*/
	inline bool		BindParameter(const std::tstring& sParam, const TCHAR* pValue) {return BindParameter(sParam.c_str(), pValue);};
	/**
		@brief Sets the value of a parameter in the statement
		@param pParam The parameter to set
		@param sValue The value to set it to
		@return true if set successfully, false if failed
	*/
	inline bool		BindParameter(const TCHAR* pParam, const std::tstring& sValue) {return BindParameter(pParam, sValue.c_str());};
	
	// Bind numeric parameter to named parameter
	/// Set the value of a paramenter in the statement
	bool			BindParameter(const TCHAR* pParam, int nValue);
	/**
		@brief Sets the value of a parameter in the statement
		@param sParam The parameter to set
		@param nValue The value to set it to
		@return true if set successfully, false if failed
	*/
	inline bool		BindParameter(const std::tstring& sParam, int nValue) {return BindParameter(sParam.c_str(), nValue);};

	// Bind text parameter to indexed parameter
	/// Sets the value of a parameter in the statement (by order)
	bool			BindParameter(int nParam, const TCHAR* pValue);
	/**
		@brief Sets the value of a parameter in the statement (by order)
		@param nParam The parameter to set (by order)
		@param sValue The value to set it to
		@return true if set successfully, false if failed
	*/
	inline bool		BindParameter(int nParam, const std::tstring& sValue) {return BindParameter(nParam, sValue.c_str());};
	// Bind numeric parameter to indexed parameter
	/// Sets the value of a parameter in the statement (by order)
	bool			BindParameter(int nParam, int nValue);
#ifdef _DEBUG
//	void			Dump(CDumpContext& dc) const;
	/**
		@brief Tests if the statement wrapper is valid
		@return true if there's an actual statement wrapped, false otherwise
	*/
	bool			IsEmptyStatement() const {return (m_pReal == NULL);};
#endif

	/// Retrieve the ID of the last row inserted by the wrapped statement
	sqlite_int64	GetLastInsertRowid() const;

	/// Retrieve the description of the last error that occured while running the statement
	std::tstring	GetLastError();

public:
	// Operator overloading
	/**
		@brief Assignment operator
		@param other The statement wrapper object to copy
		@return A reference to this object
	*/
	virtual const Statement& operator=(const Statement& other) {if (m_pReal == other.m_pReal) return *this; DecRef(); m_pReal = other.m_pReal; AddRef(); return *this;};

protected:
	// Helpers
	/// Adds a reference to the actual statement object
	void		AddRef() const;
	/// Deletes a reference to the actual statement object (can delete the object)
	void		DecRef() const;
};

/**
    @brief Wrapper class for actual transactions
*/
class Transaction
{
public:
	// Ctors/Dtor
	/**
		@brief Copy constructor
		@param other The transaction wrapper object to copy from
	*/
	Transaction(const Transaction& other) : m_pReal(other.m_pReal) {AddRef();};
	/**
		@brief Destructor
	*/
	virtual ~Transaction() {DecRef();};
protected:
	/**
		@brief Constructor from an actual transaction object
		@param pReal Pointer to the actual transaction object to wrap
	*/
	Transaction(RealTransaction* pReal) : m_pReal(pReal) {AddRef();};

public:
	// Definitions
	friend class DB;
	/// Definition of types of transactions supported
	typedef enum {TTDeferred, TTImmediate, TTExclusive} TransactionType;

protected:
	/// Pointer to the actual transaction object wrapped
	RealTransaction*	m_pReal;

public:
	// Members
	/// End the transaction (commiting it)
	bool		Close();
	/// Commit the transaction
	bool		Commit();
	/// Rollback the transaction
	bool		Rollback();

	/// Execute an SQL command as a part of the transaction
	bool		Execute(LPCTSTR pSQL);
	/**
		@brief Execute an SQL command as a part of the transaction
		@param sSQL The statement to execute
		@return true if executed successfully, false if failed
	*/
	inline bool	Execute(const std::string& sSQL) {return Execute(sSQL.c_str());};
	/// Execute a statement as a part of the transaction
	bool		Execute(Statement& state);
	/// Run query as a part of the transaction
	Recordset	Query(LPCTSTR pSQL);
	/**
		@brief Run query as a part of the transaction
		@param sSQL The SQL query to perform
		@return The results of the query
	*/
	inline Recordset Query(const std::string& sSQL) {return Query(sSQL.c_str());};
	/// Run a SELECT statement as a part of the transaction
	Recordset	Query(Statement& state);

	/// Retrieves the description of the last error found while performing the transaction
	const std::tstring& GetLastError();

public:
	// Operator overloading
	/**
		@brief Assignment operator
		@param other The transaction wrapper object to copy
		@return A reference to this object
	*/
	virtual const Transaction& operator=(const Transaction& other) {if (m_pReal == other.m_pReal) return *this; DecRef(); m_pReal = other.m_pReal; AddRef(); return *this;};

protected:
	// Helpers
	/// Adds a reference to the actual transaction object
	void		AddRef() const;
	/// Deletes a reference to the actual transaction object (can delete the object)
	void		DecRef() const;
};

class RealDB;

/**
    @brief Wrapper class for the actual SQLite DB object
*/
class DB
{
public:
	// Ctors/Dtor
	/// Default constructor - open a database file if one is received
	DB(LPCTSTR pFilename = NULL);
	/**
		@brief Copy constructor
		@param db The database wrapper object to copy
	*/
	DB(const DB& db) : m_pDB(db.m_pDB) {AddRef();};
	/**
		@brief Destructor
	*/
	virtual ~DB() {DecRef();};
protected:
	/**
		@brief Constructor from an actual database object
		@param pDB Pointer to the actual database object
	*/
	DB(RealDB* pDB) : m_pDB(pDB) {AddRef();};

protected:
	// Definitions
	friend class RealStatement;

protected:
	// Members
	/// Pointer to the actual database object
	RealDB*		m_pDB;

public:
	// Data Access
	/// Tests if a database was opened and connected to this wrapper
	bool		IsOpen() const;

public:
	// Methods
	/// Open a database file
	bool		Open(LPCTSTR pFilename);
	/// Close the current database
	bool		Close();
	/// Perform an SQL statement
	bool		Execute(LPCTSTR pSQL);
	/**
		@brief Perform an SQL statement
		@param sSQL The statement to perform
		@return true if the statement was performed successfully, false if failed
	*/
	inline bool	Execute(const std::tstring& sSQL) {return Execute(sSQL.c_str());};
	/// Perform a SELECT SQL statement
	Recordset	Query(LPCTSTR pSQL);
	/**
		@brief Perform a SELECT SQL statement
		@param sSQL The SELECT SQL statement
		@return The results of the query
	*/
	inline Recordset Query(const std::tstring& sSQL) {return Query(sSQL.c_str());};
	/// Start a transaction
	Transaction	BeginTransaction(Transaction::TransactionType eType, const std::tstring& sName = _T(""));
	/// Compile an SQL statement for this database (can contain parameters)
	Statement	CreateStatement(LPCTSTR pStatement);
	/**
		@brief Compile an SQL statement for this database (can contain parameters)
		@param sStatement The statement to compile
		@return A statement wrapper object
	*/
	inline Statement CreateStatement(const std::tstring& sStatement) {return CreateStatement(sStatement.c_str());};

	/// Retrieve the last error for operations on the wrapped database
	std::tstring GetLastError();

	/// Retrieve the ID of the last row added to the wrapped database
	sqlite_int64 GetLastInsertRowid() const;

protected:
	// Helpers
	/// Adds a reference to the actual database object
	void		AddRef() const;
	/// Deletes a reference to the actual database object (can delete the object)
	void		DecRef() const;

public:
	// Operator overloading
	/**
		@brief Deferential operator
		@param db Database to compare
		@return true if the database is the same, false if it's a different one
	*/
	bool operator!=(const DB& db) const {return (m_pDB != db.m_pDB);};

	// Static methods
	/// Helper function for creating statements in an SQL-safe way
	static std::string printf(const char* pString, ...);
};


};

#endif   //#define _SQLITEDB_H_
