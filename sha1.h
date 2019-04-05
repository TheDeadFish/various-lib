// Sha1 hashing function/object
// File Version 1.20, 20/10/2017

#ifndef _SHA1_H_
#define _SHA1_H_
#include <stdio.h>

struct Sha1
{
	unsigned hash[5];
	
	void zero(); void ones(); void init();
	const void* next(const void* src, size_t sz);
	void last(const void* src, long long bytelength);
	void calc(const void* src, size_t sz);
	void merge(const void* src, size_t sz);
	void merge(Sha1& That);
	void toHexString(char* hexstring) const;
	void toHexString(wchar_t* hexstring) const;

	// comparison functions
	bool operator==(const Sha1& That) const;
	int qsort_comp(const Sha1& That) const;
	bool operator!=(const Sha1& That) const {
		return !(*this == That);}
	bool operator<(const Sha1& That) const {
		return qsort_comp(That) < 0; }
};

#endif
