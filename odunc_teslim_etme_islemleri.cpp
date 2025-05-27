//Şerife Nazlı Ünay
#include "odunc_teslim_etme_islemleri.h"
#include "ui_odunc_teslim_etme_islemleri.h"


odunc_teslim_etme_islemleri::odunc_teslim_etme_islemleri(QSqlDatabase db, QWidget *parent)
    : QDialog(parent), ui(new Ui::odunc_teslim_etme_islemleri)
{
    ui->setupUi(this);
    ui->label->setStyleSheet("color: red");

    sorgu = new QSqlQuery(db);  // Veritabanı sorgu nesnesi
    listele();                  // Tabloları yükle
}

odunc_teslim_etme_islemleri::~odunc_teslim_etme_islemleri()
{
    delete ui;
}

// Tablo verilerini getir: hem aktif hem teslim edilmiş kitaplar
void odunc_teslim_etme_islemleri::listele()
{
    // Aktif ödünçler
    sorgu->prepare("SELECT * FROM odunc_alinan");
    if (!sorgu->exec()) {
        QMessageBox::critical(this, "Hata", "Aktif ödünçler listelenemedi:\n" + sorgu->lastError().text());
        return;
    }
    model = new QSqlQueryModel(this);
    model->setQuery(*sorgu);
    ui->tableViewim_odunc_alinan->setModel(model);

    // Daha önce teslim edilenler
    sorgu->prepare("SELECT * FROM odunc_teslim_edilen");
    if (!sorgu->exec()) {
        QMessageBox::critical(this, "Hata", "Teslim edilmiş kayıtlar listelenemedi:\n" + sorgu->lastError().text());
        return;
    }
    model1 = new QSqlQueryModel(this);
    model1->setQuery(*sorgu);
    ui->tableView_teslimedilen->setModel(model1);
}

// Tablo satırına tıklanınca bilgileri form alanlarına al
void odunc_teslim_etme_islemleri::on_tableViewim_odunc_alinan_clicked(const QModelIndex &index)
{
    ui->ln_uye_no->setText(model->index(index.row(), 0).data().toString());
    ui->ln_kitap_no->setText(model->index(index.row(), 1).data().toString());
    seciliAlmaTarihi = model->index(index.row(), 2).data().toString();  // Daha sonra silmek için
}

// Teslim Et butonuna basıldığında çalışır
void odunc_teslim_etme_islemleri::on_pushButton_clicked()
{
    QString kitapNo = ui->ln_kitap_no->text().trimmed();
    QString uyeNo = ui->ln_uye_no->text().trimmed();
    QString vermeTarihi = ui->dateEdit_odunc_verme_tarihi->date().toString(Qt::ISODate).trimmed();
    QString almaTarihi = seciliAlmaTarihi;

    // Tarih geçerliliği kontrolü
    QDate alma = QDate::fromString(almaTarihi, Qt::ISODate);
    QDate verme = QDate::fromString(vermeTarihi, Qt::ISODate);

    if (!alma.isValid() || !verme.isValid()) {
        QMessageBox::warning(this, "Hata", "Geçerli tarih giriniz.");
        return;
    }

    if (verme < alma) {
        QMessageBox::warning(this, "Hata", "Teslim tarihi ödünç alma tarihinden önce olamaz.");
        return;
    }

    // Borç hesaplama (15 günden fazla ise 4₺/gün)
    int borc = 0;
    int gecikme = alma.daysTo(verme);
    if (gecikme > 15) {
        borc = (gecikme - 15) * 4;
    }

    // Alanlar boş mu kontrolü
    if (kitapNo.isEmpty() || uyeNo.isEmpty()) {
        QMessageBox::warning(this, "Eksik Bilgi", "Tüm alanlar doldurulmalı.");
        return;
    }

    // 1. Teslim kaydı ekle
    sorgu->prepare("INSERT INTO odunc_teslim_edilen(uye_no, kitap_no, alma_tarihi, verme_tarihi, borc) VALUES (?, ?, ?, ?, ?)");
    sorgu->addBindValue(uyeNo);
    sorgu->addBindValue(kitapNo);
    sorgu->addBindValue(almaTarihi);
    sorgu->addBindValue(vermeTarihi);
    sorgu->addBindValue(borc);

    if (!sorgu->exec()) {
        QMessageBox::critical(this, "Hata", "Teslim kaydı eklenemedi:\n" + sorgu->lastError().text());
        return;
    }

    // 2. Eski ödünç kaydını sil
    QSqlQuery silmeQuery(*sorgu);  // Aynı bağlantıyı kullanmak için
    silmeQuery.prepare("DELETE FROM odunc_alinan WHERE uye_no = ? AND kitap_no = ? AND odunc_alma_tarihi = ?");
    silmeQuery.addBindValue(uyeNo);
    silmeQuery.addBindValue(kitapNo);
    silmeQuery.addBindValue(almaTarihi);

    if (!silmeQuery.exec()) {
        QMessageBox::critical(this, "Hata", "Ödünç kaydı silinemedi:\n" + silmeQuery.lastError().text());
        return;
    }

    QMessageBox::information(this, "Teslim Alındı", "İşlem tamamlandı.\nBorç: " + QString::number(borc) + " ₺");
    listele();  // Tabloyu yenile
}
