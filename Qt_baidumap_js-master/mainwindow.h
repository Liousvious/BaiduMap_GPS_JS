#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
//#include <QTimer>
#include <QWebChannel>
#include <QWebEngineView>
#include "GpsClient.h"

namespace Ui {
class MainWindow;
}

class GpsClient;    //for Gps Thread

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void setflag(bool);
  //void timerout();
  void GpsUpdate(double longitude, double latitude);
  void onGpsClientRun();

private:
  Ui::MainWindow *ui;
  bool loadflag;
  double lat;
  double lon;
  QWebEngineView *pEngView;
  GpsClient *MyGps;

};

#endif // MAINWINDOW_H
