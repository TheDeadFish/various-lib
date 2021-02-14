#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <windows.h>
#include <io.h>

#include "cFile.h"

static
HANDLE __stdcall get_osfhandle(FILE* _fp) {
	return (HANDLE)_get_osfhandle(_fileno(_fp));
}


int CFile::open(const char* name)
{
	return open(name, "rb");
}

int CFile::open(const char* name, const char* mode)
{
	close();
	_fp = fopen(name, mode);
	if(_fp) return 0;
	return errno;
}


void CFile::close()
{
	if(_fp) { fclose(_fp); _fp = 0; }
}

int CFile::_read(void* buff, size_t size)
{
	if(fread(buff, size, 1, _fp) != 1)
		return ferror(_fp) ? errno : -1;
	return 0;
}


void CFile::seek(size_t offset)
{
	fseek(_fp, offset, SEEK_SET);
}

int CFile::_mread(void** buff, size_t size)
{
	printf("%X, %X\n", buff, size);

	void* tmp = malloc(size);
	int ec = ENOMEM;
	if(tmp) {
		ec = read(tmp, size);
		if(ec) { free(tmp); tmp = 0; }
	}
	
	*buff = tmp;
	return ec;
}

size_t CFile::getSize()
{
	return GetFileSize(get_osfhandle(_fp), NULL);
}
