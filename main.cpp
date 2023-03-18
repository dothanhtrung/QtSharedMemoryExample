#include <QCoreApplication>
#include <QCommandLineParser>
#include <QSharedMemory>
#include <QTimer>

class Test : public QObject {
Q_OBJECT
public:
    explicit Test(bool isWriter, QObject *parent = nullptr) : QObject(parent),
                                                              shm("QSharedMemoryExample") {
        if (!shm.create(8) && shm.error() == QSharedMemory::AlreadyExists)
            shm.attach();

        if (isWriter)
            connect(&timer, &QTimer::timeout, this, &Test::write);
        else
            connect(&timer, &QTimer::timeout, this, &Test::read);
        timer.setInterval(1000);
    };

    ~Test() override {
        if (shm.isAttached())
            shm.detach();
    }

    void start() {
        timer.start();
    };

private slots:
    void write() {
        count++;
        shm.lock();
        int *number = (int *) shm.data();
        *number = count;
        shm.unlock();
    };

    void read() {
        shm.lock();
        int *number = (int *) shm.constData();
        printf("Current number: %d.\n", *number);
        shm.unlock();
    };

private:
    QSharedMemory shm;
    QTimer timer;
    int count = 0;
};

#include "main.moc"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("Qt Shared Memory Example");
    parser.addOption({"w", "Is writer"});
    parser.process(a);

    Test *t;
    if (parser.isSet("w")) {
        printf("Writing to shared memory.\n");
        t = new Test(true);
    } else {
        printf("Reading from shared memory.\n");
        t = new Test(false);
    }

    t->start();

    return QCoreApplication::exec();
}
