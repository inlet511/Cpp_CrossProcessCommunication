#include <iostream>
#include <chrono>
#include <iostream>
#include <Windows.h>
#include <array>


using namespace std;


std::array<int,4> get_time()
{
    // 获取当前时间
    auto now = std::chrono::system_clock::now();    

    std::array<int, 4> timeArray{};

    // 将时间转换为时分秒和毫秒
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    auto s = std::chrono::duration_cast<std::chrono::seconds>(ms);
    auto m = std::chrono::duration_cast<std::chrono::minutes>(s);
    auto h = std::chrono::duration_cast<std::chrono::hours>(m);

    timeArray.at(0) = h.count();
    timeArray.at(1) = m.count() % 60;
    timeArray.at(2) = s.count() % 60;
    timeArray.at(3) = ms.count() % 100;


    return std::move(timeArray);
    // 输出数组中的值
    //std::cout << "Current time: " << timeArray[0] << ":" << timeArray[1] << ":" << timeArray[2] << "." << timeArray[3] << std::endl;

}

int main() {

    // 创建文件映射对象
    HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 1024, L"GLOBAL\\shared_memory");

    if (hMapFile == NULL) {
        std::cout << "Error: Could not create file mapping object " << GetLastError() << std::endl;
        return 1;
    }

    // 将共享内存映射到进程地址空间
    LPVOID lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);

    if (lpMapAddress == NULL) {
        std::cout << "Error: Could not map view of file " << GetLastError() << std::endl;
        CloseHandle(hMapFile);
        return 1;
    }

    // 创建互斥对象
    HANDLE hMutex = CreateMutex(NULL, FALSE, L"GLOBAL\\MyMutexObject");

    if (hMutex == NULL)
    {
        cerr << "Could not create mutex object (" << GetLastError() << ")" << endl;
        CloseHandle(hMapFile);
        return 1;
    }


    while (true) {
        // 循环读取硬件输入信息
        std::array<int, 4> data = get_time();
        std::cout << "生成数据:  " << data[0] << ":" << data[1] << ":" << data[2] << "." << data[3] << std::endl;

        // 加锁
        WaitForSingleObject(hMutex, INFINITE);


        // 将读取的信息写入共享内存
        CopyMemory(lpMapAddress, &data, sizeof(data));

        // 释放锁
        ReleaseMutex(hMutex);

        // 等待一段时间
        Sleep(1);
    }

    // 卸载文件映射对象
    UnmapViewOfFile(lpMapAddress);

    // 关闭文件映射对象
    CloseHandle(hMapFile);

    // 关闭互斥对象
    CloseHandle(hMutex);

    return 0;
}
