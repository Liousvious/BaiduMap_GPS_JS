#include "GpsClient.h"
#include <math.h>
#include <stdlib.h>
#include <iomanip>
#include <string>
#include <QTextCodec>
#include <stdio.h>

GpsClient::GpsClient(QObject *parent) : QThread (parent)
{
    tcpSocket = new QTcpSocket();
}

void GpsClient::run()
{
    const int timeout = 5000;
    tcpClientTargetAddr.setAddress("127.0.0.1");
    tcpSocket->connectToHost(tcpClientTargetAddr, 65500);

    if (!tcpSocket->waitForConnected(timeout)){
        return;
    }
    tcpSocket->write("GPS Client connected!");

    for (;;) {
        if (tcpSocket->waitForReadyRead()){  //this function returns true if the readyRead() is emitted
                                       //This function blocks until new data is avaliable
            QByteArray data = tcpSocket->readAll();    //receive data from server which is encoded by UTF-8
            //QTextCodec *codec = QTextCodec::codecForName("UTF-8");
            //QString unicode_data = codec->toUnicode(data.toLocal8Bit().data());
            std::string s_data=  data.toStdString();
            QString qt_data = QString::fromStdString(s_data);
            qDebug() << qt_data;
            //QStringList result = data.split(',');
            parse(data);
            //exec();
        }
    }
}

void GpsClient::onDisconnect()
{
    tcpSocket->disconnectFromHost();
}

void GpsClient::parse(const QString &rmc)
{
    Loc gpscoord;//googlecoord, bdcoord;
    double longitude;
    double latitude;

    QStringList array = rmc.split(',');
    if (array.length() == 2){
        gpscoord.lon = array[0].toDouble();
        gpscoord.lat = array[1].toDouble();
        qDebug() << gpscoord.lon;
        //googlecoord = wgs2gcj(gpscoord);
        //bdcoord = gcj2bd(googlecoord);
        longitude = gpscoord.lon;
        latitude = gpscoord.lat;
        qDebug() << fixed << qSetRealNumberPrecision(10) << longitude << latitude;
        emit position(longitude, latitude);
    }
}


/*bool GpsClient::outOfChina(double lon, double lat)
{
    if (lon < 72.004 || lon > 137.8347)
        return true;
    if (lat < 0.8293 || lat > 55.8271)
        return true;
    return  false;
}*/

double GpsClient::transformLon(double x, double y)
{
    double ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * sqrt(std::abs(x));
    ret += (20.0 * sin(6.0 * x * pi) + 20.0 * sin(2.0 * x * pi)) * 2.0 / 3.0;
    ret += (20.0 * sin(x * pi) + 40.0 * sin(x / 3.0 * pi)) * 2.0 / 3.0;
    ret += (150.0 * sin(x / 12.0 * pi) + 300.0 * sin(x / 30.0 * pi)) * 2.0 / 3.0;
    return ret;
}

double GpsClient::transformLat(double x, double y)
{
    double ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * sqrt(std::abs(x));
    ret += (20.0 * sin(6.0 * x * pi) + 20.0 * sin(2.0 * x * pi)) * 2.0 / 3.0;
    ret += (20.0 * sin(y * pi) + 40.0 * sin(y / 3.0 * pi)) * 2.0 / 3.0;
    ret += (160.0 * sin(y / 12.0 * pi) + 320 * sin(y * pi / 30.0)) * 2.0 / 3.0;
    return ret;
}

Loc GpsClient::wgs2gcj(Loc gps)
{
    Loc gcjcoord;
    double dLon = transformLon(gps.lon - 105.0, gps.lat - 35.0);
    double dLat = transformLat(gps.lat - 105.0, gps.lat - 35.0);
    double radLat = gps.lat / 180.0 * pi;
    double magic = sin(radLat);
    magic = 1 - ee * magic * magic;
    double sqrtMagic = sqrt(magic);
    dLon = (dLon * 180.0) / (a / sqrtMagic * cos(radLat) * pi);
    dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * pi);
    gcjcoord.lon = gps.lon + dLon;
    gcjcoord.lat = gps.lat + dLat;
    return gcjcoord;

}

Loc GpsClient::gcj2bd(Loc gg)
{
    Loc bdcoord;
    double x = gg.lon;
    double y = gg.lat;
    double z = sqrt(x * x + y * y) + 0.00002 * sin(y * x_pi);
    double theta = atan2(y, x) + 0.000003 * cos(x * x_pi);
    bdcoord.lon = z * cos(theta) + 0.0065;
    bdcoord.lat = z * sin(theta) + 0.006;
    return bdcoord;
}


