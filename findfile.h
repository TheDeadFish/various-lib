#ifndef _FINDFILE_H_
#define _FINDFILE_H_

struct FindFiles_t_ { void* ctx; 
	Bstr fName; int pathLen, baseLen; 
	FindFiles_t_(cch* name) : fName(name){} };

struct FindFiles_t : FindFiles_t_, WIN32_FIND_DATAU 
{
	cstr relPath() { return {
		fName.data+baseLen, pathLen-baseLen}; }
	cstr relName() { return fName.right(baseLen); }
	cstr getName() { return fName.right(pathLen); }
	cstr getPath() { return fName.left(pathLen); }

	FindFiles_t(cch*);
	
	int first(HANDLE&);
	int next(HANDLE); int next(int);
	
	void cat(cstr name) { fName.slen 
		= pathLen; fName.pathcat(name); }
	void cat(cch* name) { fName.slen 
		= pathLen; fName.pathcat(name); }
	
	
	#define FIND_FILES_LOOP(ff, ...) int ret; \
		{ HANDLE hFind; SCOPE_EXIT(FindClose(hFind)); \
		for(ret = (ff).first(hFind); ret < 0; \
			ret = (ff).next(hFind)) { __VA_ARGS__ }}
	#define FIND_FILES_ENTER(ff) SCOPE_REF( \
		int, oldPathLen, (ff).pathLen);
};







size_t findFiles(cch* name, int flags, void* ctx,
	size_t (__stdcall *cb)(int, FindFiles_t&));
enum { FF_ERR_OK = 1, FF_ERR_CB = 2, FF_DIR_CB = 4 };

#endif
