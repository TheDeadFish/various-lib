#ifndef _WAVE_H_
#define _WAVE_H_
#include "stdshit.h"

class WavFile
{
public:
	WavFile()								throw();
	~WavFile()								throw();
	WavFile(const WavFile& wf)				throw();
	bool copy(const WavFile& wf)			throw();
	bool alloc(void)						throw();
	void free(void)							throw();
	byte* release(void)						throw();
	int load(const char* file)				throw();
	int save(const char* file)				throw();
	enum{
		FILE_OK,
		BAD_NAME,
		BAD_FILE,
		INT_ERROR
	};

	int wordCount;
	int sampleRate;
	byte sampleBits;
	byte chanCount;
	union{
		byte* data8;
		short* data16;
	};

private:
	bool mustFree;
	int load(FILE* fp);
	int save(FILE* fp);
};

inline
WavFile::~WavFile()	throw()
{
	free();
}

#endif
