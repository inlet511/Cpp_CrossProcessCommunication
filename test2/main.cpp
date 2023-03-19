#include <windows.h>
#include <iostream>

int main()
{
    // Open the semaphore created by the Qt program
    HANDLE semaphore = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, "Global\\MySemaphore");

    if (semaphore == NULL) {
        std::cout << "Failed to open semaphore. Error code: " << GetLastError() << std::endl;
        return 1;
    }

    // Signal the semaphore to release the Qt program
    std::cout << "Signaling semaphore..." << std::endl;
    ReleaseMutex(semaphore);

    // Close the semaphore handle
    CloseHandle(semaphore);

    return 0;
}
