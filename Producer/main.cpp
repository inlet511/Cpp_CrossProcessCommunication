#include <iostream>
#include <chrono>
#include <iostream>
#include <Windows.h>
#include <array>


using namespace std;


std::array<int,4> get_time()
{
    // ��ȡ��ǰʱ��
    auto now = std::chrono::system_clock::now();    

    std::array<int, 4> timeArray{};

    // ��ʱ��ת��Ϊʱ����ͺ���
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    auto s = std::chrono::duration_cast<std::chrono::seconds>(ms);
    auto m = std::chrono::duration_cast<std::chrono::minutes>(s);
    auto h = std::chrono::duration_cast<std::chrono::hours>(m);

    timeArray.at(0) = h.count();
    timeArray.at(1) = m.count() % 60;
    timeArray.at(2) = s.count() % 60;
    timeArray.at(3) = ms.count() % 100;


    return std::move(timeArray);
    // ��������е�ֵ
    //std::cout << "Current time: " << timeArray[0] << ":" << timeArray[1] << ":" << timeArray[2] << "." << timeArray[3] << std::endl;

}

int main() {

    // �����ļ�ӳ�����
    HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 1024, L"GLOBAL\\shared_memory");

    if (hMapFile == NULL) {
        std::cout << "Error: Could not create file mapping object " << GetLastError() << std::endl;
        return 1;
    }

    // �������ڴ�ӳ�䵽���̵�ַ�ռ�
    LPVOID lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);

    if (lpMapAddress == NULL) {
        std::cout << "Error: Could not map view of file " << GetLastError() << std::endl;
        CloseHandle(hMapFile);
        return 1;
    }

    // �����������
    HANDLE hMutex = CreateMutex(NULL, FALSE, L"GLOBAL\\MyMutexObject");

    if (hMutex == NULL)
    {
        cerr << "Could not create mutex object (" << GetLastError() << ")" << endl;
        CloseHandle(hMapFile);
        return 1;
    }


    while (true) {
        // ѭ����ȡӲ��������Ϣ
        std::array<int, 4> data = get_time();
        std::cout << "��������:  " << data[0] << ":" << data[1] << ":" << data[2] << "." << data[3] << std::endl;

        // ����
        WaitForSingleObject(hMutex, INFINITE);


        // ����ȡ����Ϣд�빲���ڴ�
        CopyMemory(lpMapAddress, &data, sizeof(data));

        // �ͷ���
        ReleaseMutex(hMutex);

        // �ȴ�һ��ʱ��
        Sleep(1);
    }

    // ж���ļ�ӳ�����
    UnmapViewOfFile(lpMapAddress);

    // �ر��ļ�ӳ�����
    CloseHandle(hMapFile);

    // �رջ������
    CloseHandle(hMutex);

    return 0;
}
