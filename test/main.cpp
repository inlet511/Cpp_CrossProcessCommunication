#include <QSystemSemaphore>
#include <QDebug>
#include <QThread>

int main(int argc, char *argv[])
{
    // Create a system semaphore with a key and initial count of 1
    QSystemSemaphore semaphore("MySemaphore", 1, QSystemSemaphore::Create);

    // Wait for a signal from the C++ program to release the semaphore
    qDebug() << "Waiting for signal to release semaphore...";
    semaphore.acquire();
    qDebug() << "Semaphore released!";
    semaphore.release();
    QThread::sleep(20);
    qDebug() << "Finished.";

    return 0;
}
