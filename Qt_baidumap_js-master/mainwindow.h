#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include <QTimer>
#include <QWebChannel>
#include <QWebEngineView>
#include "GpsClient.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonDocument>

namespace Ui {
class MainWindow;
}

class GpsClient;    //for Gps Thread

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

signals:
  void waypoints(QString);

private slots:
  void setflag(bool);
  //void timerout();
  void GpsUpdate(double longitude, double latitude, QJsonArray json_arr);
  void onGpsClientRun();
  void onPathPlanning();
  void onExitPathPlanning();
  //void onObstacles(QJsonArray json_arr);

public slots:
  void getcoordinates(double lon, double lat);
  void returnWayPoints(const QString &r_waypoints);

private:
  Ui::MainWindow *ui;
  bool loadflag;
  double lat;
  double lon;
  QList<QString> wgs_waypointsArr;
  QWebEngineView *pEngView;
  GpsClient *MyGps;

public:
  QString wgs_wayresult;
};

#endif // MAINWINDOW_H
