#include "stdshit.h"
#include "resize.h"

void WndResize::add(HWND hwnd, const CtrlDef* lst, int count)
{
	while(count--) {add(hwnd,lst->ctrlID,
		lst->flags); lst++; }
}

static
RECT WINAPI GetChildRect(HWND hWnd) {
	RECT rect; GetWindowRect(hWnd, &rect);
    MapWindowPoints(HWND_DESKTOP, GetParent(hWnd), (LPPOINT) &rect, 2); 
	return rect; } 

void WndResize::init(HWND hwnd)
{
	RECT rect;
	GetClientRect(hwnd, &rect);
	orgSize.X = rect.right;
	orgSize.Y = rect.bottom;
}

void WndResize::add(HWND hwnd, UINT flags) 
{
	RECT rect = GetChildRect(hwnd);
	WndInfo& info = xNextAlloc(wndInfo, nWndInfo);
	info.isWindow = true;
	info.hwnd = hwnd;
	*(UINT*)&info.flags = flags;
	info.setRect(&rect);
}

void WndResize::add(RECT* rect, UINT flags)
{
	WndInfo& info = xNextAlloc(wndInfo, nWndInfo);
	info.isWindow = false;
	info.rect = rect;
	*(UINT*)&info.flags = flags;	
	info.setRect(rect);
}

void WndResize::resize(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	if((wParam == SIZE_MAXSHOW)
	||(wParam == SIZE_MAXHIDE)
	||(wParam == SIZE_MINIMIZED))
		return;
	SHORT deltaX = LOWORD(lParam)-orgSize.X;
	SHORT deltaY = HIWORD(lParam)-orgSize.Y;

	HDWP wndPos = BeginDeferWindowPos(nWndInfo);
	for(int i = 0; i < nWndInfo; i++) 
	{
		// calculate new size
		int sizeX = wndInfo[i].mulDiv(deltaX, 0);
		int posX = wndInfo[i].mulDiv(deltaX, 1);
		int sizeY = wndInfo[i].mulDiv(deltaY, 2);
		int posY = wndInfo[i].mulDiv(deltaY, 3);

		// perform the resize
		if(wndInfo[i].isWindow == false) {
			InvalidateRect(hwnd, wndInfo[i].rect, TRUE);
			wndInfo[i].rect->left = posX; wndInfo[i].rect->right = posX+sizeX;
			wndInfo[i].rect->top = posY; wndInfo[i].rect->bottom = posY+sizeY;
			InvalidateRect(hwnd, wndInfo[i].rect, TRUE); 
		} else {
			DeferWindowPos(wndPos, wndInfo[i].hwnd, 0, posX, posY,
				sizeX, sizeY, SWP_NOZORDER);
		}
	}
	EndDeferWindowPos(wndPos);
}
