//23100011009
//Şerife Nazlı Ünay
#ifndef KITAP_ISLEMLERI_H
#define KITAP_ISLEMLERI_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include<QMessageBox>
#include <QSqlError>

namespace Ui {
class kitap_islemleri;
}

class kitap_islemleri : public QDialog
{
    Q_OBJECT

public:
    explicit kitap_islemleri(QSqlDatabase db, QWidget *parent = nullptr);
    ~kitap_islemleri();

private slots:
    void on_ln_search_textChanged(const QString &text);
    void on_btn_yeni_kayit_clicked();
    void on_btn_guncelle_clicked();
    void on_btn_sil_clicked();
    void on_tableView_kitap_clicked(const QModelIndex &index);

private:
    Ui::kitap_islemleri *ui;
    QSqlQuery *sorgu;
    QSqlQueryModel *model;
    QSqlQueryModel *model_odunc_aktif;
    QSqlQueryModel *model_odunc_gecmis;

    void listele();
    void temizle();
};

#endif // KITAP_ISLEMLERI_H

