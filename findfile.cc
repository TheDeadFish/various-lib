#include <stdshit.h>
#include "findfile.h"

FindFiles_t::FindFiles_t(cch* nm) : 
	FindFiles_t_(nm) { baseLen = -1; }
		
int FindFiles_t::first(HANDLE& hFind) {
	
	// setup the name
	if(baseLen < 0) { cstr path = ::getPath(fName);
		pathLen = baseLen = path.slen;
		char* end = path.end(); if(*end == '.') { 
			end++; VARFIX(end); if(*end == '.') end++; }
			if(!*end) goto DOWN;
	} else { DOWN: fName.pathcat("*.*");
		pathLen = fName.slen-3; }
		
		
	int ret = findFirstFile(hFind, fName, this);
	if(ret < 0) cat(cStr()); return ret;
}

int FindFiles_t::next(HANDLE hFind) {
	int ret = findNextFile(hFind, this);
	if(ret < 0) cat(cStr()); return ret; }

// findFile internal structure
struct FindFile_x_ { int flags; size_t errCode;
	size_t (__stdcall *cb)(int, FindFiles_t&); };
struct FindFile_x : FindFile_x_, FindFiles_t { 
	FindFile_x(cch* nm) : FindFiles_t(nm) {}
	size_t doFind(); };
	

	
size_t FindFile_x::doFind()
{
	FIND_FILES_ENTER(*this);

	FIND_FILES_LOOP(*this, 
		if(!isDir() || (flags & 4)) IFRET(cb(0, *this));
		if(isDir()) { IFRET(doFind()); } )
	
	// handle error
	if(ret > 0) { if(flags & 2) { fName.slen = pathLen; 	
		IFRET(cb(ret, *this)); } else { if((ret > 2) 
		||(!(flags & 1))) return ret; errCode = 2; }}
	return 0;
}

size_t findFiles(cch* name, int flags, void* ctx,
	size_t (__stdcall *cb)(int, FindFiles_t&))
{
	FindFile_x ff{name}; ff.ctx = ctx;
	ff.cb = cb; ff.flags = flags; ff.errCode = 0;
	IFRET(ff.doFind()); return ff.errCode;
}
