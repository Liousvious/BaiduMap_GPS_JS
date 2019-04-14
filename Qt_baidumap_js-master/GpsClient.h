#ifndef GPSCLIENT_H
#define GPSCLIENT_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QStringList>
#include <QString>
#include <QThread>
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
protected:
    void run();

signals:
    void position(double, double);
private slots:
    void onDisconnect();

private:
    QTcpSocket *tcpSocket;
    QHostAddress tcpClientTargetAddr;
    const double pi = 3.14159265358979324;
    const double a = 6378245.0;
    const double ee = 0.00669342162296594323;
    const double x_pi = 3.14159265358979324 * 3000.0 / 180.0;

protected:
    void parse(const QString &rmc);
    bool outOfChina(double lon, double lat);
    double transformLon(double x, double y);
    double transformLat(double x, double y);


    Loc wgs2gcj(Loc gps);
    Loc gcj2bd(Loc gg);

};


#endif
