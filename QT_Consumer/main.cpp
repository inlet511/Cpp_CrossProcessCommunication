#include <QCoreApplication>
#include <QLocale>
#include <QSharedMemory>
#include <QDebug>
#include <QThread>
#include <QSystemSemaphore>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // 打开共享内存
      QSharedMemory sharedMemory("shared_memory");
      if (!sharedMemory.attach()) {
          qDebug() << "Failed to attach shared memory";
          return 1;
      }


      QSystemSemaphore semaphore("MyMutexObject",1,QSystemSemaphore::Open);


      while (true) {

          // 读取共享内存中的数组

          semaphore.acquire();
          int *data = (int*)sharedMemory.data();
          int array[4];
          for (int i = 0; i < 4; i++) {
              array[i] = data[i];
          }
          semaphore.release();


          // 打印数组
          qDebug() << "Received array:" << array[0] << "|" << array[1] << "|"<< array[2] << "|"<< array[3];

          QCoreApplication::processEvents();
          QThread::msleep(1);
      }

    return a.exec();
}
