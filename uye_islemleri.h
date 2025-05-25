//23100011009
//Şerife Nazlı Ünay
#ifndef UYE_ISLEMLERI_H
#define UYE_ISLEMLERI_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include<QMessageBox>
#include <QSqlError>
#include <QModelIndex>

namespace Ui {
class uye_islemleri;
}

class uye_islemleri : public QDialog
{
    Q_OBJECT

public:
    explicit uye_islemleri(QSqlDatabase,QWidget *parent = nullptr);
    ~uye_islemleri();
    void listele();

private slots:
    void on_btn_yeni_kayit_clicked();

    void on_btn_guncelle_clicked();

    void on_btn_sil_clicked();

    void on_tableView_uye_clicked(const QModelIndex &index);

    void temizle();

    void on_ln_search_textChanged(const QString &text);

private:
    Ui::uye_islemleri *ui;
    QSqlQuery *sorgu;
    QSqlQueryModel *model;

};

#endif // UYE_ISLEMLERI_H
