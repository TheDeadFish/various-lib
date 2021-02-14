#pragma once

struct CFile
{
	// open/close
	CFile() : _fp(0) {}
	~CFile() { close(); }
	int open(const char* name);
	int open(const char* name, const char* mode);
	void close();

	// 

	
	
	
	
	//size_t read(void* buff, size_t size);
	
	// error handling
	//int 
	
	
	// file position
	void seek(size_t offset);
	size_t getSize();
	
	
	
	
	
	
	int _read(void* buff, size_t size);
	int _mread(void** buff, size_t size);
	
	// typed read
	template <class T>
	int read(T* buff, size_t size=1) {
		return _read(buff, sizeof(T)*size); }
	template <class T>
	int mread(T** buff, size_t size=1) {
		return _mread((void**)buff, sizeof(T)*size); }

	FILE* _fp;
};