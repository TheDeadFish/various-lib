// Hotpatch V2.22, 11/01/2014
// DeadFish Shitware

#ifndef _HOTPATCH_H_
#define _HOTPATCH_H_

#define HOTPATCH(old, new, pp) hotPatch((void*)old, (void*)new, (void**)pp)
#define HOTCALL(ftype, addr) (*((typeof(&ftype))(size_t(addr))))
void hotPatch(void* lpOldProc, void* lpNewProc, void** lpPatchProc);

#endif
