#include "mainwindow.h"
#include "bridge.h"
#include "ui_mainwindow.h"
//#include <QTimer>
#include <QUrl>
#include <QWebChannel>
#include <QWebEngineView>
#include "GpsClient.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowTitle("BMAP_GPS");

    MyGps = new GpsClient();
    loadflag = false;

    QWebChannel *channel = new QWebChannel;
    channel->registerObject("bridge", (QObject *)bridge::instance());
    ui->m_webview->page()->setWebChannel(channel);
    ui->m_webview->setContextMenuPolicy(Qt::NoContextMenu);
    ui->m_webview->page()->load(QUrl("qrc:/test.html"));
    ui->m_webview->show();

    QObject::connect(ui->client_Clicked, SIGNAL(clicked()), this, SLOT(onGpsClientRun()));
    QObject::connect(ui->m_webview, SIGNAL(loadFinished(bool)), this, SLOT(setflag(bool)));    //this indicate once the page was loaded
                                                                                        //successfully, the load flag will become true
    QObject::connect(MyGps, SIGNAL(position(double, double)), SLOT(GpsUpdate(double, double)));
    QObject::connect(ui->Disconnect_Clicked, SIGNAL(clicked()), MyGps, SLOT(onDisconnect()));

  //QTimer *timer = new QTimer(this);
  //QObject::connect(timer, SIGNAL(timeout()), this, SLOT(timerout()));
  //timer->start(30);
}

MainWindow::~MainWindow() {
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

void MainWindow::GpsUpdate(double longitude, double latitude)
{
    if (loadflag == true){
        double lng = longitude;
        double lat = latitude;
        qDebug() << fixed << qSetRealNumberPrecision(10) << longitude << latitude;
        ui->m_webview->page()->runJavaScript(QString("theLocation(%1,%2)").arg(lng, 0, 'g', 11).arg(lat, 0, 'g', 10));
    }
}


