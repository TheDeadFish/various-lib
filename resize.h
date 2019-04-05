#ifndef _RESIZE_H_
#define _RESIZE_H_

#define HORSIZ(n,d) (((n<<4)|(d&15))<<0)
#define HORPOS(n,d) (((n<<4)|(d&15))<<8)
#define VERSIZ(n,d) (((n<<4)|(d&15))<<16)
#define VERPOS(n,d) (((n<<4)|(d&15))<<24)
#define HOR_LEFT HORSIZ(0,0) | HORPOS(0,0)
#define HOR_RIGH HORSIZ(0,0) | HORPOS(1,1)
#define HOR_BOTH HORSIZ(1,1) | HORPOS(0,0)
#define HOR(d, p) HORSIZ(1,d) | HORPOS((p-1),d)
#define VER_TOPP VERSIZ(0,0) | VERPOS(0,0)
#define VER_BOTT VERSIZ(0,0) | VERPOS(1,1)
#define VER_BOTH VERSIZ(1,1) | VERPOS(0,0)
#define VER(d, p) VERSIZ(1,d) | VERPOS((p-1),d)
#define HVR_BOTH HOR_BOTH | VER_BOTH

struct WndResize
{
	WndResize() : wndInfo(0), nWndInfo(0) {}
	~WndResize() { free(wndInfo); }
	void init(HWND hwnd);
	void add(HWND hwnd, UINT ctrlId, UINT flags) {
		add(GetDlgItem(hwnd, ctrlId), flags); }
	void add(HWND hwnd, UINT flags);
	void add(RECT* rect, UINT flags);
	void resize(HWND hwnd, WPARAM wParam, LPARAM lParam);
	#pragma pack(push, 1)
	struct CtrlDef { WORD ctrlID; UINT flags; };
	#pragma pack(pop)
	void add(HWND hwnd, const CtrlDef* lst, int count);
	
//private:
	COORD orgSize; 
	
	struct WndInfo {
		union { 
			HWND hwnd;
			RECT* rect; };
		bool isWindow;
		BYTE flags[4];
		UINT points[4];
		
		int left()	{ return points[0]; }
		int top()	{ return points[2]; }
		int right()	{ return points[0] + points[1]; }
		int bottom(){ return points[2] + points[3]; }
		void setRect(RECT* rect) {
			points[0] = rect->right-rect->left;
			points[1] = rect->left;
			points[2] = rect->bottom-rect->top; 
			points[3] = rect->top; }
		int mulDiv(int offset, int index) {
			if(flags[index] == 0) return points[index];
			return points[index] + (offset * 
			(flags[index]>>4) / (flags[index] & 15)); }
	} *wndInfo;
	int nWndInfo;
};

#endif
