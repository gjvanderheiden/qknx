#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "datapoints/KnxBooleanDataPoint.h"
#include "datapoints/KnxFloatDataPoint.h"
#include "datapoints/KnxIntegerDataPoint.h"
#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <asio/awaitable.hpp>
#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>
#include <knx/connection/KnxClientConnection.h>
#include <knx/connection/KnxConnectionFactory.h>
#include <knx/connection/KnxConnectionListener.h>
#include <thread>

namespace Ui {
class MainWindow;
}

class QKnxListener : public KnxConnectionListener {
public:
  QKnxListener() = default;
  ~QKnxListener() override = default;
  void onConnect() override;
  void onDisconnect() override;

  void onGroupRead(const IndividualAddress &source,
                   const GroupAddress &ga) override;

  void onGroupReadResponse(const IndividualAddress &source,
                           const GroupAddress &ga,
                           std::span<const std::uint8_t> data) override;

  void onGroupWrite(const IndividualAddress &source, const GroupAddress &ga,
                    std::span<const std::uint8_t> data) override;
};

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow();
  ~MainWindow();

public slots:
  void on_lampjeBankCheckBox_toggled(bool state);
  void on_horizontalSlider_sliderReleased();
  void on_updateButton_clicked();

private:
  void valueChanged();
  void valueLightChanged();
  void valueTempBuitdenChanged();

private:
  asio::io_context ioc_;
  Ui::MainWindow *ui_;
  knx::connection::KnxClientConnection knxConnection;
  std::thread *t_;
  std::unique_ptr<asio::steady_timer> timer1_;
  std::shared_ptr<KnxBooleanDataPoint> datapoint;
  std::shared_ptr<KnxIntegerDataPoint> light;
  std::shared_ptr<KnxFloatDataPoint> tempBuitenDP;
  std::shared_ptr<QKnxListener> knxListener;
};

#endif // MAINWINDOW_H
