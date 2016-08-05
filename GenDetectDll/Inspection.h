#pragma once

#ifdef _WINDLL
#define VISIONMODULE_DLL __declspec(dllexport)
#else 
#define VISIONMODULE_DLL __declspec(dllimport)
#endif

#include <atlimage.h>

extern "C"
{
	VISIONMODULE_DLL BOOL WINAPI HasConnectedToCamera();
	VISIONMODULE_DLL BOOL WINAPI ConnectToCamera();
	VISIONMODULE_DLL BOOL WINAPI DisconnectFromCamera();
	VISIONMODULE_DLL BOOL WINAPI GrabImage(CImage& lpImage);
	VISIONMODULE_DLL BOOL WINAPI GrabHImage(HBITMAP& hImage);
	VISIONMODULE_DLL BOOL WINAPI GrabImageFromFileA(LPCSTR lpszFileName, CImage& lpImage);
	VISIONMODULE_DLL BOOL WINAPI GrabHImageFromFileA(LPCSTR lpszFileName, HBITMAP& hImage);
	VISIONMODULE_DLL BOOL WINAPI GrabImageFromFileW(LPCWSTR lpszFileName, CImage& lpImage);
	VISIONMODULE_DLL BOOL WINAPI GrabHImageFromFileW(LPCWSTR lpszFileName, HBITMAP& hImage);
	VISIONMODULE_DLL BOOL WINAPI DoInspect1(CImage& lpImage, CImage& lpReport, LONG& result);
	VISIONMODULE_DLL BOOL WINAPI DoHInspect1(HBITMAP& hImage, HBITMAP& hReport, LONG& result);
	VISIONMODULE_DLL BOOL WINAPI DoInspect1FromFileA(LPCSTR lpszFileName, CImage& lpImage,
		CImage& lpReport, LONG& result);
	VISIONMODULE_DLL BOOL WINAPI DoHInspect1FromFileA(LPCSTR lpszFileName, HBITMAP& hImage,
		HBITMAP& hReport, LONG& result);
	VISIONMODULE_DLL BOOL WINAPI DoInspect1FromFileW(LPCWSTR lpszFileName, CImage& lpImage,
		CImage& lpReport, LONG& result);
	VISIONMODULE_DLL BOOL WINAPI DoHInspect1FromFileW(LPCWSTR lpszFileName, HBITMAP& hImage,
		HBITMAP& hReport, LONG& result);
	VISIONMODULE_DLL BOOL WINAPI DoInspect2(CImage& lpImage, CImage& lpReport, LONG& result);
	VISIONMODULE_DLL BOOL WINAPI DoHInspect2(HBITMAP& hImage, HBITMAP& hReport, LONG& result);
	VISIONMODULE_DLL BOOL WINAPI DoInspect2FromFileA(LPCSTR lpszFileName, CImage& lpImage,
		CImage& lpReport, LONG& result);
	VISIONMODULE_DLL BOOL WINAPI DoHInspect2FromFileA(LPCSTR lpszFileName, HBITMAP& hImage,
		HBITMAP& hReport, LONG& result);
	VISIONMODULE_DLL BOOL WINAPI DoInspect2FromFileW(LPCWSTR lpszFileName, CImage& lpImage,
		CImage& lpReport, LONG& result);
	VISIONMODULE_DLL BOOL WINAPI DoHInspect2FromFileW(LPCWSTR lpszFileName, HBITMAP& hImage,
		HBITMAP& hReport, LONG& result);

#ifdef _UNICODE
#define GrabImageFromFile GrabImageFromFileW
#define DoInspect1FromFile DoInspect1FromFileW
#define DoInspect2FromFile DoInspect2FromFileW
#define DoHInspect1FromFile DoHInspect1FromFileW
#define DoHInspect2FromFile DoHInspect2FromFileW
#else
#define GrabImageFromFile GrabImageFromFileA
#define DoInspect1FromFile DoInspect1FromFileA
#define DoInspect2FromFile DoInspect2FromFileA
#define DoHInspect1FromFile DoHInspect1FromFileA
#define DoHInspect2FromFile DoHInspect2FromFileA
#endif

}