//23100011009
//Şerife Nazlı Ünay
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Veritabanı dosya yolu
    db.setDatabaseName("Kütüphane.db");

    // Durum çubuğuna mesaj için etiket
    kaliciMesaj = new QLabel(this);
    ui->statusbar->addWidget(kaliciMesaj);

    // Veritabanı bağlantısı kontrolü
    if (!db.open())
        kaliciMesaj->setText("Veritabanına Bağlanılamadı!");
    else
        kaliciMesaj->setText("Veritabanına Bağlanıldı!");
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool karanlikMod = false;

// Tema uygula
void MainWindow::karanlikModuUygula(bool karanlik) {
    if (karanlik) {
        // Karanlık mod stili
        qApp->setStyleSheet(
            "QMainWindow { background-color: #121212; color: white; }"
            "QPushButton { background-color: #333; color: white; border-radius: 5px; }"
            "QLineEdit, QTableView, QDateEdit { background-color: #1E1E1E; color: white; border: 1px solid #444; }"
            "QLabel { color: white; }"
            "QMessageBox, QDialog { background-color: #121212; color: white; }"
            );
        ui->label->setStyleSheet("color: red");
    } else {
        // Varsayılan tema
        qApp->setStyleSheet("");
    }
}

// Üye işlemleri penceresi aç
void MainWindow::on_btn_uye_islemleri_clicked()
{
    auto *w = new uye_islemleri(db, this);
    w->show();
}

// Kitap işlemleri penceresi aç
void MainWindow::on_btn_kitap_islemleri_clicked()
{
    auto *w = new kitap_islemleri(db, this);
    w->show();
}

// Ödünç alma penceresi aç
void MainWindow::on_btn_odunc_alma_clicked()
{
    auto *w = new odunc_alma_islemleri(db, this);
    w->show();
}

// Teslim etme penceresi aç
void MainWindow::on_btn_odunc_teslim_etme_clicked()
{
    auto *w = new odunc_teslim_etme_islemleri(db, this);
    w->show();
}

// Beyaz tema
void MainWindow::on_actionBeyaz_triggered()
{
    karanlikModuUygula(false);
    ui->statusbar->setStyleSheet("color: black;");
    ui->menubar->setStyleSheet("color: black;");
}

// Siyah tema
void MainWindow::on_actionSiyah_triggered()
{
    karanlikModuUygula(true);
    ui->statusbar->setStyleSheet("color: white;");
    ui->menubar->setStyleSheet("color: white;");
    ui->menuTema->setStyleSheet("color: black;");
}

