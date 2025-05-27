//Şerife Nazlı Ünay

#include "uye_islemleri.h"
#include "ui_uye_islemleri.h"

uye_islemleri::uye_islemleri(QSqlDatabase db, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::uye_islemleri)
{
    ui->setupUi(this);
    sorgu = new QSqlQuery(db);             // Veritabanı sorguları için nesne
    model = new QSqlQueryModel(this);      // Tablo görüntüleme için model

    ui->label->setStyleSheet("color: red"); // Başlık rengini kırmızı yap
    listele();                              // Form açılırken üye listesini getir
}

uye_islemleri::~uye_islemleri()
{
    delete sorgu;
    delete ui;
}

// Arama kutusuna yazıldıkça filtreleme yapılır
void uye_islemleri::on_ln_search_textChanged(const QString &text)
{
    QString pattern = "%" + text.trimmed() + "%";

    // Eğer arama kutusu boşsa tüm üyeleri listele
    if (pattern == "%%") {
        listele(); return;
    }

    // Ad veya soyad içinde geçen kayıtları arar
    sorgu->prepare("SELECT * FROM uye WHERE uye_ad LIKE ? OR uye_soyad LIKE ?");
    sorgu->addBindValue(pattern);
    sorgu->addBindValue(pattern);

    if (!sorgu->exec()) {
        QMessageBox::critical(this, "Hata", "Üye ararken hata oluştu:\n" + sorgu->lastError().text());
        return;
    }

    model->setQuery(*sorgu);
    ui->tableView_uye->setModel(model);
    ui->tableView_uye->clearSelection();
}

// Veritabanından üyeleri çeker ve yeni üye numarasını hesaplar
void uye_islemleri::listele()
{
    // Son üye numarasını bulup +1 ekleyerek yeni numara önerisi yapılır
    QSqlQuery maxQuery(*sorgu);
    maxQuery.prepare("SELECT MAX(uye_no) FROM uye");

    if (maxQuery.exec() && maxQuery.next())
        ui->ln_uye_no->setText(QString::number(maxQuery.value(0).toInt() + 1));
    else
        ui->ln_uye_no->setText("1");

    // Tüm üyeleri listele
    sorgu->prepare("SELECT * FROM uye");
    if (!sorgu->exec()) {
        QMessageBox::critical(this, "Hata", "Üye listesi alınamadı.");
        return;
    }

    model->setQuery(*sorgu);
    ui->tableView_uye->setModel(model);
    ui->tableView_uye->clearSelection();
}

// Alanları temizler ve tabloyu yeniler
void uye_islemleri::temizle()
{
    ui->ln_ad->clear();
    ui->ln_soyad->clear();
    listele();
}

// Yeni üye kaydı oluştur
void uye_islemleri::on_btn_yeni_kayit_clicked()
{
    QString ad = ui->ln_ad->text().trimmed();
    QString soyad = ui->ln_soyad->text().trimmed();

    // Boş alan kontrolü
    if (ad.isEmpty() || soyad.isEmpty()) {
        QMessageBox::warning(this, "Eksik Bilgi", "Lütfen tüm alanları doldurun.");
        return;
    }

    // Yeni kayıt sorgusu
    sorgu->prepare("INSERT INTO uye(uye_ad, uye_soyad) VALUES (?, ?)");
    sorgu->addBindValue(ad);
    sorgu->addBindValue(soyad);

    if (!sorgu->exec()) {
        QMessageBox::critical(this, "Hata", "Üye eklenemedi:\n" + sorgu->lastError().text());
        return;
    }

    QMessageBox::information(this, "Başarılı", "Üye başarıyla eklendi.");
    temizle();
}

// Seçili üyeyi güncelle
void uye_islemleri::on_btn_guncelle_clicked()
{
    QModelIndex index = ui->tableView_uye->currentIndex();

    // Seçim yapılmamışsa uyarı ver
    if (!index.isValid()) {
        QMessageBox::warning(this, "Uyarı", "Güncellenecek bir üye seçin.");
        return;
    }

    // Alanlar boşsa uyarı ver
    if (ui->ln_ad->text().isEmpty() || ui->ln_soyad->text().isEmpty()) {
        QMessageBox::warning(this, "Eksik Bilgi", "Tüm alanları doldurun.");
        return;
    }

    // Seçilen satırdan üye numarasını al
    int uye_no = model->data(model->index(index.row(), 0)).toInt();

    // Güncelleme sorgusu
    sorgu->prepare("UPDATE uye SET uye_ad=?, uye_soyad=? WHERE uye_no=?");
    sorgu->addBindValue(ui->ln_ad->text());
    sorgu->addBindValue(ui->ln_soyad->text());
    sorgu->addBindValue(uye_no);

    if (!sorgu->exec()) {
        QMessageBox::critical(this, "Hata", "Güncelleme başarısız:\n" + sorgu->lastError().text());
        return;
    }

    QMessageBox::information(this, "Başarılı", "Üye bilgisi güncellendi.");
    temizle();
}

// Üyeyi sil: ama önce ödünç kitap kontrolü yap
void uye_islemleri::on_btn_sil_clicked()
{
    QString uye_no = ui->ln_uye_no->text().trimmed();

    // Üyenin ödünç kitabı olup olmadığı kontrol edilir
    sorgu->prepare("SELECT * FROM odunc_alinan WHERE uye_no = ?");
    sorgu->addBindValue(uye_no);

    if (!sorgu->exec()) {
        QMessageBox::critical(this, "Hata", "Kontrol hatası:\n" + sorgu->lastError().text());
        return;
    }

    // Kayıt varsa silinemez
    if (sorgu->next()) {
        QMessageBox::warning(this, "Silme Engeli", "Üye silinemez. Teslim edilmemiş kitap mevcut.");
        return;
    }

    // Silme işlemi
    sorgu->prepare("DELETE FROM uye WHERE uye_no = ?");
    sorgu->addBindValue(uye_no);

    if (!sorgu->exec()) {
        QMessageBox::critical(this, "Hata", "Silme hatası:\n" + sorgu->lastError().text());
        return;
    }

    QMessageBox::information(this, "Başarılı", "Üye silindi.");
    temizle();
}

// Tabloda satıra tıklandığında bilgileri forma aktar
void uye_islemleri::on_tableView_uye_clicked(const QModelIndex &index)
{
    ui->ln_uye_no->setText(model->index(index.row(), 0).data().toString());
    ui->ln_ad->setText(model->index(index.row(), 1).data().toString());
    ui->ln_soyad->setText(model->index(index.row(), 2).data().toString());
}

