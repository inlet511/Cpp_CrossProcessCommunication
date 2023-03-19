#include <Windows.h>
#include <iostream>
#include <array>

using namespace std;

int main()
{
    // 打开互斥对象
    HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, L"Global\\MyMutexObject");

    if (hMutex == NULL)
    {
        cerr << "Could not open mutex (" << GetLastError() << ")" << endl;
        return 1;
    }

    // 打开共享内存对象
    HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,L"shared_memory");

    if (hMapFile == NULL)
    {
        cerr << "Could not open file mapping object (" << GetLastError() << ")" << endl;
        CloseHandle(hMutex);
        return 1;
    }

    // 将共享内存映射到进程地址空间
    // 已知对象长度为4
    LPVOID pBuf = MapViewOfFile(hMapFile,FILE_MAP_ALL_ACCESS,0,0,sizeof(int)*4);

    if (pBuf == NULL)
    {
        cerr << "Could not map view of file (" << GetLastError() << ")" << endl;
        CloseHandle(hMapFile);
        CloseHandle(hMutex);
        return 1;
    }


    // 循环读取共享内存
    while (true)
    {
        // 加锁
        WaitForSingleObject(hMutex, INFINITE);

        // 读取数据
        std::array<int, 4>* iData = reinterpret_cast<std::array<int, 4>*>(pBuf);

        // 释放锁
        ReleaseMutex(hMutex);

        // 输出数据
        cout << "Program B received data: " << iData->at(0) << ":" << iData->at(1) << ":" << iData->at(2) << "." << iData->at(3)<< std::endl;

        // 等待一段时间
        Sleep(1);
    }

    // 卸载文件映射对象
    UnmapViewOfFile(pBuf);

    // 关闭文件映射对象
    CloseHandle(hMapFile);

    // 关闭互斥对象
    CloseHandle(hMutex);

    return 0;
}