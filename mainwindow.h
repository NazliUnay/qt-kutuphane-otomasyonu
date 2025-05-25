//23100011009
//Şerife Nazlı Ünay
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "kitap_islemleri.h"
#include "odunc_alma_islemleri.h"
#include "odunc_teslim_etme_islemleri.h"
#include "uye_islemleri.h"

#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlQueryModel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_btn_uye_islemleri_clicked();

    void on_btn_kitap_islemleri_clicked();

    void on_btn_odunc_alma_clicked();

    void on_btn_odunc_teslim_etme_clicked();

    void on_actionBeyaz_triggered();

    void on_actionSiyah_triggered();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE");
    QSqlTableModel *model1;
    QSqlTableModel *model2;
    QSqlQuery *sorgu;

    QLabel *kaliciMesaj;

    bool karanlikMod;
    void karanlikModuUygula(bool karanlik);

};
#endif // MAINWINDOW_H
