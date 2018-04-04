#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QTemporaryFile>

#include <memory>

#include "flashingthread.h"

#define FLASHER_VERSION   "1.0"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_ENUMS(Type)

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:

    void on_Btn_SD_clicked();
    void on_Btn_APP_clicked();
    void on_Btn_BOOT_clicked();

    void on_btn_reset_clicked();
    void on_btn_halt_clicked();
    void on_btn_run_clicked();
    void on_Btn_send_clicked();
    void on_Btn_flash_clicked();
    void on_Btn_erase_clicked();

    void on_Btn_flashErase_clicked();

private:

    Ui::MainWindow *ui;

    std::unique_ptr<FlashingThread> flasher;

    QString lastPathSD, lastPathAPP, lastPathBOOT;
    QString lastFileSD, lastFileAPP, lastFileBOOT;

    void closeEvent(QCloseEvent *event) override;

    bool check_files();
};

#endif // MAINWINDOW_H
