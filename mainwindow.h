#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }  //das die ui-Klasse existiert
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void datenbearbeiten();
    void datenerhalten(const QString& sender, const QString& antwort);
    void automatischsenden();
    void updatePunktestand();

private:
    Ui::MainWindow *ui;
    QUdpSocket *m_socket;
    QTimer *m_timer;
    int index = 0;
    int spieler1_punkte = 0;
    int spieler2_punkte = 0;
    QString spieler1_ip;
    QString spieler2_ip;
    void ladeFragenAusDatei(const QString &dateiname);
    QStringList fragen;
    QStringList antworten;

};
#endif // MAINWINDOW_H
