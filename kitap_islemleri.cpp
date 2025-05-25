//23100011009
//Şerife Nazlı Ünay
#include "kitap_islemleri.h"
#include "ui_kitap_islemleri.h"

kitap_islemleri::kitap_islemleri(QSqlDatabase db, QWidget *parent)
    : QDialog(parent), ui(new Ui::kitap_islemleri)
{
    ui->setupUi(this);
    ui->label->setStyleSheet("color: red"); // Başlık kırmızı

    sorgu = new QSqlQuery(db);              // Veritabanı sorguları için nesne
    model = new QSqlQueryModel(this);       // Tablo görüntüleme modeli

    listele();                              // Form açıldığında kitapları listele
}

kitap_islemleri::~kitap_islemleri()
{
    delete ui;
}

// Arama kutusu değiştikçe filtreleme yapılır
void kitap_islemleri::on_ln_search_textChanged(const QString &text)
{
    QString pattern = "%" + text.trimmed() + "%";

    if (text.isEmpty()) {
        listele(); return;
    }

    // Kitap adı içinde geçenleri listele
    sorgu->prepare("SELECT * FROM kitap WHERE kitap_ad LIKE ?");
    sorgu->addBindValue(pattern);

    if (!sorgu->exec()) {
        QMessageBox::critical(this, "Hata", "Kitap arama hatası:\n" + sorgu->lastError().text());
        return;
    }

    model->setQuery(*sorgu);
    ui->tableView_kitap->setModel(model);
    ui->tableView_kitap->clearSelection();
}

// Kitapları listele ve yeni kitap numarasını hazırla
void kitap_islemleri::listele()
{
    // En son kitap_no'yu bulup bir fazlasını öner
    QSqlQuery maxQuery(*sorgu);
    maxQuery.prepare("SELECT MAX(kitap_no) FROM kitap");

    if (maxQuery.exec() && maxQuery.next()) {
        int nextNo = maxQuery.value(0).toInt() + 1;
        ui->ln_kitap_no->setText(QString::number(nextNo));
    } else {
        ui->ln_kitap_no->setText("1");
    }

    // Tüm kitapları listele
    sorgu->prepare("SELECT * FROM kitap");
    if (!sorgu->exec()) {
        QMessageBox::critical(this, "Hata", "Kitapları listelerken hata oluştu.");
        return;
    }

    model->setQuery(*sorgu);
    ui->tableView_kitap->setModel(model);
    ui->tableView_kitap->clearSelection();
}

// Alanları temizle ve tabloyu yenile
void kitap_islemleri::temizle()
{
    ui->ln_kitap_ad->clear();
    ui->ln_stok->clear();
    listele();
}

// Yeni kitap kaydı ekle
void kitap_islemleri::on_btn_yeni_kayit_clicked()
{
    QString kitapAd = ui->ln_kitap_ad->text().trimmed();
    QString kitapStok = ui->ln_stok->text().trimmed();

    // Boş alan kontrolü
    if (kitapAd.isEmpty() || kitapStok.isEmpty()) {
        QMessageBox::warning(this, "Eksik Bilgi", "Kitap adı ve stok boş olamaz.");
        return;
    }

    // Stok sayısı geçerli mi?
    bool ok;
    int stok = kitapStok.toInt(&ok);
    if (!ok || stok < 0) {
        QMessageBox::warning(this, "Hatalı Giriş", "Stok sayısı pozitif bir sayı olmalıdır.");
        return;
    }

    // Veritabanına ekle
    sorgu->prepare("INSERT INTO kitap(kitap_ad, kitap_sayisi) VALUES(?, ?)");
    sorgu->addBindValue(kitapAd);
    sorgu->addBindValue(stok);

    if (!sorgu->exec()) {
        QMessageBox::critical(this, "Hata", "Kitap eklenirken hata:\n" + sorgu->lastError().text());
        return;
    }

    QMessageBox::information(this, "Başarılı", "Kitap başarıyla eklendi.");
    temizle();
}

// Seçilen kitabı güncelle
void kitap_islemleri::on_btn_guncelle_clicked()
{
    QModelIndex index = ui->tableView_kitap->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "Uyarı", "Güncellenecek kitap seçilmedi.");
        return;
    }

    // Giriş alanları kontrolü
    if (ui->ln_kitap_ad->text().isEmpty() || ui->ln_stok->text().isEmpty()) {
        QMessageBox::warning(this, "Eksik Bilgi", "Tüm alanları doldurun.");
        return;
    }

    bool ok;
    int stok = ui->ln_stok->text().toInt(&ok);
    if (!ok || stok < 0) {
        QMessageBox::warning(this, "Geçersiz", "Stok sayısı pozitif olmalı.");
        return;
    }

    int kitap_no = model->data(model->index(index.row(), 0)).toInt();

    sorgu->prepare("UPDATE kitap SET kitap_ad=?, kitap_sayisi=? WHERE kitap_no=?");
    sorgu->addBindValue(ui->ln_kitap_ad->text());
    sorgu->addBindValue(ui->ln_stok->text());
    sorgu->addBindValue(kitap_no);

    if (!sorgu->exec()) {
        QMessageBox::critical(this, "Hata", "Güncelleme başarısız:\n" + sorgu->lastError().text());
        return;
    }

    QMessageBox::information(this, "Başarılı", "Kitap bilgisi güncellendi.");
    temizle();
}

// Kitabı sil, önce ödünçte mi kontrol et
void kitap_islemleri::on_btn_sil_clicked()
{
    QString kitapNo = ui->ln_kitap_no->text().trimmed();

    // Ödünç verilmiş mi?
    sorgu->prepare("SELECT COUNT(*) FROM odunc_alinan WHERE kitap_no = ?");
    sorgu->addBindValue(kitapNo);

    if (!sorgu->exec()) {
        QMessageBox::critical(this, "Hata", "Ödünç kontrol hatası:\n" + sorgu->lastError().text());
        return;
    }

    int syc = 0;
    if (sorgu->next())
        syc = sorgu->value(0).toInt();

    if (syc > 0) {
        QMessageBox::warning(this, "Silme Reddedildi", "Bu kitap hala ödünçte.");
        return;
    }

    // Silme işlemi
    sorgu->prepare("DELETE FROM kitap WHERE kitap_no = ?");
    sorgu->addBindValue(kitapNo);

    if (!sorgu->exec()) {
        QMessageBox::critical(this, "Hata", "Kitap silinemedi:\n" + sorgu->lastError().text());
        return;
    }

    QMessageBox::information(this, "Başarılı", "Kitap silindi.");
    temizle();
}

// Tabloda satıra tıklandığında form alanlarını doldurur
void kitap_islemleri::on_tableView_kitap_clicked(const QModelIndex &index)
{
    QString kitapNo = model->index(index.row(), 0).data().toString();
    ui->ln_kitap_no->setText(kitapNo);
    ui->ln_kitap_ad->setText(model->index(index.row(), 1).data().toString());
    ui->ln_stok->setText(model->index(index.row(), 2).data().toString());

    // Aktif ödünç kayıtları
    QSqlQuery* aktifSorgu = new QSqlQuery();
    aktifSorgu->prepare("SELECT * FROM odunc_alinan WHERE kitap_no = ?");
    aktifSorgu->addBindValue(kitapNo);

    if (!aktifSorgu->exec()) {
        QMessageBox::critical(this, "Hata", "Aktif ödünç bilgisi alınamadı:\n" + aktifSorgu->lastError().text());
        return;
    }

    model_odunc_aktif = new QSqlQueryModel(this);
    model_odunc_aktif->setQuery(*aktifSorgu);
    ui->tableView_odunc_alinan->setModel(model_odunc_aktif);

    // Geçmiş ödünç kayıtları
    QSqlQuery* gecmisSorgu = new QSqlQuery();
    gecmisSorgu->prepare("SELECT * FROM odunc_teslim_edilen WHERE kitap_no = ?");
    gecmisSorgu->addBindValue(kitapNo);

    if (!gecmisSorgu->exec()) {
        QMessageBox::critical(this, "Hata", "Geçmiş kayıtlar alınamadı:\n" + gecmisSorgu->lastError().text());
        return;
    }

    model_odunc_gecmis = new QSqlQueryModel(this);
    model_odunc_gecmis->setQuery(*gecmisSorgu);
    ui->tableView_daha_onceden_odunc->setModel(model_odunc_gecmis);
}

