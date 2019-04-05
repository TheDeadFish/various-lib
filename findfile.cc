#include <stdshit.h>
#include "findfile.h"

/* should be part os stdshit
static
cstr REGCALL(2) remTrailSep(cstr str) { while(
	isPathSep(str.getr(-1))) str.slen--; return str; }
static
cstr REGCALL(2) getPath2(cstr str) {
	return getPath(remTrailSep(str)); }
*/


FindFiles_t::FindFiles_t(cch* nm) : FindFiles_t_(nm) {
	cstr path = ::getPath(fName); 
	pathLen = path.slen; char* end = path.end();
	if((path.slen && isPathSep(RW(end-1)))) down(); else { 
	if(RW(end) == '..') end++; if(RW(end) == '.') down(); }
	baseLen = pathLen; }
int FindFiles_t::down() { fName.pathcat("*.*");
	return release(pathLen, fName.slen-3); }
		
int FindFiles_t::first(HANDLE& hFind) {
	return next(findFirstFile(hFind, fName, this)); }
int FindFiles_t::next(HANDLE hFind) {
	return next(findNextFile(hFind, this)); }
int FindFiles_t::next(int ret) { if(ret < 0) { fName.slen 
	= pathLen; fName.pathcat(cStr()); } return ret;	}	

// findFile internal structure
struct FindFile_x_ { int flags; size_t errCode;
	size_t (__stdcall *cb)(int, FindFiles_t&); };
struct FindFile_x : FindFile_x_, FindFiles_t { 
	FindFile_x(cch* nm) : FindFiles_t(nm) {}
	size_t doFind(); };
	
size_t FindFile_x::doFind()
{
	#define IFRET2(...) if(ret = \
		__VA_ARGS__) goto RETURN;

	HANDLE hFind; int ret = first(hFind);
	while(ret < 0) {
		if(!isDir() || (flags & 4)) IFRET2(cb(0, *this));
		if(isDir()) { int oldPathLen = down();
			 IFRET2(doFind()); up(oldPathLen); }
		ret = next(hFind); }
	
	// handle error
	if(ret > 0) { if(flags & 2) { fName.slen = pathLen; 	
		IFRET2(cb(ret, *this)); } else { if((ret > 2) 
		||(!(flags & 1))) goto RETURN; errCode = 2; }}
	ret = 0; RETURN: REGFIX(S, ret); 
	FindClose(hFind); return ret;
}

size_t findFiles(cch* name, int flags, void* ctx,
	size_t (__stdcall *cb)(int, FindFiles_t&))
{
	FindFile_x ff{name}; ff.ctx = ctx;
	ff.cb = cb; ff.flags = flags; ff.errCode = 0;
	IFRET(ff.doFind()); return ff.errCode;
}
