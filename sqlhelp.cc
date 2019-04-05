#include <stdshit.h>
#include "sqlhelp.h"

int __stdcall sql_open(sqlite3 **db, cch* fName, SqlOpenMode mode) 
	{ return sqlite3_open_v2(fName, db, mode.mode, 0); }

ASM_FUNC("sql_finalize_", "push %eax; push %edx; push %ecx;"
	"push 16(%esp); call _sqlite3_finalize; add $4, %esp;"
	"pop %ecx; pop %edx; pop %eax; ret $4");
	

cstr sql_getStr(sqlite3_stmt* pStmt, int iCol) {
	return {(cch*)sqlite3_column_text(pStmt, iCol), 
		sqlite3_column_bytes(pStmt, iCol) }; }
xarray<byte> sql_getBlob(sqlite3_stmt* pStmt, int iCol) {
	return {(byte*)sqlite3_column_blob(pStmt, iCol),
		sqlite3_column_bytes(pStmt, iCol)}; }

struct sqlFmt_ctx { void** va0;
	struct arg_t { u8 type, idx; };
	arg_t argv[256]; int argc; };

size_t sqlFmt_cb(xstrfmt_fmt* ctx, char ch)
{
	if(!is_one_of(ch, 'b', '~', '@')) return -1;	
	void** ap = (void**)ctx->ap; ctx->ap = 
	(va_list)(ap + ((ch == 'b') ? 2 : 1));
	if(ctx->dstPosArg == NULL) return 1;
	
	sqlFmt_ctx* ctx2 = Void(ctx->cbCtx);
	byte xh = ((ctx->flags & 24) * 8);
	if(ch == 'b') xh |= 0x80;
	ctx2->argv[ctx2->argc++] = {ch^xh,ap
	-ctx2->va0}; *ctx->dstPosArg = '?';
	return (size_t)ctx->dstPosArg+1;
}

sqlStmt1 sqlFmt_(
	sqlite3 *db, VaArgFwd<cch*> va)
{
	// prepare format text
	sqlFmt_ctx ctx; ctx.argc = 0;
	ctx.va0 = (void**)va.start();
	char* str = xstrfmt(&ctx, &sqlFmt_cb, va);

	// create sql statement
	sqlite3_stmt* pStmt;
	int ret = sqlite3_prepare(db,
		str, -1, &pStmt, 0);
	free(str); if(ret) return {0, ret};
	
	// assign blobs
	for(int i = 0; i < ctx.argc; i++) {	byte type = ctx.argv[i].type;
		void** data = ctx.va0+ctx.argv[i].idx;
		if(type & 0x20) { int len = (type & 0x80) ? (int)data[1] : -1;
		auto frfn = !(type & 0x40) ? free_ : SQLITE_STATIC;
		if(!(type&4)) { ret = sqlite3_bind_blob(pStmt, i+1, *data, len, frfn);
		} else { ret = sqlite3_bind_text(pStmt, i+1, (char*)*data, len, frfn); }
		} else { ret = sqlite3_bind_int(pStmt, i+1, (int)*data); } 
		if(ret) { sql_finalize(pStmt); return {0, ret}; }
	}  return {pStmt, 0};
}

sqlStmt1 sqlExec1(sqlite3 *db, VaArgFwd<cch*> va)
{
	auto tmp = sqlFmt_(db, va);
	if(tmp) { tmp.ec = sqlite3_step(tmp);
	if(tmp.ec == SQLITE_ROW) tmp.ec = 0;
	ei(tmp.ec == SQLITE_DONE) tmp.ec = -1; 
	} return tmp;
}

// single step functions getters
int sqlExec(sqlite3 *db, cch* fmt, ...) {
	VA_ARG_FWD(fmt); auto tmp = sqlExec1(db, va);
	sql_finalize(tmp); return max(0,tmp.ec); }
sqlStmt1 __stdcall sqlExec1(sqlite3 *db, cch* fmt, ...) {
	VA_ARG_FWD(fmt); return sqlExec1(db, va); }
sql_read_int_t sql_read_int(sqlite3 *db, cch* fmt, ...) {
	VA_ARG_FWD(fmt); auto tmp = sqlExec1(db, va);
	int ret = sqlite3_column_int(tmp, 0);
	sql_finalize(tmp); return {ret, tmp.ec}; }
	
/*cstr sql_read_str(sqlite3 *db, int* ec, cch* fmt, ...) {
	VA_ARG_FWD(fmt); auto tmp = sqlExec1(db, va);
	cstr ret = sql_dupStr(tmp, 0); if(ec) *ec = tmp.ec;
	sqlite3_finalize(tmp); return ret; }
cstr sql_read_str2(sqlite3 *db, int* ec, cch* fmt, ...) {
	VA_ARG_FWD(fmt); auto tmp = sqlExec1(db, va);
	cstr ret = sql_dupStr2(tmp, 0); if(ec) *ec = tmp.ec;
	sqlite3_finalize(tmp); return ret; } */

// string duplication helpers

/*cstr sql_dupStr(sqlite3_stmt* pStmt, int iCol) {
	cstr ret = sql_getStr(pStmt, iCol);
	if(!ret) return ret; return ret.xdup();  }
cstr sql_dupStr2(sqlite3_stmt* pStmt, int iCol) {
	return sql_getStr(pStmt, iCol).xdup(); }
*/

SqlStmt::SqlStmt(sqlite3 *db, cch* fmt, ...)
{
	VA_ARG_FWD(fmt); auto tmp = sqlFmt_(db, va);
	if(tmp) pStmt = tmp; else ec = tmp.ec;
}

bool SqlStmt::step(void)
{ 
	if(!IS_PTR(pStmt)) return false;
	int rc = sqlite3_step(pStmt);
	if(rc == SQLITE_ROW) return true;
	if(rc == SQLITE_DONE) rc = 0;
	stop(rc); return false;
}

void SqlStmt::stop(int ec)
{
	sql_finalize(pStmt); 
	pStmt = Void(ec);
}

void SqlStmt::chk(int ec)
{
	if(ec) stop(ec);
}
