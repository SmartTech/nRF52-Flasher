#ifndef FLASHINGTHREAD_H
#define FLASHINGTHREAD_H

#include <QThread>
#include <QDebug>


#define debugLog     qDebug().noquote
#define infoLog      qInfo().noquote
#define warningLog   qWarning().noquote
#define errorLog     qCritical().noquote


enum {
    NRF_EXECUTE_RUN=0,
    NRF_EXECUTE_HALT,
    NRF_EXECUTE_RESET,
    NRF_EXECUTE_ERASE,
    NRF_EXECUTE_FLASH,
    NRF_EXECUTE_UNKNOWN
};

class QProcess;


class FlashingException
{
    QString m_what;
public:
    FlashingException(const QString& str);
    QString what() const;
};


class FlashingThread : public QThread
{
    Q_OBJECT
public:

    FlashingThread(const QString &_pathSD = QString(),
                   const QString &_pathAPP = QString(),
                   const QString &_pathBOOT = QString());

    void setPath(const QString &_pathSD, const QString &_pathAPP, const QString &_pathBOOT);

    void nrf_run();
    void nrf_halt();
    void nrf_reset();
    void nrf_erase();
    void nrf_flash();

private:

    void run() override;

    bool executeOpenOCD(QProcess *proc, const QStringList& args) throw(FlashingException);

    uint8_t executeType = NRF_EXECUTE_UNKNOWN;

    QString pathSD, pathAPP, pathBOOT;


private slots:

    void readOutput();

};

#endif // FLASHINGTHREAD_H
