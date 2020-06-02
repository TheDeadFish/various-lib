#include "stdshit.h"
#include "sha1.h"

/* SHA-1 hash in x86 assembly, (sha1-fast.S)
 * Copyright (c) 2014 Project Nayuki
 * http://www.nayuki.io/page/fast-sha1-hash-implementation-in-x86-assembly
 * (MIT License) - sha1_compress function as found in "sha1-fast.S" */
 
#define ROUND0a(a, b, c, d, e, i)  \
	movl    (i*4)(%edi), %esi;  \
	bswapl  %esi;               \
	movl    %esi, (i*4)(%esp);  \
	addl    %esi, %e;           \
	movl    %c, %esi;           \
	xorl    %d, %esi;           \
	andl    %b, %esi;           \
	xorl    %d, %esi;           \
	ROUNDTAIL(a, b, e, i, 0x5A827999)
	
#define SCHEDULE(i, e)  \
	movl  (((i- 3)&0xF)*4)(%esp), %esi;  \
	xorl  (((i- 8)&0xF)*4)(%esp), %esi;  \
	xorl  (((i-14)&0xF)*4)(%esp), %esi;  \
	xorl  (((i-16)&0xF)*4)(%esp), %esi;  \
	roll  $1, %esi;                      \
	addl  %esi, %e;                      \
	movl  %esi, ((i&0xF)*4)(%esp);
	
#define ROUND0b(a, b, c, d, e, i)  \
	SCHEDULE(i, e)   \
	movl  %c, %esi;  \
	xorl  %d, %esi;  \
	andl  %b, %esi;  \
	xorl  %d, %esi;  \
	ROUNDTAIL(a, b, e, i, 0x5A827999)
	
#define ROUND1(a, b, c, d, e, i)  \
	SCHEDULE(i, e)   \
	movl  %b, %esi;  \
	xorl  %c, %esi;  \
	xorl  %d, %esi;  \
	ROUNDTAIL(a, b, e, i, 0x6ED9EBA1)

#define ROUND2(a, b, c, d, e, i)  \
	SCHEDULE(i, e)     \
	movl  %c, %esi;    \
	movl  %c, %edi;    \
	orl   %d, %esi;    \
	andl  %b, %esi;    \
	andl  %d, %edi;    \
	orl   %edi, %esi;  \
	ROUNDTAIL(a, b, e, i, 0x8F1BBCDC)
	
#define ROUND3(a, b, c, d, e, i)  \
	SCHEDULE(i, e)   \
	movl  %b, %esi;  \
	xorl  %c, %esi;  \
	xorl  %d, %esi;  \
	ROUNDTAIL(a, b, e, i, 0xCA62C1D6)

#define ROUNDTAIL(a, b, e, i, k)  \
	roll  $30, %b;         \
	leal  k(%e,%esi), %e;  \
	movl  %a, %esi;        \
	roll  $5, %esi;        \
	addl  %esi, %e;

#define SHA1_COMPRESS .section .text$_sha1_compress; \
	.globl _sha1_compress@8;_sha1_compress@8:;\
	subl    $80, %esp;movl    %ebx, 64(%esp);movl    %esi, 68(%esp);\
	movl    %edi, 72(%esp);movl    %ebp, 76(%esp);movl    84(%esp), %esi;\
	movl    88(%esp), %edi;movl     0(%esi), %eax;movl     4(%esi), %ebx;\
	movl     8(%esi), %ecx;	movl    12(%esi), %edx;movl    16(%esi), %ebp;\
	ROUND0a(eax, ebx, ecx, edx, ebp,  0);ROUND0a(ebp, eax, ebx, ecx, edx,  1)\
	ROUND0a(edx, ebp, eax, ebx, ecx,  2);ROUND0a(ecx, edx, ebp, eax, ebx,  3)\
	ROUND0a(ebx, ecx, edx, ebp, eax,  4);ROUND0a(eax, ebx, ecx, edx, ebp,  5)\
	ROUND0a(ebp, eax, ebx, ecx, edx,  6);ROUND0a(edx, ebp, eax, ebx, ecx,  7)\
	ROUND0a(ecx, edx, ebp, eax, ebx,  8);ROUND0a(ebx, ecx, edx, ebp, eax,  9)\
	ROUND0a(eax, ebx, ecx, edx, ebp, 10);ROUND0a(ebp, eax, ebx, ecx, edx, 11)\
	ROUND0a(edx, ebp, eax, ebx, ecx, 12);ROUND0a(ecx, edx, ebp, eax, ebx, 13)\
	ROUND0a(ebx, ecx, edx, ebp, eax, 14);ROUND0a(eax, ebx, ecx, edx, ebp, 15)\
	ROUND0b(ebp, eax, ebx, ecx, edx, 16);ROUND0b(edx, ebp, eax, ebx, ecx, 17)\
	ROUND0b(ecx, edx, ebp, eax, ebx, 18);ROUND0b(ebx, ecx, edx, ebp, eax, 19)\
	ROUND1(eax, ebx, ecx, edx, ebp, 20);ROUND1(ebp, eax, ebx, ecx, edx, 21)\
	ROUND1(edx, ebp, eax, ebx, ecx, 22);ROUND1(ecx, edx, ebp, eax, ebx, 23)\
	ROUND1(ebx, ecx, edx, ebp, eax, 24);ROUND1(eax, ebx, ecx, edx, ebp, 25)\
	ROUND1(ebp, eax, ebx, ecx, edx, 26);ROUND1(edx, ebp, eax, ebx, ecx, 27)\
	ROUND1(ecx, edx, ebp, eax, ebx, 28);ROUND1(ebx, ecx, edx, ebp, eax, 29)\
	ROUND1(eax, ebx, ecx, edx, ebp, 30);ROUND1(ebp, eax, ebx, ecx, edx, 31)\
	ROUND1(edx, ebp, eax, ebx, ecx, 32);ROUND1(ecx, edx, ebp, eax, ebx, 33)\
	ROUND1(ebx, ecx, edx, ebp, eax, 34);ROUND1(eax, ebx, ecx, edx, ebp, 35)\
	ROUND1(ebp, eax, ebx, ecx, edx, 36);ROUND1(edx, ebp, eax, ebx, ecx, 37)\
	ROUND1(ecx, edx, ebp, eax, ebx, 38);ROUND1(ebx, ecx, edx, ebp, eax, 39)\
	ROUND2(eax, ebx, ecx, edx, ebp, 40);ROUND2(ebp, eax, ebx, ecx, edx, 41)\
	ROUND2(edx, ebp, eax, ebx, ecx, 42);ROUND2(ecx, edx, ebp, eax, ebx, 43)\
	ROUND2(ebx, ecx, edx, ebp, eax, 44);ROUND2(eax, ebx, ecx, edx, ebp, 45)\
	ROUND2(ebp, eax, ebx, ecx, edx, 46);ROUND2(edx, ebp, eax, ebx, ecx, 47)\
	ROUND2(ecx, edx, ebp, eax, ebx, 48);ROUND2(ebx, ecx, edx, ebp, eax, 49)\
	ROUND2(eax, ebx, ecx, edx, ebp, 50);ROUND2(ebp, eax, ebx, ecx, edx, 51)\
	ROUND2(edx, ebp, eax, ebx, ecx, 52);ROUND2(ecx, edx, ebp, eax, ebx, 53)\
	ROUND2(ebx, ecx, edx, ebp, eax, 54);ROUND2(eax, ebx, ecx, edx, ebp, 55)\
	ROUND2(ebp, eax, ebx, ecx, edx, 56);ROUND2(edx, ebp, eax, ebx, ecx, 57)\
	ROUND2(ecx, edx, ebp, eax, ebx, 58);ROUND2(ebx, ecx, edx, ebp, eax, 59)\
	ROUND3(eax, ebx, ecx, edx, ebp, 60);ROUND3(ebp, eax, ebx, ecx, edx, 61)\
	ROUND3(edx, ebp, eax, ebx, ecx, 62);ROUND3(ecx, edx, ebp, eax, ebx, 63)\
	ROUND3(ebx, ecx, edx, ebp, eax, 64);ROUND3(eax, ebx, ecx, edx, ebp, 65)\
	ROUND3(ebp, eax, ebx, ecx, edx, 66);ROUND3(edx, ebp, eax, ebx, ecx, 67)\
	ROUND3(ecx, edx, ebp, eax, ebx, 68);ROUND3(ebx, ecx, edx, ebp, eax, 69)\
	ROUND3(eax, ebx, ecx, edx, ebp, 70);ROUND3(ebp, eax, ebx, ecx, edx, 71)\
	ROUND3(edx, ebp, eax, ebx, ecx, 72);ROUND3(ecx, edx, ebp, eax, ebx, 73)\
	ROUND3(ebx, ecx, edx, ebp, eax, 74);ROUND3(eax, ebx, ecx, edx, ebp, 75)\
	ROUND3(ebp, eax, ebx, ecx, edx, 76);ROUND3(edx, ebp, eax, ebx, ecx, 77)\
	ROUND3(ecx, edx, ebp, eax, ebx, 78);ROUND3(ebx, ecx, edx, ebp, eax, 79)\
	movl    84(%esp), %esi;addl    %eax,  0(%esi);addl    %ebx,  4(%esi);\
	addl    %ecx,  8(%esi);addl    %edx, 12(%esi);addl    %ebp, 16(%esi);\
	movl    64(%esp), %ebx;movl    68(%esp), %esi;movl    72(%esp), %edi;\
	movl    76(%esp), %ebp;addl    $80, %esp;retl $8;

#define ms1(...) # __VA_ARGS__
#define ms2(...) ms1(__VA_ARGS__)
asm(ms2(SHA1_COMPRESS));
extern "C"
void __stdcall sha1_compress(
	void* state, const void* block);

void Sha1::zero(void)
{
	hash[0] = 0;
	hash[1] = 0;
	hash[2] = 0;
	hash[3] = 0;
	hash[4] = 0;	
}

void Sha1::ones(void)
{
	hash[0] = 0xFFFFFFFF;
	hash[1] = 0xFFFFFFFF;
	hash[2] = 0xFFFFFFFF;
	hash[3] = 0xFFFFFFFF;
	hash[4] = 0xFFFFFFFF;	
}

void Sha1::init(void)
{
	hash[0] = 0x67452301;
	hash[1] = 0xefcdab89;
	hash[2] = 0x98badcfe;
	hash[3] = 0x10325476;
	hash[4] = 0xc3d2e1f0;
}

const void* Sha1::next(const void* src, size_t sz)
{
	const int* src_ = (int*)src;
	size_t count = sz >> 6; while(count--) {
		sha1_compress(hash, src_); src_ += 16; }
	return src_;
}

void Sha1::last(const void* src, long long bytelength)
{
	byte block[64];
	size_t rem = bytelength & 63;
	memcpy(block, src, rem);
	block[rem++] = 0x80;
	if (64 - rem >= 8)
		memset(block + rem, 0, 56 - rem);
	else {
		memset(block + rem, 0, 64 - rem);
		sha1_compress(hash, block);
		memset(block, 0, 56);
	}	
	uint64_t longLen = ((uint64_t)bytelength) << 3;
	*(uint64_t*)(block+56) = __builtin_bswap64(longLen);
	sha1_compress(hash, block);
	for(int i = 0; i < 5; i++)
		hash[i] = __builtin_bswap32(hash[i]);
}

void Sha1::calc(const void* src, size_t sz)
{
	Sha1::init(); Sha1::last(
		Sha1::next(src, sz), sz);
}

typedef byte (*ARRAY_B20)[20];

void Sha1::toHexString(char* hexstring) const
{
	for(byte ch : *((ARRAY_B20)&hash)) {
		WRI(hexstring, tableOfHex[1][(ch >> 4) & 0xf]);
		WRI(hexstring, tableOfHex[1][ch & 0xf]); }
	*hexstring = '\0';
}

void Sha1::toHexString(wchar_t* hexstring) const
{
	for(byte ch : *((ARRAY_B20)&hash)) {
		WRI(hexstring, (wchar_t)tableOfHex[1][(ch >> 4) & 0xf]);
		WRI(hexstring, (wchar_t)tableOfHex[1][ch & 0xf]); }
	*hexstring = '\0';
}

static 
int hex_char(unsigned ch)
{
	u32 x = ch-'0'; if(x < 10) return x;
	x = (ch|0x20)-'a'; if(x < 6) return x+10;
	return -1;
}

#define HEX_CHAR(ch, ...) ({ u32 y = ch; VARFIX(y); \
	u32 x = y-'0'; if(x >= 10) { x = (y|0x20)-'a'; \
	if(x > 5){ __VA_ARGS__; } x+=10; } x; })

const char* Sha1::parse(const char* str)
{
	for(byte& ch : *((ARRAY_B20)&hash)) {
		ch = (HEX_CHAR(str[0], return 0)<<4)
			| HEX_CHAR(str[1], return 0); 
		str += 2; 
	}

	return str;
}

void Sha1::merge(Sha1& That)
{
	for(int i = 0; i < 5; i++) { hash[i] ^= That.hash[i]
		+ 0x9e3779b9 + (hash[i]<<6) + (hash[i]>>2); }
}

void Sha1::merge(const void* src, size_t sz)
{
	Sha1 tmp; tmp.calc(src, sz);
	this->merge(tmp);
}

bool Sha1::operator==(const Sha1& That) const {
	for(int i = 0; i < 5; i++) if(hash[i] != That.hash[i])
		return false; return true; }
int Sha1::qsort_comp(const Sha1& That) const {
	for(int i = 0; i < 5; i++) {
		if(hash[i] > That.hash[i]) return 1;
		if(hash[i] < That.hash[i]) return -1; }
	return 0; }
