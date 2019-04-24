#include "mainwindow.h"
#include "bridge.h"
#include "ui_mainwindow.h"
#include <QUrl>
#include <QWebChannel>
#include <QWebEngineView>
#include "GpsClient.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("BMAP_GPS");

    MyGps = new GpsClient();
    loadflag = false;

    QWebChannel *channel = new QWebChannel(this);

    //channel->registerObject(QString("bridge"), (QObject *)bridge::instance());
    channel->registerObject(QString("showcoord"), this);
    channel->registerObject(QString("returnpoints"), this);

    ui->m_webview->page()->setWebChannel(channel);
    ui->m_webview->setContextMenuPolicy(Qt::NoContextMenu);
    ui->m_webview->page()->load(QUrl("qrc:/Bmap.html"));
    ui->m_webview->show();

    connect(ui->client_Clicked, SIGNAL(clicked()), this, SLOT(onGpsClientRun()));
    connect(ui->m_webview, SIGNAL(loadFinished(bool)), this, SLOT(setflag(bool)));    //this indicate once the page was loaded
                                                                                        //successfully, the load flag will become true
    connect(MyGps, SIGNAL(position_obstacle(double, double, QJsonArray)), SLOT(GpsUpdate(double, double, QJsonArray)));
    connect(ui->Disconnect_Clicked, SIGNAL(clicked()), MyGps, SLOT(onDisconnect()));
    connect(ui->path_Clicked, SIGNAL(clicked()), this, SLOT(onPathPlanning()));
    connect(this, SIGNAL(waypoints(QString)), MyGps, SLOT(onsendwaypoints(QString)));
    connect(ui->client_connect, SIGNAL(clicked()), MyGps, SLOT(Tcpclientconnect()));
    //connect(MyGps, SIGNAL(obstacles(QJsonArray)), this, SLOT(onObstacles(QJsonArray)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onGpsClientRun()
{
    MyGps->start();
}

void MainWindow::setflag(bool t)
{
    loadflag = t;
}

/*******************************************************************************
 *
 * run javascript via TCP protocal
 *
 * ****************************************************************************/

void MainWindow::GpsUpdate(double longitude, double latitude, QJsonArray json_arr)
{
    qDebug() << json_arr;
    QString json_str = QString(QJsonDocument(json_arr).toJson()).remove(QRegExp("\\s"));
    qDebug() << json_str;
    if (loadflag == true){
        double lng = longitude;
        double lat = latitude;
        qDebug() << fixed << qSetRealNumberPrecision(10) << longitude << latitude;
        ui->m_webview->page()->runJavaScript(QString("theLocation_obstacle(%1,%2,%3)").arg(lng, 0, 'g', 11).arg(lat, 0, 'g', 10).arg(json_str));
    }
}

void MainWindow::onPathPlanning()
{
    disconnect(ui->path_Clicked, SIGNAL(clicked()), this, SLOT(onPathPlanning()));
    connect(ui->path_Clicked,SIGNAL(clicked()), this, SLOT(onExitPathPlanning()));
    ui->path_Clicked->setText("Exit");
    if (loadflag == true){
        ui->m_webview->page()->runJavaScript(QString("pathplanning()"));
    }
}

void MainWindow::onExitPathPlanning()
{
    disconnect(ui->path_Clicked,SIGNAL(clicked()), this, SLOT(onExitPathPlanning()));
    connect(ui->path_Clicked, SIGNAL(clicked()), this, SLOT(onPathPlanning()));
    ui->path_Clicked->setText("Path Plan");
    ui->m_webview->page()->runJavaScript(QString("exitPathPlanning()"));
}
/**************************************************************************************
 *
 * Qwebchannel: for data interaction
 *
 * ************************************************************************************/

void MainWindow::getcoordinates(double lon, double lat)
{
    double wgs_longitude;
    double wgs_latitude;
    Loc bdcoord, gcjcoord, wgscoord;
    bdcoord.lon = lon;
    bdcoord.lat = lat;
    gcjcoord = MyGps->bd2gcj(bdcoord);
    wgscoord = MyGps->gcj2wgs(gcjcoord);
    wgs_longitude = wgscoord.lon;
    wgs_latitude = wgscoord.lat;

    QString showlon = "Mouse WGS_Longitude: " + QString::number(wgs_longitude, 'g' , 12) + " °";
    QString showlat = "Mouse WGS_Latitude: " + QString::number(wgs_latitude, 'g', 11) + " °";
    ui->label_mouse_longitude->setText(showlon);
    ui->label_mouse_latitude->setText(showlat);
}

void MainWindow::returnWayPoints(const QString &r_waypoints)
{
    qDebug() << r_waypoints;
    wgs_wayresult = "";
    wgs_waypointsArr.clear();
    Loc point_bd, point_gcj, point_wgs;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(r_waypoints.toLocal8Bit().data());
    QJsonArray jsonarray = jsonDocument.array();
    qDebug() << jsonarray;

    for (int i = 0; i < jsonarray.size(); i++) {
        point_bd.lat = jsonarray.at(i).toObject().value("lat").toDouble();
        point_bd.lon = jsonarray.at(i).toObject().value("lng").toDouble();
        //qDebug() << fixed << qSetRealNumberPrecision(8) << point_bd.lon;
        point_gcj = MyGps->bd2gcj(point_bd);
        point_wgs = MyGps->gcj2wgs(point_gcj);
        QString wgs_waypoints = QString::number(point_wgs.lon, 'g', 11) + "," + QString::number(point_wgs.lat, 'g', 10);
        wgs_waypointsArr.append(wgs_waypoints);
        qDebug() <<  fixed << qSetRealNumberPrecision(8) << point_wgs.lon <<','<< point_wgs.lat;
    }

    for (int i = 0; i < wgs_waypointsArr.size(); i++) {
        wgs_wayresult += wgs_waypointsArr.at(i) + ";";
    }
    wgs_wayresult = "$" + wgs_wayresult.left(wgs_wayresult.length()-1);
    emit waypoints(wgs_wayresult);
    qDebug() << wgs_wayresult;  //used to store a list of QString which is the final result path planning waypoints
}
