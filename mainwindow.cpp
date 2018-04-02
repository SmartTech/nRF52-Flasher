#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QProcess>


MainWindow* obj = nullptr;

#define MESSAGE_PATTERN "<span style=\"color: #" \
    "%{if-debug}808080%{endif}%{if-info}003153%{endif}%{if-warning}ffb300%{endif}%{if-critical}f80000%{endif}%{if-fatal}8f000a%{endif}" \
    "\">[%{time hh:mm:ss}][%{type}] %{message}</span>"

void logHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &str)
{
    if (obj) obj->addToLog( qFormatLogMessage(type, ctx, str) );
    qt_message_output(type, ctx, str);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Init log
    connect(this, SIGNAL(addToLog(QString)), ui->log, SLOT(append(QString)), Qt::QueuedConnection);
    qInstallMessageHandler(logHandler);
    if (!qEnvironmentVariableIsSet("QT_MESSAGE_PATTERN"))
        qSetMessagePattern(MESSAGE_PATTERN);
    addToLog("=======================");
    addToLog("'nRF52 FLASHER' v" + QString(FLASHER_VERSION));
    addToLog("---------------------------------------------");
    addToLog("Vega-Absolute, 2018");
    addToLog("=======================\r\n");

    // Init flasher thread
    flasher.reset(new FlashingThread);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::check_files() {
    if(ui->File_SD->text().length() && ui->File_APP->text().length() && ui->File_BOOT->text().length()) {
        ui->Btn_flash->setEnabled(true);
    }
}

void MainWindow::on_Btn_SD_clicked()
{
    auto fileName = QFileDialog::getOpenFileName(this,
                    tr("Open SoftDevice Firmware"), lastPathSD, tr("Firmware Files (*.hex)"));
    if (!fileName.isNull())
    {
        QFileInfo fi(fileName);
        lastPathSD = fi.absolutePath();
        QSettings s;
        s.setValue("LastPathSD", lastPathSD);
        ui->File_SD->setText(fileName);
        check_files();
    }
}

void MainWindow::on_Btn_APP_clicked()
{
    auto fileName = QFileDialog::getOpenFileName(this,
                    tr("Open Application Firmware"), lastPathSD, tr("Firmware Files (*.hex)"));
    if (!fileName.isNull())
    {
        QFileInfo fi(fileName);
        lastPathAPP = fi.absolutePath();
        QSettings s;
        s.setValue("LastPathAPP", lastPathAPP);
        ui->File_APP->setText(fileName);
        check_files();
    }
}

void MainWindow::on_Btn_BOOT_clicked()
{
    auto fileName = QFileDialog::getOpenFileName(this,
                    tr("Open Bootloader Firmware"), lastPathSD, tr("Firmware Files (*.hex)"));
    if (!fileName.isNull())
    {
        QFileInfo fi(fileName);
        lastPathBOOT = fi.absolutePath();
        QSettings s;
        s.setValue("LastPathSD", lastPathBOOT);
        ui->File_BOOT->setText(fileName);
        check_files();
    }
}

void MainWindow::on_Refresh_SD_clicked()
{

}

void MainWindow::on_Refresh_APP_clicked()
{

}

void MainWindow::on_Refresh_BOOT_clicked()
{

}

void MainWindow::on_btn_reset_clicked()
{
    flasher->nrf_reset();
    flasher->start();
}

void MainWindow::on_btn_halt_clicked()
{
    flasher->nrf_halt();
    flasher->start();
}

void MainWindow::on_btn_run_clicked()
{
    flasher->nrf_run();
    flasher->start();
}

void MainWindow::on_Btn_flash_clicked()
{
    flasher->setPath(lastFileSD, lastFileAPP, lastFileBOOT);
    flasher->start();
}

void MainWindow::on_Btn_erase_clicked()
{
    flasher->nrf_erase();
    flasher->start();
}

void MainWindow::on_Btn_send_clicked()
{

}
