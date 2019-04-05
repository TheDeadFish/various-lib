#include "wavfile.h"
#include <errno.h>

WavFile::WavFile() throw()
{
	data8 = NULL;
	mustFree = false;
}

WavFile::WavFile(const WavFile& wf) throw()
{
	wordCount = wf.wordCount;
	sampleRate = wf.sampleRate;
	sampleBits = wf.sampleBits;
	chanCount = wf.chanCount;
	data8 = wf.data8;
	mustFree = false;
}

void WavFile::free(void) throw()
{
	if(mustFree)
	{
		::free(data8);
		data8 = NULL;
		mustFree = false;
	}
}

byte* WavFile::release(void) throw()
{
	mustFree = false;
	return data8;
}

bool WavFile::alloc(void) throw()
{
	free();
	data8 = (byte*)malloc(wordCount * (sampleBits/8));
	if(data8 == NULL)
		return false;
	mustFree = true;
	return true;
}

bool WavFile::copy(const WavFile& wf) throw()
{
	int oldSize = wordCount * (sampleBits/8);
	wordCount = wf.wordCount;
	sampleRate = wf.sampleRate;
	sampleBits = wf.sampleBits;
	chanCount = wf.chanCount;

	// allocate buffer
	int newSize = wordCount * (sampleBits/8);
	if((!mustFree) || (newSize != oldSize))
	{
		if(!alloc())
			return false;
	}
	memcpy(data8, wf.data8, wordCount * (sampleBits/8));
	return true;
}

class WaveHeader
{
public:
	byte ChunkID[4];
	u32 ChunkSize;
	u32 Format;
	byte SubChunk1ID[4];
	u32 SubChunk1Size;
	u16 AudioFormat;
	u16 NumChannels;
	u32 SampleRate;
	u32 ByteRate;
	u16 BlockAlign;
	u16 BitsPerSample;
	byte SubChunk2ID[4];
	u32 SubChunk2Size;
	
	int chkHeader(void);
	void setDefault(void);
	void setInfo(int words, int rate,
		byte bits, byte chans);
};

int WaveHeader::chkHeader(void)
{
	// check header
	if((memcmp(&ChunkID, "RIFF", 4))	
	or(	memcmp(&Format, "WAVE", 4))
	or( memcmp(&SubChunk1ID, "fmt ", 4))
	or( AudioFormat != 1 )
	or(	((BitsPerSample != 8) && (BitsPerSample != 16))))
		return -1;
	return SubChunk1Size-16;
}

void WaveHeader::setDefault(void)
{
	static const char head1[] = {
		0x52, 0x49, 0x46, 0x46, 0x00, 0x00, 0x00, 0x00,
		0x57, 0x41, 0x56, 0x45, 0x66, 0x6D, 0x74, 0x20,
		0x10, 0x00, 0x00, 0x00, 0x01, 0x00 };
	static const char head2[] = { 0x64, 0x61, 0x74, 0x61 };
	memcpy(&ChunkID, &head1, sizeof(head1));
	memcpy(&SubChunk2ID, &head2, 4);
}

void WaveHeader::setInfo( int words, int rate,
	byte bits, byte chans)
{
	// set members
	SampleRate = rate;
	BitsPerSample = bits;
	NumChannels = chans;

	// calculate members
	ByteRate = SampleRate * NumChannels * (BitsPerSample/8);
	BlockAlign = NumChannels * (BitsPerSample/8);
	SubChunk2Size = words * (BitsPerSample/8);
	ChunkSize = 36 + SubChunk2Size;
}

int WavFile::load(const char* file) throw()
{
	free();
	// open the file
	FILE* fp = xfopen(file, "rb");
	if(fp == NULL)
		return BAD_NAME;

	// read the header
	WaveHeader wh;
	if(fread(&wh, sizeof(wh)-8, 1, fp) != 1)
	{
		int eof = feof(fp);
		fclose(fp);
		if(eof == 0)
			return INT_ERROR;
		return BAD_FILE;
	}
	
	// check header and skip bullcrap
	int result = wh.chkHeader(); 
	if(result < 0)
	{
		fclose(fp);
		return BAD_FILE;
	}
	while(1)
	{
		fseek(fp, result, SEEK_CUR);
		if(fread(&wh.SubChunk2ID, 8, 1, fp) != 1)
		{
			int eof = feof(fp);
			fclose(fp);
			if(eof == 0)
				return INT_ERROR;
			return BAD_FILE;
		}
		if(memcmp(&wh.SubChunk2ID, "data", 4) == 0)
			break;
		result = wh.SubChunk2Size;
	}
	
	// setup members
	wordCount = wh.SubChunk2Size / (wh.BitsPerSample/8);
	sampleRate = wh.SampleRate;
	sampleBits = wh.BitsPerSample;
	chanCount = wh.NumChannels;
	
	// read file data
	if(!alloc())
	{
		fclose(fp);
		return BAD_FILE;
	}
	if(fread(data8, wh.SubChunk2Size, 1, fp) != 1)
	{
		int eof = feof(fp);
		fclose(fp);
		if(eof == 0)
			return INT_ERROR;
		return BAD_FILE;
	}
	fclose(fp);
	return FILE_OK;
}

int WavFile::save(const char* file) throw()
{
	// open the file
	FILE* fp = xfopen(file, "wb");
	if(fp == NULL)
		return BAD_NAME;
		
	// write the header	
	WaveHeader wh;
	wh.setDefault();
	wh.setInfo( wordCount, sampleRate, sampleBits, chanCount);
	if(fwrite(&wh, sizeof(wh), 1, fp) != 1)
	{
		fclose(fp);
		return INT_ERROR;
	}
	
	if(fwrite(data8, wh.SubChunk2Size, 1, fp) != 1)
	{
		fclose(fp);
		return INT_ERROR;
	}
	
	fclose(fp);
	return FILE_OK;
}
