#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QNetworkDatagram>
#include <QDebug>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui = new Ui::MainWindow;    //user interface
    m_socket = new QUdpSocket(this);
    m_timer = new QTimer(this);

    ui->setupUi(this);

    connect(m_socket, &QUdpSocket::readyRead, this, &MainWindow::datenbearbeiten);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::automatischsenden);

    QTimer::singleShot(100, this, &MainWindow::automatischsenden);  //100m

    QString pfad = QCoreApplication::applicationDirPath() + "/fragen.txt";
    ladeFragenAusDatei(pfad);

    //qDebug() << "geladener Fragen:" << fragen.size();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::datenbearbeiten()
{
    while (m_socket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = m_socket->receiveDatagram();    //Datenpacket gespeichert
        QString antwort = QString(datagram.data()).trimmed();
        QString sender = datagram.senderAddress().toString();   //IP-Adresse

        ui->textEdit_2->append("Antwort von " + sender + ": " + antwort);

        if (sender == spieler1_ip || sender == spieler2_ip) {
            datenerhalten(sender, antwort);
        }
    }
}

void MainWindow::automatischsenden()
{
    spieler1_ip = ui->lineEdit->text().trimmed();
    spieler2_ip = ui->lineEdit_2->text().trimmed();

    if (spieler1_ip.isEmpty() || spieler2_ip.isEmpty()) {
        ui->textEdit_2->append("⚠ Bitte IP-Adressen für beide Spieler eingeben!");
        QTimer::singleShot(2000, this, &MainWindow::automatischsenden);
        QTimer::singleShot(2000, [this]() {
            if (ui->textEdit_2->toPlainText().contains("IP-Adressen")) {
                ui->textEdit_2->clear();
            }
        });
        return;
    }

    if (index < fragen.size()) {
        ui->textEdit->setText(fragen[index]);
        //.toUtf8() -> für datenübertragung bei UDP
        m_socket->writeDatagram(ui->textEdit->toPlainText().toUtf8(), QHostAddress(spieler1_ip), 60000);
        m_socket->writeDatagram(ui->textEdit->toPlainText().toUtf8(), QHostAddress(spieler2_ip), 60000);
        index++;
        m_timer->start(10000);
        ui->textEdit_2->clear();
    } else {
        ui->textEdit->append("Quiz beendet!");
        m_timer->stop();

        QString gewinner;
        if (spieler1_punkte > spieler2_punkte) {
            gewinner = "Spieler 1 gewinnt!";
        } else if (spieler2_punkte > spieler1_punkte) {
            gewinner = "Spieler 2 gewinnt!";
        } else {
            gewinner = "Unentschieden!";
        }
        ui->textEdit_2->append("\nErgebnis:\n" + gewinner);
    }
}

void MainWindow::datenerhalten(const QString& sender, const QString& antwort)
{
    if (index > 0 && index <= antworten.size()) {
        QString richtigeAntwort = antworten[index - 1];

        if (antwort.contains(richtigeAntwort, Qt::CaseInsensitive)) {
            if (sender == spieler1_ip) {
                spieler1_punkte++;
                ui->textEdit_2->append("✅ Spieler1 ist richtig!");
            } else if (sender == spieler2_ip) {
                spieler2_punkte++;
                ui->textEdit_2->append("✅ Spieler2 ist richtig!");
            }
            updatePunktestand();
        } else {
            ui->textEdit_2->append("❌ Falsche Antwort von " + sender);
        }
    }
}

void MainWindow::updatePunktestand()
{
    ui->textEdit_3->setText(
        "Spieler 1: " + QString::number(spieler1_punkte) + " Punkte\n"
        "Spieler 2: " + QString::number(spieler2_punkte) + " Punkte"
        );
}

void MainWindow::ladeFragenAusDatei(const QString &dateipfad)
{
    QFile file(dateipfad);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Konnte Datei nicht öffnen:" << dateipfad;
        return;
    }

    QTextStream in(&file);      //Zeilenweise lesen
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList parts = line.split("|");
        if (parts.size() == 2) {
            fragen.append(parts.at(0));
            antworten.append(parts.at(1));
        }
    }

    file.close();
}

