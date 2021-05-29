//Win clay

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include <windows.h>
#include <ddraw.h>
#include <stdio.h>
#include <stdarg.h>
#include "resource.h"

//-----------------------------------------------------------------------------
// Local definitions
//-----------------------------------------------------------------------------
#define NAME                "Clayworks"
#define TITLE               "Clayworks 3.0"

//-----------------------------------------------------------------------------
// Default settings
//-----------------------------------------------------------------------------
#define TIMER_ID            1
#define TIMER_RATE          500

//-----------------------------------------------------------------------------
// Global data
//-----------------------------------------------------------------------------
LPDIRECTDRAW4               g_pDD = NULL;        // DirectDraw object
LPDIRECTDRAWSURFACE4        g_pDDSPrimary = NULL;// DirectDraw primary surface
LPDIRECTDRAWSURFACE4        g_pDDSBack = NULL;   // DirectDraw back surface
BOOL                        g_bActive = FALSE;   // Is application active?
HWND                        hWnd;
//-----------------------------------------------------------------------------
// Local data
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: ReleaseAllObjects()
// Desc: Finished with all objects we use; release them
//-----------------------------------------------------------------------------
static void
ReleaseAllObjects(void)
{
	if (g_pDD != NULL)
	{
		if (g_pDDSPrimary != NULL)
		{
			g_pDDSPrimary->Release();
			g_pDDSPrimary = NULL;
		}
		g_pDD->Release();
		g_pDD = NULL;
	}
}




//-----------------------------------------------------------------------------
// Name: InitFail()
// Desc: This function is called if an initialization function fails
//-----------------------------------------------------------------------------
HRESULT
InitFail(HWND hWnd, HRESULT hRet, LPCTSTR szError,...)
{
	char                        szBuff[128];
	va_list                     vl;

	va_start(vl, szError);
	vsprintf(szBuff, szError, vl);
	ReleaseAllObjects();
	MessageBox(hWnd, szBuff, TITLE, MB_OK);
	DestroyWindow(hWnd);
	va_end(vl);
	return hRet;
}




//-----------------------------------------------------------------------------
// Name: UpdateFrame()
// Desc: Displays the proper text for the page
//-----------------------------------------------------------------------------
static BOOL UpdateFrame()
{
	static BYTE                 phase = 0;
	HDC                         hdc;
	DDBLTFX                     ddbltfx;
	RECT                        rc;
	SIZE                        size;
	unsigned short *scrptr;
	HRESULT                     hRet;	

	DDSURFACEDESC2              ddsd;
	ddsd.dwSize = sizeof(ddsd);
	hRet=g_pDDSPrimary->Lock(NULL, &ddsd, 0, NULL);
	if (hRet != DD_OK)
	{
		switch (hRet)
		{		          
		   case DDERR_INVALIDOBJECT:InitFail(hWnd, hRet, "Invalid object");break;         	
		   case DDERR_INVALIDPARAMS:InitFail(hWnd, hRet, "Invalid params");break;         	
		   case DDERR_OUTOFMEMORY:InitFail(hWnd, hRet, "No memory");break;         	
		   case DDERR_SURFACEBUSY:InitFail(hWnd, hRet, "Busy bee");break;         	
		   case DDERR_SURFACELOST:InitFail(hWnd, hRet, "Can't be found");break;         	
		   case DDERR_WASSTILLDRAWING:InitFail(hWnd, hRet, "Still waiting");break;         	

		   default:InitFail(hWnd, hRet, "Surface all screwy");
		}
		return FALSE;
	}

	scrptr=(unsigned short *)ddsd.lpSurface;
	int i=0;
	scrptr[500]=0xFFFF;
	while (i++<640*480)
	  scrptr[i]=0xf999;
	
	g_pDDSPrimary->Unlock(NULL);
   
	// Use the blter to do a color fill to clear the back buffer
	/*ZeroMemory(&ddbltfx, sizeof(ddbltfx));
	ddbltfx.dwSize = sizeof(ddbltfx);
	ddbltfx.dwFillColor = 0;
	g_pDDSBack->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);

	if (g_pDDSBack->GetDC(&hdc) == DD_OK)
	{
		SetBkColor(hdc, RGB(0, 0, 255));
		SetTextColor(hdc, RGB(255, 255, 0));
		if (phase)
		{
			GetClientRect(hWnd, &rc);
			GetTextExtentPoint(hdc, szMsg, lstrlen(szMsg), &size);
			TextOut(hdc, (rc.right - size.cx) / 2, (rc.bottom - size.cy) / 2,
					szMsg, sizeof(szMsg) - 1);
			TextOut(hdc, 0, 0, szFrontMsg, lstrlen(szFrontMsg));
			phase = 0;
		}
		else
		{
			TextOut(hdc, 0, 0, szBackMsg, lstrlen(szBackMsg));
			phase = 1;
		}
		g_pDDSBack->ReleaseDC(hdc);
	}*/
	return TRUE;
}




//-----------------------------------------------------------------------------
// Name: WindowProc()
// Desc: The Main Window Procedure
//-----------------------------------------------------------------------------
long FAR PASCAL
WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HRESULT                     hRet;

	switch (message)
	{
		case WM_ACTIVATEAPP:
			// Pause if minimized or not the top window
			g_bActive = (wParam == WA_ACTIVE) || (wParam == WA_CLICKACTIVE);
			return 0L;
		case WM_DESTROY:
			// Clean up and close the app
			ReleaseAllObjects();
			PostQuitMessage(0);
			return 0L;

		case WM_KEYDOWN:
			// Handle any non-accelerated key commands
			UpdateFrame();	
			switch (wParam)
			{
				case VK_ESCAPE:
				case VK_F12:
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					return 0L;			 
			}
			break;

		case WM_SETCURSOR:
			// Turn off the cursor since this is a full-screen app
			//SetCursor(NULL);
			return TRUE;        
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}




//-----------------------------------------------------------------------------
// Name: InitApp()
// Desc: Do work required for every instance of the application:
//          Create the window, initialize data
//-----------------------------------------------------------------------------
static HRESULT
InitApp(HINSTANCE hInstance, int nCmdShow)
{
	
	WNDCLASS                    wc;
	DDSURFACEDESC2              ddsd;
	DDSCAPS2                    ddscaps;
	HRESULT                     hRet;
	LPDIRECTDRAW                pDD;

	// Set up and register window class
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAIN_ICON));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH )GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NAME;
	wc.lpszClassName = NAME;
	RegisterClass(&wc);

	// Create a window
	hWnd = CreateWindowEx(WS_EX_TOPMOST,NAME,TITLE,WS_POPUP,0,0,
						  GetSystemMetrics(SM_CXSCREEN),
						  GetSystemMetrics(SM_CYSCREEN),
						  NULL,NULL,hInstance,NULL);
	if (!hWnd)
		return FALSE;
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	SetFocus(hWnd);

	///////////////////////////////////////////////////////////////////////////
	// Create the main DirectDraw object
	///////////////////////////////////////////////////////////////////////////
	hRet = DirectDrawCreate(NULL, &pDD, NULL);
	if (hRet != DD_OK)
		return InitFail(hWnd, hRet, "DirectDrawCreate FAILED");

	// Fetch DirectDraw4 interface
	hRet = pDD->QueryInterface(IID_IDirectDraw4, (LPVOID *) & g_pDD);
	if (hRet != DD_OK)
		return InitFail(hWnd, hRet, "QueryInterface FAILED");

	// Get exclusive mode
	hRet = g_pDD->SetCooperativeLevel(hWnd,  DDSCL_EXCLUSIVE |DDSCL_FULLSCREEN|DDSCL_ALLOWREBOOT |DDSCL_NOWINDOWCHANGES);//|
	if (hRet != DD_OK)
		return InitFail(hWnd, hRet, "SetCooperativeLevel FAILED");

	// Set the video mode to 640x480x16
	hRet = g_pDD->SetDisplayMode(640, 480, 16, 0, 0);
	if (hRet != DD_OK)
		return InitFail(hWnd, hRet, "SetDisplayMode FAILED");

	// Create the primary surface with no back buffers

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	hRet = g_pDD->CreateSurface(&ddsd, &g_pDDSPrimary, NULL);
	if (hRet != DD_OK)
		return InitFail(hWnd, hRet, "CreateSurface FAILED");
	// Get a pointer to the back buffer
	/*ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
	hRet = g_pDDSPrimary->GetAttachedSurface(&ddscaps, &g_pDDSBack);
	if (hRet != DD_OK)
		return InitFail(hWnd, hRet, "GetAttachedSurface FAILED");

	// Create a timer to flip the pages
	if (TIMER_ID != SetTimer(hWnd, TIMER_ID, TIMER_RATE, NULL))
		return InitFail(hWnd, hRet, "SetTimer FAILED");
		*/    
	return DD_OK;
}




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Initialization, message loop
//-----------------------------------------------------------------------------
int PASCAL
WinMain(HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPSTR lpCmdLine,
		int nCmdShow)
{
	MSG                         msg;

	if (InitApp(hInstance, nCmdShow) != DD_OK)
		return FALSE;
	

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		
	}
	return msg.wParam;
}
