//Şerife Nazlı Ünay
#ifndef ODUNC_TESLIM_ETME_ISLEMLERI_H
#define ODUNC_TESLIM_ETME_ISLEMLERI_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include<QMessageBox>
#include <QSqlError>


namespace Ui {
class odunc_teslim_etme_islemleri;
}

class odunc_teslim_etme_islemleri : public QDialog
{
    Q_OBJECT

public:
    explicit odunc_teslim_etme_islemleri(QSqlDatabase,QWidget *parent = nullptr);
    ~odunc_teslim_etme_islemleri();
    void listele();

private slots:
    void on_pushButton_clicked();

    void on_tableViewim_odunc_alinan_clicked(const QModelIndex &index);


private:
    Ui::odunc_teslim_etme_islemleri *ui;
    QSqlQuery *sorgu;
    QSqlQueryModel *model;
    QSqlQueryModel *model1;

    QString seciliAlmaTarihi;
};

#endif // ODUNC_TESLIM_ETME_ISLEMLERI_H
