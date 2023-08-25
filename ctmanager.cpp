#include "ctmanager.h"

CTManager::CTManager(QObject *parent) : QObject(parent)
{
}

CTManager::~CTManager()
{
    stopConsumer();
}

void CTManager::startConsumer()
{
    if (!m_consumerThread) {
        m_consumerThread = new QThread;
        m_consumerThread->start();

        // Connect thread started signal to message consumption
        connect(m_consumerThread, &QThread::started, this, [this]() {
            // Your message consumption logic here
        });
    }
}

void CTManager::stopConsumer()
{
    if (m_consumerThread) {
        m_consumerThread->quit();
        m_consumerThread->wait();

        delete m_consumerThread;
        m_consumerThread = nullptr;
    }
}
