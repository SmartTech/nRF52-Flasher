#include <QCoreApplication>

#include "flashingthread.h"

// ------------ FlashingException ------------
FlashingException::FlashingException(const QString &str) :
    m_what(str){}

QString FlashingException::what() const { return m_what; }


// ------------ FlashingThread ------------
FlashingThread::FlashingThread(const QString &_pathSD, const QString &_pathAPP, const QString &_pathBOOT) :
    QThread(), pathSD(_pathSD), pathAPP(_pathAPP), pathBOOT(_pathBOOT)
{
}

void FlashingThread::setPath(const QString &_pathSD, const QString &_pathAPP, const QString &_pathBOOT)
{
    pathSD   = _pathSD;
    pathAPP  = _pathAPP;
    pathBOOT = _pathBOOT;
}

void FlashingThread::run()
{
    std::unique_ptr<QProcess> proc;

    try {
        proc.reset(new QProcess());

        proc->setReadChannelMode(QProcess::MergedChannels);
        proc->setReadChannel(QProcess::StandardOutput);
        connect(proc.get(), SIGNAL(readyRead()), SLOT(readOutput()));
        connect(proc.get(), &QProcess::errorOccurred, this, &FlashingThread::errorOccurred);

        switch(executeType)
        {
            case NRF_EXECUTE_RESET :
                infoLog() << tr("Resetting device...");
                if (!executeOpenOCD(proc.get(), {"-f", "openocd.cfg", "-c",
                                    "init; reset; shutdown;" }))
                throw FlashingException(tr("Failed to RESET device"));
            break;
            case NRF_EXECUTE_HALT :
                if (!executeOpenOCD(proc.get(), {"-f", "openocd.cfg", "-c",
                                    "init; halt; shutdown;" }))
                throw FlashingException(tr("Failed to HALT device"));
                infoLog() << tr("Device halted");
            break;
            case NRF_EXECUTE_ERASE :
                infoLog() << tr("Erasing device...");
                if (!executeOpenOCD(proc.get(), {"-f", "openocd.cfg", "-c",
                                    "init; reset halt; nrf52 mass_erase 0; shutdown;" }))
                throw FlashingException(tr("Failed to erase device"));
                infoLog() << tr("OK!");
            break;
            case NRF_EXECUTE_FLASH :
                infoLog() << tr("Start flashing device...");
                infoLog() << pathSD;
                if (!executeOpenOCD(proc.get(), {"-f", "openocd.cfg", "-c",
                                    "init; program \"" + QString(pathSD) + "\";program \"" + pathAPP + "\";program \"" + pathBOOT + "\";reset;shutdown;" }))
                throw FlashingException(tr("Failed to erase device"));
                infoLog() << tr("Device programmed!");
            break;
            default: infoLog() << tr("Unknown command"); break;
        }
        executeType = NRF_EXECUTE_UNKNOWN;

    }
    catch(FlashingException& e) {
        errorLog() << e.what();
    }
}

bool FlashingThread::executeOpenOCD(QProcess* proc, const QStringList &args) throw(FlashingException)
{
    proc->setWorkingDirectory(qApp->applicationDirPath() + "/nRF52-OpenOCD/");
    proc->start(qApp->applicationDirPath() + "/nRF52-OpenOCD/openocd", args);

    if (!proc->waitForStarted() || !proc->waitForFinished())
    {
        proc->terminate();
        throw FlashingException(tr("OpenOCD %1 is too long run: %2").arg(args.join(' ')).arg(proc->errorString()));
    }

    if (proc->exitCode() != 0)
        throw FlashingException(proc->readAllStandardError());

    return proc->exitCode() == 0;
}


void FlashingThread::readOutput()
{
    auto proc = static_cast<QProcess*>(sender());
    QByteArray buffer;
    QList<QByteArray> lines;
    while (proc && proc->bytesAvailable())
    {
        buffer = proc->readAll();
        buffer.replace('\r', nullptr);
        lines = buffer.split('\n');
        for (const QByteArray& line: lines)
        {
            qDebug() << line.constData();
        }
    }
}

void FlashingThread::errorOccurred(QProcess::ProcessError error)
{
    auto proc = static_cast<QProcess*>(sender());
    qCritical() << error << proc->errorString();
 }


void FlashingThread::nrf_run()
{
    executeType = NRF_EXECUTE_RUN;
}

void FlashingThread::nrf_halt()
{
    executeType = NRF_EXECUTE_HALT;
}

void FlashingThread::nrf_reset()
{
    executeType = NRF_EXECUTE_RESET;
}

void FlashingThread::nrf_erase()
{
    executeType = NRF_EXECUTE_ERASE;
}

void FlashingThread::nrf_flash()
{
    executeType = NRF_EXECUTE_FLASH;
}
