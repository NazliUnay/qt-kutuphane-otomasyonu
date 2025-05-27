//Şerife Nazlı Ünay
#ifndef ODUNC_ALMA_ISLEMLERI_H
#define ODUNC_ALMA_ISLEMLERI_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include<QMessageBox>
#include <QSqlError>

namespace Ui {
class odunc_alma_islemleri;
}

class odunc_alma_islemleri : public QDialog
{
    Q_OBJECT

public:
    explicit odunc_alma_islemleri(QSqlDatabase,QWidget *parent = nullptr);
    ~odunc_alma_islemleri();
    void listele();

private slots:
    void on_btn_odunc_al_clicked();

    void on_tableView_tum_uyeler_clicked(const QModelIndex &index);

    void on_tableView_tum_kitaplar_clicked(const QModelIndex &index);

    void on_ln_search_uyeler_textChanged(const QString &text);

    void on_ln_search_kitaplar_textChanged(const QString &text);


private:
    Ui::odunc_alma_islemleri *ui;
    QSqlQuery *sorgu;
    QSqlQueryModel *model;
    QSqlQueryModel *model1;
    QSqlQueryModel *model2;
};

#endif // ODUNC_ALMA_ISLEMLERI_H
