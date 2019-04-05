#ifndef _SQLHELP_H_
#define _SQLHELP_H_
#include <sqlite3.h>

// lazy level: master
struct SqlOpenMode { int mode; SqlOpenMode(int val
	= 0) : mode((val == 2) ? 6 : (val ? 2 : 1)) {} };
int __stdcall sql_open(sqlite3 **ppDb, 
	cch* fileName, SqlOpenMode mode = {0});

#define sql_finalize(p) asm volatile("push %0;" \
	"call sql_finalize_;" :: "m"(p));

// text retrieval at column
cstr __stdcall sql_getStr(sqlite3_stmt* pStmt, int iCol);
xarray<byte> __stdcall sql_getBlob(sqlite3_stmt* pStmt, int iCol);

#define SQLSTMT_COMMON  \
	operator sqlite3_stmt*() { return pStmt; } \
	int getInt(int iCol) { return sqlite3_column_int(pStmt, iCol); } \
	cstr getStr(int iCol) { return sql_getStr(pStmt, iCol); } \
	xarray<byte> getBlob(int iCol) { return sql_getBlob(pStmt, iCol); }

struct sqlStmt1 { sqlite3_stmt* pStmt; int ec; SQLSTMT_COMMON };
sqlStmt1 __stdcall sqlFmt_(sqlite3 *db, VaArgFwd<cch*> va);
sqlStmt1 __stdcall sqlExec1(sqlite3 *db, VaArgFwd<cch*> va);
sqlStmt1 __stdcall sqlExec1(sqlite3 *db, cch* fmt, ...);
int sqlExec(sqlite3 *db, cch* fmt, ...);


struct SqlStmt1 : sqlStmt1 {
	SqlStmt1(); SqlStmt1(const sqlStmt1& that) {
		pStmt = that.pStmt; ec = that.ec; }
	template<typename... Args>
	SqlStmt1(sqlite3 *db, cch* fmt, Args... args) : 
		SqlStmt1(sqlExec1(db, fmt, args...)) {}
	~SqlStmt1() { sql_finalize(pStmt); }
};

// sql reading
DEF_RETPAIR(sql_read_int_t, int, out, int, ec);
sql_read_int_t sql_read_int(sqlite3 *db, cch* fmt, ...);

struct SqlStmt { union { 
	sqlite3_stmt* pStmt; int ec; };
	SqlStmt(sqlite3 *db, cch* fmt, ...);
	bool step(void); void stop(
		int ec); void chk(int ec);
	SQLSTMT_COMMON
};

#endif
