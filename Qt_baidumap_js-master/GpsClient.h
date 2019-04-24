#ifndef GPSCLIENT_H
#define GPSCLIENT_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QStringList>
#include <QString>
#include <QThread>
#include <QJsonObject>
#include <QJsonArray>
//#include "mainwindow.h"
struct Loc
{
    double lon;
    double lat;
};

class GpsClient : public QThread
{
    Q_OBJECT
public:
    GpsClient(QObject *parent = nullptr);
    void parse(const QString &rmc);
    bool outOfChina(double lon, double lat);
    double transformLon(double x, double y);
    double transformLat(double x, double y);


    Loc wgs2gcj(Loc gps);
    Loc gcj2bd(Loc gg);
    Loc bd2gcj(Loc bd);
    Loc gcj2wgs(Loc gcj);
    QByteArray Data;

protected:
    void run();

signals:
    void position_obstacle(double, double, QJsonArray);
    //void obstacles(QJsonArray);

public slots:
    void onsendwaypoints(QString wgs_wayresult);
    void Tcpclientconnect();

private slots:
    void onDisconnect();

private:
    QTcpSocket *tcpSocket;
    QThread *myThread;
    //MainWindow *Gpsmainwindow;
    QHostAddress tcpClientTargetAddr;
    const int timeout = 5000;
    const double pi = 3.1415926535897932384626;
    const double a = 6378245.0;
    const double ee = 0.00669342162296594323;
    const double x_pi = 3.14159265358979324 * 3000.0 / 180.0;

};


#endif
