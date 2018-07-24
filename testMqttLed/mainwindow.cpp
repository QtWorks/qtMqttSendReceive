#include "mainwindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets/QMessageBox>
//#include <QtMqtt/QMqttClient>


#define TOPIC_UP "leds-control/devices/leds-1/up"
#define TOPIC_DOWN "leds-control/devices/leds-1/down"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_client = new QMqttClient(this);
    m_client->setHostname("eu.thethings.network");
    m_client->setPort(1883);
    m_client->setUsername("leds-control");
    m_client->setPassword("ttn-account-v2.GzYy6dxbgfWGUqkef_7JZ6YQjjlNLdCejB4bGS58BZM");

    connect(m_client, &QMqttClient::disconnected, this, &MainWindow::brokerDisconnected);
    connect(m_client, &QMqttClient::stateChanged, this, &MainWindow::stateChange);
    connect(m_client, &QMqttClient::messageReceived, this, &MainWindow::receivedMessage);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pbConnexion_clicked()
{
    if (m_client->state() == QMqttClient::Disconnected) {
        ui->pbConnexion->setText(tr("Deconnexion"));
        m_client->connectToHost();

    } else {
        ui->pbConnexion->setText(tr("Connexion"));
        m_client->disconnectFromHost();
    }
}

void MainWindow::brokerDisconnected()
{
    QMessageBox::critical(this, "Erreur", "Connexion avec le broker interrompue");
}

void MainWindow::stateChange()
{
    QString message;
    switch (m_client->state())
    {
        case 0 :message = "Déconnecté";break;
        case 1 :message = "En cours de connexion";break;
        case 2 :message = "Connecté";
            QString myTopic1 = TOPIC_UP"/led";
            auto subscription1 = m_client->subscribe(myTopic1);
            if (!subscription1) {
                QMessageBox::critical(this, "Erreur", "Impossible de souscrire au topic\n"+myTopic1);
                return;
            }
            QString myTopic2 = TOPIC_UP"/pwm";
            auto subscription2 = m_client->subscribe(myTopic2);
            if (!subscription2) {
                QMessageBox::critical(this, "Erreur", "Impossible de souscrire au topic\n"+myTopic2);
                return;
            }
            break;
    }

    ui->statusBar->showMessage(message);
}

void MainWindow::receivedMessage(const QByteArray &message, const QMqttTopicName &topic)
{

    ui->statusBar->showMessage("Receive Data ...",1000);
    QString unit;
    if(topic.name()==TOPIC_UP"/led")
    {
        unit = " ";
        ui->lbHum->setText(message+unit);
        qDebug()<< message;
    }

    if(topic.name()==TOPIC_UP"/pwm")
    {
        int value = message.toInt();
        if(value>100) value = 100;
        ui->progressPWM->setValue(value);
        qDebug()<< message;
    }

}


void MainWindow::on_pbPWM_clicked()
{
    QByteArray payload;

    QByteArray port = "{\"port\":1,";
    QByteArray payload_raw = "\"payload_raw\":\"";
    QByteArray end = "\"}";
    QByteArray message;

    QString myTopic1 = TOPIC_DOWN;

    int pwm;
    pwm = ui->slidePWM->value();
    pwm = pwm << 1;

    payload[0] =pwm + ui->checkBox->isChecked();

    message.append(port);
    message.append(payload_raw);
    message.append(payload.toBase64());
    message.append(end);
    m_client->publish(myTopic1,message,2);
    qDebug() << message;
    ui->statusBar->showMessage("Send Data ...",1000);
}
