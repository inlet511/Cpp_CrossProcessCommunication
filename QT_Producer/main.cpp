#include <QCoreApplication>
#include <QSharedMemory>
#include <QDebug>
#include <QTime>
#include <QThread>
#include <QSystemSemaphore>
#include <QString>
#include <windows.h>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // 创建一个共享内存对象
    QSharedMemory sharedMemory("shared_memory");

    // 创建一个系统级的Semaphore
    QSystemSemaphore semaphore("MyMutexObject",1,QSystemSemaphore::Create);

    // 创建一个4个int长度的共享内存
    if (!sharedMemory.create(sizeof(int)*4)) {
        qDebug() << "Failed to create shared memory";
        return 1;
    }

    while (true) {

        //获取当前时间
        QTime currentTime = QTime::currentTime();

        // 分解为四个整数
        int hours = currentTime.hour();
        int minutes = currentTime.minute();
        int seconds = currentTime.second();
        int milliseconds = currentTime.msec();


        // 将数组写入共享内存
        semaphore.acquire();
        int *data = (int*)sharedMemory.data();
        data[0] = hours;
        data[1] = minutes;
        data[2] = seconds;
        data[3] = milliseconds;
        semaphore.release();

        qDebug() << "memory set:" << data[0] << "|" << data[1] <<"|"<< data[2] <<"|"<< data[3];


        QCoreApplication::processEvents();
        QThread::msleep(1);
    }

    return a.exec();
}
