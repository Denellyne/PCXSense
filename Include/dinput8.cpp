#define CINTERFACE
#include "Minhook.h"
#include "dinput8.h"

#pragma comment (lib, "dxguid.lib")

HRESULT WINAPI dEnumDevicesA(
	LPDIRECTINPUT8A	This,
	DWORD dwDevType,
	LPDIENUMDEVICESCALLBACKA lpCallback,
	LPVOID pvRef,
	DWORD dwFlags
)
{

	if (dwDevType == 1)
	{
		return NULL;
	}
	else
	{
		return pEnumDevicesA(This, dwDevType, lpCallback, pvRef, dwFlags);
	}

}

HRESULT WINAPI dEnumDevicesW(
	LPDIRECTINPUT8W	This,
	DWORD dwDevType,
	LPDIENUMDEVICESCALLBACKW lpCallback,
	LPVOID pvRef,
	DWORD dwFlags
)
{

	if (dwDevType == 1)
	{
		return NULL;
	}
	else
	{
		return pEnumDevicesW(This, dwDevType, lpCallback, pvRef, dwFlags);
	}

}

HRESULT WINAPI DirectInput8Create(
	HINSTANCE hinst,
	DWORD dwVersion,
	REFIID	riidltf,
	LPVOID * ppvOut,
	LPUNKNOWN punkOuter
){

	HRESULT ret = oDirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);

	LPDIRECTINPUT8A pDICa = static_cast<LPDIRECTINPUT8A>(*ppvOut);
	LPDIRECTINPUT8W pDICu = static_cast<LPDIRECTINPUT8W>(*ppvOut);

	if (IsEqualIID(riidltf, IID_IDirectInput8A))
	{
		if (pDICa->lpVtbl->EnumDevices)
		{
			oEnumDevicesA = pDICa->lpVtbl->EnumDevices;

			MH_CreateHook((DWORD_PTR*)oEnumDevicesA, dEnumDevicesA, reinterpret_cast<void**>(&pEnumDevicesA));
			MH_EnableHook((DWORD_PTR*)oEnumDevicesA);
		}
	}
	
	if (IsEqualIID(riidltf, IID_IDirectInput8W))
	{
		if (pDICu->lpVtbl->EnumDevices)
		{
			oEnumDevicesW = pDICu->lpVtbl->EnumDevices;

			MH_CreateHook((DWORD_PTR*)oEnumDevicesW, dEnumDevicesW, reinterpret_cast<void**>(&pEnumDevicesW));
			MH_EnableHook((DWORD_PTR*)oEnumDevicesW);
		}
	}

	return ret;

}


bool WINAPI DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved) {
	static HMODULE din8DLL = nullptr;
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
	{
	
		if (MH_Initialize() != MH_OK)
		{
			MessageBox(NULL, ("Error"), ("Failed to Initialize MinHook"), MB_OK);
			return 1;
		}

		char path[MAX_PATH];
		GetSystemDirectory(path, MAX_PATH);
		strcat_s(path, "\\dinput8.dll");
		din8DLL = LoadLibrary(path);

		oDirectInput8Create = (tDirectInput8Create)GetProcAddress(din8DLL, "DirectInput8Create");

	}
	case DLL_PROCESS_DETACH:
		FreeLibrary(hModule);
		break;
	}
	return true;
}
