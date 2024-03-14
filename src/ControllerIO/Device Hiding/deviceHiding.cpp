#include "deviceHiding.h"
#include <Windows.h>

//#define IOCTL_GET_WHITELIST = 0x80016000U;
#define IOCTL_SET_WHITELIST CTL_CODE(IoControlDeviceType, 2049, METHOD_BUFFERED, FILE_READ_DATA)
//#define IOCTL_GET_BLACKLIST = 0x80016008U;
//#define IOCTL_SET_BLACKLIST = 0x8001600CU;
#define IOCTL_GET_ACTIVE CTL_CODE(IoControlDeviceType, 2052, METHOD_BUFFERED, FILE_READ_DATA)
#define IOCTL_SET_ACTIVE  CTL_CODE(IoControlDeviceType, 2053, METHOD_BUFFERED, FILE_READ_DATA)
#define IOCTL_GET_WL_INVERT CTL_CODE(IoControlDeviceType, 2054, METHOD_BUFFERED, FILE_READ_DATA)
#define IOCTL_SET_WL_INVERT CTL_CODE(IoControlDeviceType, 2055, METHOD_BUFFERED, FILE_READ_DATA)
#define IoControlDeviceType 32769
#include <iostream>

void toggleDeviceHiding(const HANDLE& handle) {
    bool result = false;
    byte state = 0;
   // std::cout << "result" << result << " byte " << state << '\n';

    DeviceIoControl(handle, IOCTL_GET_WL_INVERT, NULL, 0, &result, 1, NULL, NULL);
    state = !result;


    //DeviceIoControl(handle, 0x80016010,NULL,0,&result,1,NULL,NULL);
    
    DeviceIoControl(handle, IOCTL_SET_WL_INVERT, &state, 1, NULL, 0, NULL, NULL);
    
  //  DeviceIoControl(handle, IOCTL_GET_WL_INVERT, NULL, 0, &result, 1, NULL, NULL);

  //  std::cout << "result" << result << " byte " << state << '\n';

}

void PCXSenseSeeThrough(const HANDLE& handle) {
    bool result = false;
    char path[62] = "C:\\Users\\Gustavo Santos\\Desktop\\Games\\PCXSense\\PCXSense.exe";
    path[60] = '\0';
    path[61] = '\0';

    DeviceIoControl(handle, IOCTL_SET_WHITELIST, &path, sizeof(path)/sizeof(path[0]), NULL, 0, NULL, NULL);

}

void hideDevice() {

    HANDLE handle = CreateFile(
        L"\\\\.\\HidHide",
        GENERIC_READ,
        (FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE),
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    PCXSenseSeeThrough(handle);

    CloseHandle(handle);
}