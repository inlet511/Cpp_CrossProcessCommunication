#include <Windows.h>
#include <iostream>
#include <array>

using namespace std;

int main()
{
    // �򿪻������
    HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, L"Global\\MyMutexObject");

    if (hMutex == NULL)
    {
        cerr << "Could not open mutex (" << GetLastError() << ")" << endl;
        return 1;
    }

    // �򿪹����ڴ����
    HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,L"shared_memory");

    if (hMapFile == NULL)
    {
        cerr << "Could not open file mapping object (" << GetLastError() << ")" << endl;
        CloseHandle(hMutex);
        return 1;
    }

    // �������ڴ�ӳ�䵽���̵�ַ�ռ�
    // ��֪���󳤶�Ϊ4
    LPVOID pBuf = MapViewOfFile(hMapFile,FILE_MAP_ALL_ACCESS,0,0,sizeof(int)*4);

    if (pBuf == NULL)
    {
        cerr << "Could not map view of file (" << GetLastError() << ")" << endl;
        CloseHandle(hMapFile);
        CloseHandle(hMutex);
        return 1;
    }


    // ѭ����ȡ�����ڴ�
    while (true)
    {
        // ����
        WaitForSingleObject(hMutex, INFINITE);

        // ��ȡ����
        std::array<int, 4>* iData = reinterpret_cast<std::array<int, 4>*>(pBuf);

        // �ͷ���
        ReleaseMutex(hMutex);

        // �������
        cout << "Program B received data: " << iData->at(0) << ":" << iData->at(1) << ":" << iData->at(2) << "." << iData->at(3)<< std::endl;

        // �ȴ�һ��ʱ��
        Sleep(1);
    }

    // ж���ļ�ӳ�����
    UnmapViewOfFile(pBuf);

    // �ر��ļ�ӳ�����
    CloseHandle(hMapFile);

    // �رջ������
    CloseHandle(hMutex);

    return 0;
}