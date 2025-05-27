//Şerife Nazlı Ünay
#include "odunc_alma_islemleri.h"
#include "ui_odunc_alma_islemleri.h"

odunc_alma_islemleri::odunc_alma_islemleri(QSqlDatabase db, QWidget *parent)
    : QDialog(parent), ui(new Ui::odunc_alma_islemleri)
{
    ui->setupUi(this);
    ui->label->setStyleSheet("color: red");

    sorgu = new QSqlQuery(db);
    listele();  // Başlangıçta tüm üyeler, kitaplar ve ödünçler listelenir
}

odunc_alma_islemleri::~odunc_alma_islemleri()
{
    delete ui;
}

// Üye arama alanı değiştiğinde filtreleme yapılır
void odunc_alma_islemleri::on_ln_search_uyeler_textChanged(const QString &text)
{
    QString arama = text.trimmed();

    if (arama.isEmpty()) {
        sorgu->prepare("SELECT * FROM uye");  // Tüm üyeler
    } else {
        sorgu->prepare("SELECT * FROM uye WHERE uye_ad LIKE ? OR uye_soyad LIKE ?");
        QString pattern = "%" + arama + "%";
        sorgu->addBindValue(pattern);
        sorgu->addBindValue(pattern);
    }

    if (!sorgu->exec()) {
        QMessageBox::critical(this, "Hata", "Üye araması sırasında hata:\n" + sorgu->lastError().text());
        return;
    }

    model->setQuery(*sorgu);
    ui->tableView_tum_uyeler->setModel(model);
    ui->tableView_tum_uyeler->clearSelection();
}

// Kitap arama alanı değiştiğinde filtreleme yapılır
void odunc_alma_islemleri::on_ln_search_kitaplar_textChanged(const QString &text)
{
    QString arama = text.trimmed();

    if (arama.isEmpty()) {
        sorgu->prepare("SELECT * FROM kitap");
    } else {
        sorgu->prepare("SELECT * FROM kitap WHERE kitap_ad LIKE ?");
        QString pattern = "%" + arama + "%";
        sorgu->addBindValue(pattern);
    }

    if (!sorgu->exec()) {
        QMessageBox::critical(this, "Hata", "Kitap araması sırasında hata:\n" + sorgu->lastError().text());
        return;
    }

    model1->setQuery(*sorgu);
    ui->tableView_tum_kitaplar->setModel(model1);
    ui->tableView_tum_kitaplar->clearSelection();
}

// Sayfa ilk açıldığında tüm üyeleri, kitapları ve ödünç kayıtlarını listeler
void odunc_alma_islemleri::listele()
{
    // Üye listesi
    sorgu->prepare("SELECT * FROM uye");
    if (!sorgu->exec()) {
        QMessageBox::critical(this, "Hata", "Üye tablosu çekilemedi.");
        return;
    }
    model = new QSqlQueryModel(this);
    model->setQuery(*sorgu);
    ui->tableView_tum_uyeler->setModel(model);

    // Kitap listesi
    sorgu->prepare("SELECT * FROM kitap");
    if (!sorgu->exec()) {
        QMessageBox::critical(this, "Hata", "Kitap tablosu çekilemedi.");
        return;
    }
    model1 = new QSqlQueryModel(this);
    model1->setQuery(*sorgu);
    ui->tableView_tum_kitaplar->setModel(model1);

    // Ödünç alınan kitaplar
    sorgu->prepare("SELECT * FROM odunc_alinan");
    if (!sorgu->exec()) {
        QMessageBox::critical(this, "Hata", "Ödünç kayıtları çekilemedi.");
        return;
    }
    model2 = new QSqlQueryModel(this);
    model2->setQuery(*sorgu);
    ui->tableView_odunc_alinan_kitaplar->setModel(model2);
}

// Ödünç verme işlemi
void odunc_alma_islemleri::on_btn_odunc_al_clicked()
{
    QString kitapNo = ui->ln_kitap_no->text().trimmed();
    QString uyeNo = ui->ln_uye_no->text().trimmed();
    QString oduncTarihi = ui->dateEdit_odunc_alma_tarihi->date().toString(Qt::ISODate);

    if (kitapNo.isEmpty() || uyeNo.isEmpty()) {
        QMessageBox::warning(this, "Eksik Bilgi", "Kitap ve üye numarası gerekli.");
        return;
    }

    // 1. Kitabın toplam mevcut adedi
    sorgu->prepare("SELECT kitap_sayisi FROM kitap WHERE kitap_no = ?");
    sorgu->addBindValue(kitapNo);

    if (!sorgu->exec() || !sorgu->next()) {
        QMessageBox::critical(this, "Hata", "Kitap bilgisi alınamadı:\n" + sorgu->lastError().text());
        return;
    }
    int kitapSayisi = sorgu->value(0).toInt();

    // 2. Bu kitabın kaç tanesi ödünçte?
    sorgu->prepare("SELECT COUNT(*) FROM odunc_alinan WHERE kitap_no = ?");
    sorgu->addBindValue(kitapNo);

    if (!sorgu->exec() || !sorgu->next()) {
        QMessageBox::critical(this, "Hata", "Stok durumu alınamadı:\n" + sorgu->lastError().text());
        return;
    }
    int oduncVerilen = sorgu->value(0).toInt();

    if (oduncVerilen >= kitapSayisi) {
        QMessageBox::warning(this, "Stok Yok", "Bu kitabın tüm kopyaları ödünç verilmiş.");
        return;
    }

    // 3. Aynı üye aynı kitabı hâlâ iade etmediyse tekrar alamaz
    sorgu->prepare("SELECT COUNT(*) FROM odunc_alinan WHERE uye_no = ? AND kitap_no = ?");
    sorgu->addBindValue(uyeNo);
    sorgu->addBindValue(kitapNo);

    if (!sorgu->exec() || !sorgu->next()) {
        QMessageBox::critical(this, "Hata", "Üye kontrolü başarısız:\n" + sorgu->lastError().text());
        return;
    }

    int mevcutOdunc = sorgu->value(0).toInt();
    if (mevcutOdunc > 0) {
        QMessageBox::warning(this, "Engel", "Bu üye zaten bu kitabı ödünç almış.");
        return;
    }

    // 4. Ödünç kaydını oluştur
    sorgu->prepare("INSERT INTO odunc_alinan (uye_no, kitap_no, odunc_alma_tarihi) VALUES (?, ?, ?)");
    sorgu->addBindValue(uyeNo);
    sorgu->addBindValue(kitapNo);
    sorgu->addBindValue(oduncTarihi);

    if (!sorgu->exec()) {
        QMessageBox::critical(this, "Hata", "Ödünç işlemi başarısız:\n" + sorgu->lastError().text());
        return;
    }

    QMessageBox::information(this, "Başarılı", "Kitap ödünç verildi.");
    listele(); // Tabloları güncelle
}

// Tabloya tıklanınca üye numarasını al
void odunc_alma_islemleri::on_tableView_tum_uyeler_clicked(const QModelIndex &index)
{
    ui->ln_uye_no->setText(model->index(index.row(), 0).data().toString());
}

// Tabloya tıklanınca kitap numarasını al
void odunc_alma_islemleri::on_tableView_tum_kitaplar_clicked(const QModelIndex &index)
{
    ui->ln_kitap_no->setText(model1->index(index.row(), 0).data().toString());
}


