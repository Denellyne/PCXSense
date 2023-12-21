#pragma once
#include <dinput.h>

typedef HRESULT(WINAPI *tDirectInput8Create)(
	HINSTANCE				 hinst,
	DWORD				     dwVersion,
	REFIID					 riidltf,
	LPVOID *			     ppvOut,
	LPUNKNOWN				 punkOuter);

tDirectInput8Create oDirectInput8Create;

typedef HRESULT(WINAPI *tEnumDevicesA)(
	LPDIRECTINPUT8A			 This,
	DWORD					 dwDevType,
	LPDIENUMDEVICESCALLBACKA lpCallback,
	LPVOID				     pvRef,
	DWORD					 dwFlags);

tEnumDevicesA oEnumDevicesA;
tEnumDevicesA pEnumDevicesA;

typedef HRESULT(WINAPI *tEnumDevicesW)(
	LPDIRECTINPUT8W			 This,
	DWORD					 dwDevType,
	LPDIENUMDEVICESCALLBACKW lpCallback,
	LPVOID				     pvRef,
	DWORD					 dwFlags);


tEnumDevicesW oEnumDevicesW;
tEnumDevicesW pEnumDevicesW;
