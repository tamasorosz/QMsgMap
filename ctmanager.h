#ifndef CTMANAGER_H
#define CTMANAGER_H
//ConsumerThreadManager, very long
#include <QObject>
#include <QThread>

class CTManager : public QObject
{
    Q_OBJECT
public:
    explicit CTManager(QObject *parent = nullptr);
    ~CTManager();

    void startConsumer();
    void stopConsumer();

private:
    QThread *m_consumerThread = nullptr;
};

#endif // CTMANAGER_H
