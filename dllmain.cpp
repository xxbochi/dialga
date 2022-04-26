// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <iostream>
#include <vector>
#include<bitset>
#include <string> 
#include <iomanip>
#include <sstream>

HMODULE myhModule;

DWORD __stdcall EjectThread(LPVOID lpParameter) {
    Sleep(100);
    FreeLibraryAndExitThread(myhModule, 0);
}

DWORD GetAddressFromSignature(std::vector<int> signature, DWORD startaddress = 0, DWORD endaddress = 0) {
    std::cout << "searching...";
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    if (startaddress == 0) {
        startaddress = (DWORD)(si.lpMinimumApplicationAddress);
    }
    if (endaddress == 0) {
        endaddress = (DWORD)(si.lpMaximumApplicationAddress);
    }

    MEMORY_BASIC_INFORMATION mbi{ 0 };
    DWORD protectflags = (PAGE_GUARD | PAGE_NOCACHE | PAGE_NOACCESS);

    for (DWORD i = startaddress; i < endaddress - signature.size(); i++) {
        //std::cout << "scanning: " << std::hex << i << std::endl;
        if (VirtualQuery((LPCVOID)i, &mbi, sizeof(mbi))) {
            if (mbi.Protect & protectflags || !(mbi.State & MEM_COMMIT)) {
               // std::cout << "Bad Region! Region Base Address: " << mbi.BaseAddress << " | Region end address: " << std::hex << (int)((DWORD)mbi.BaseAddress + mbi.RegionSize) << std::endl;
                i += mbi.RegionSize;
                continue; // if bad adress then dont read from it
            }
            // std::cout << "Good Region! Region Base Address: " << mbi.BaseAddress << " | Region end address: " << std::hex << (int)((DWORD)mbi.BaseAddress + mbi.RegionSize) << std::endl;
            for (DWORD k = (DWORD)mbi.BaseAddress; k < (DWORD)mbi.BaseAddress + mbi.RegionSize - signature.size(); k++) {
                for (DWORD j = 0; j < signature.size(); j++) {
                    if (signature.at(j) != -1 && signature.at(j) != *(byte*)(k + j))
                        break;
                    if (j + 1 == signature.size())
                        return k;
                }
            }
            i = (DWORD)mbi.BaseAddress + mbi.RegionSize;
        }
    }
    return NULL;
}

DWORD WINAPI Menu() {
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    std::cout << "test" << std::endl;
    while (1) {
        Sleep(100);
        if (GetAsyncKeyState(VK_NUMPAD5)) 
            break;
        
            
        if (GetAsyncKeyState(VK_NUMPAD3)) {
            std::vector<int> sig = { 0xA3, 0x2C, 0x6A, -1, -1, 0xD9, 0xEE, 0xDD, 0x5C, 0x24, 0x10, 0x83, 0x3D, -1, -1, };
            DWORD Entry = GetAddressFromSignature(sig, 0x01000000, 0x20000000);
            if (Entry == NULL)
                Entry = GetAddressFromSignature(sig, 0x1F000000, 0x4A000000);
            if (Entry == NULL)
                Entry = GetAddressFromSignature(sig);
           // std::cout << "Result: " << (int)Entry << std::endl;
            intptr_t value = *(intptr_t*)Entry;
            std::cout << "found bytes! : " << value << std::endl;
            const std::uintptr_t whatever = *reinterpret_cast<std::uintptr_t*>((int)Entry + 1);
            std::cout << whatever;

            while (2)
            {
                std::string s = std::to_string(*(intptr_t*)whatever);
                std::cout << s << std::endl;
            }

/* 
            std::ostringstream ss;
            ss << std::hex << value;
            std::string resultingString = ss.str();
            std::cout << resultingString;
            // std::string s = std::hex << std::to_string(value);
            int n = resultingString.length();
            std::cout << n;
        */
            // declaring character array
          //  char char_array [9];

            // copying the contents of the
            // string to char array
            //strcpy_s(char_array, resultingString.c_str());

        //    for (int l = 0; l < n; l++)
             //   std::cout << char_array[l];

           


           
        }

       
    }
    fclose(fp);
    FreeConsole();
    CreateThread(0, 0, EjectThread, 0, 0, 0);
    int i = 0;
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        myhModule = hModule;
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Menu, NULL, 0, NULL);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

