#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QTextEdit>
#include <QProcess>

#include <iostream>

QTextEdit *textlog = nullptr;

#define MESSAGE_PATTERN "<span style=\"color: #" \
    "%{if-debug}808080%{endif}%{if-info}003153%{endif}%{if-warning}ffb300%{endif}%{if-critical}f80000%{endif}%{if-fatal}8f000a%{endif}" \
    "\">[%{time hh:mm:ss}][%{type}] %{message}</span>"

void logHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &str)
{
    QMetaObject::invokeMethod(textlog, "append", Qt::QueuedConnection, Q_ARG(QString, str));
    //if (obj) obj->addToLog( qFormatLogMessage(type, ctx, str) );
    qt_message_output(type, ctx, str);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    //this->setWindowTitle("nRF52-Flasher");

    // Init log
    textlog = ui->log;
    // connect(this, SIGNAL(addToLog(QString)), ui->log, SLOT(append(QString)), Qt::QueuedConnection);
    qInstallMessageHandler(logHandler);
    if (!qEnvironmentVariableIsSet("QT_MESSAGE_PATTERN"))
        qSetMessagePattern(MESSAGE_PATTERN);
    qInfo() << "=======================";
    qInfo() << "'nRF52 FLASHER' v" + QString(FLASHER_VERSION);
    qInfo() << "---------------------------------------------";
    qInfo() << "Vega-Absolute, 2018";
    qInfo() << "=======================\r\n";

    // Init flasher thread
    flasher.reset(new FlashingThread);

    // Load last params
    QSettings s;
    lastPathSD   = s.value("LastPathSD",   QDir::homePath()).toString();
    lastPathAPP  = s.value("LastPathAPP",  QDir::homePath()).toString();
    lastPathBOOT = s.value("LastPathBOOT", QDir::homePath()).toString();
    lastFileSD   = s.value("LastFileSD").toString();
    lastFileAPP  = s.value("LastFileAPP").toString();
    lastFileBOOT = s.value("LastFileBOOT").toString();
    ui->File_SD->setText(lastFileSD);
    ui->File_APP->setText(lastFileAPP);
    ui->File_BOOT->setText(lastFileBOOT);
    check_files();
    check_file_SD();
    check_file_APP();
    check_file_BOOT();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::check_files() {
    if(ui->File_SD->text().length() && ui->File_APP->text().length() && ui->File_BOOT->text().length()) {
        if(ui->File_SD->text().endsWith(".hex") && ui->File_APP->text().endsWith(".hex") && ui->File_BOOT->text().endsWith(".hex")) {
            ui->Btn_flash->setEnabled(true);
            ui->Btn_flashErase->setEnabled(true);
            return true;
        }
    }
    ui->Btn_flash->setEnabled(false);
    ui->Btn_flashErase->setEnabled(false);
    infoLog() << "Error in one or more file paths";
    return false;
}

bool MainWindow::check_file_SD() {
    if(ui->File_SD->text().length() && ui->File_SD->text().endsWith(".hex")) {
        ui->Btn_flash_SD->setEnabled(true);
        return true;
    }
    ui->Btn_flash_SD->setEnabled(false);
    return false;
}

bool MainWindow::check_file_APP() {
    if(ui->File_APP->text().length() && ui->File_APP->text().endsWith(".hex")) {
        ui->Btn_flash_APP->setEnabled(true);
        return true;
    }
    ui->Btn_flash_APP->setEnabled(false);
    return false;
}

bool MainWindow::check_file_BOOT() {
    if(ui->File_BOOT->text().length() && ui->File_BOOT->text().endsWith(".hex")) {
        ui->Btn_flash_BOOT->setEnabled(true);
        return true;
    }
    ui->Btn_flash_BOOT->setEnabled(false);
    return false;
}

void MainWindow::on_Btn_SD_clicked()
{
    lastFileSD = QFileDialog::getOpenFileName(this,
                    tr("Open SoftDevice Firmware"), lastPathSD, tr("Firmware Files (*.hex)"));
    if (!lastFileSD.isNull())
    {
        QFileInfo fi(lastFileSD);
        lastPathSD = fi.absolutePath();
        QSettings s;
        s.setValue("LastPathSD", lastPathSD);
        s.setValue("LastFileSD", lastFileSD);
        ui->File_SD->setText(lastFileSD);
        check_files();
        infoLog() << ("SoftDevice firmware: ") << QString(lastFileSD);
    }

}

void MainWindow::on_Btn_APP_clicked()
{
    lastFileAPP = QFileDialog::getOpenFileName(this,
                    tr("Open Application Firmware"), lastPathAPP, tr("Firmware Files (*.hex)"));
    if (!lastFileAPP.isNull())
    {
        QFileInfo fi(lastFileAPP);
        lastPathAPP = fi.absolutePath();
        QSettings s;
        s.setValue("LastPathAPP", lastPathAPP);
        s.setValue("LastFileAPP", lastFileAPP);
        ui->File_APP->setText(lastFileAPP);
        check_files();
        infoLog() << ("Application firmware: ") << QString(lastFileAPP);
    }
}

void MainWindow::on_Btn_BOOT_clicked()
{
    lastFileBOOT = QFileDialog::getOpenFileName(this,
                    tr("Open Bootloader Firmware"), lastPathBOOT, tr("Firmware Files (*.hex)"));
    if (!lastFileBOOT.isNull())
    {
        QFileInfo fi(lastFileBOOT);
        lastPathBOOT = fi.absolutePath();
        QSettings s;
        s.setValue("LastPathBOOT", lastPathBOOT);
        s.setValue("LastFileBOOT", lastFileBOOT);
        ui->File_BOOT->setText(lastFileBOOT);
        check_files();
        infoLog() << ("Bootloader firmware: ") << QString(lastFileBOOT);
    }
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
    if(!check_files()) return;
    flasher->setPath(lastFileSD, lastFileAPP, lastFileBOOT);
    flasher->nrf_flash_all();
    flasher->start();
}

void MainWindow::on_Btn_flash_SD_clicked()
{
    if(!check_file_SD()) return;
    flasher->setPath(lastFileSD, lastFileAPP, lastFileBOOT);
    flasher->nrf_flash_sd();
    flasher->start();
}


void MainWindow::on_Btn_flash_APP_clicked()
{
    if(!check_file_APP()) return;
    flasher->setPath(lastFileSD, lastFileAPP, lastFileBOOT);
    flasher->nrf_flash_app();
    flasher->start();
}


void MainWindow::on_Btn_flash_BOOT_clicked()
{
    if(!check_file_BOOT()) return;
    flasher->setPath(lastFileSD, lastFileAPP, lastFileBOOT);
    flasher->nrf_flash_boot();
    flasher->start();
}

void MainWindow::on_Btn_flashErase_clicked()
{
    if(!check_files()) return;
    flasher->setPath(lastFileSD, lastFileAPP, lastFileBOOT);
    flasher->nrf_flashErase();
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


void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings s;
    s.setValue("LastPathSD",   lastPathSD);
    s.setValue("LastPathAPP",  lastPathAPP);
    s.setValue("LastPathBOOT", lastPathBOOT);
    s.setValue("LastFileSD",   lastFileSD);
    s.setValue("LastFileAPP",  lastFileAPP);
    s.setValue("LastFileBOOT", lastFileBOOT);

    QMainWindow::closeEvent(event);
}
