#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <asio.hpp>
#include <asio/awaitable.hpp>
#include <asio/io_context.hpp>
#include <iostream>
#include <knx/connection/KnxClientConnection.h>
#include <knx/connection/KnxConnectionFactory.h>
#include <memory>
#include <string_view>
#include <thread>

using namespace std::literals::chrono_literals;
using namespace knx::connection;

void print(const std::error_code & /*e*/) {
  std::cout << "Timer expired" << std::endl;
}

MainWindow::MainWindow()
    : ioc_{}, knxConnection{KnxConnectionFactory::createTunnelingClient(
                  ioc_, "192.168.20.10", 3671, "192.168.10.114")} {

  qDebug() << "MainWindow::MainWindow()";
  datapoint = std::make_shared<KnxBooleanDataPoint>(
      knxConnection, GroupAddress{4, 1, 8}, GroupAddress{4, 0, 8});
  knxConnection.addListener(datapoint);
  datapoint->addUpdateListener(
      std::bind_front(&MainWindow::valueChanged, this));

  light = std::make_shared<KnxIntegerDataPoint>(
      knxConnection, GroupAddress{2, 5, 28}, GroupAddress{2, 3, 28});
  knxConnection.addListener(light);
  light->addUpdateListener(
      std::bind_front(&MainWindow::valueLightChanged, this));

  ui_ = new Ui::MainWindow();
  ui_->setupUi(this);
  co_spawn(ioc_, knxConnection.start(), asio::detached);
  t_ = new std::thread([&]() {
    ioc_.run();
    qDebug("io context done");
  });
  this->ui_->horizontalSlider->setMaximum(255);
}

MainWindow::~MainWindow() {
  qDebug() << "MainWindow::~MainWindow()";
  ioc_.stop();
  qDebug() << "MainWindow::~MainWindow() delete ui_";
  delete (ui_);
}

void MainWindow::valueChanged() {
  qDebug() << "value changed";
  ui_->lampjeBankCheckBox->blockSignals(true);
  ui_->lampjeBankCheckBox->setChecked(datapoint->getValue());
  ui_->lampjeBankCheckBox->blockSignals(false);
}

void MainWindow::valueLightChanged() {
  qDebug() << "value light changed " << light->getValue();
  ui_->horizontalSlider->blockSignals(true);
  ui_->horizontalSlider->setValue(light->getValue());
  ui_->horizontalSlider->blockSignals(false);
}
void MainWindow::on_horizontalSlider_sliderReleased() {
  qDebug() << "slider changed to " << ui_->horizontalSlider->value();
  co_spawn(ioc_,this->light->setValue(ui_->horizontalSlider->value()), asio::detached);
}

void MainWindow::on_lampjeBankCheckBox_toggled(bool state) {
  qDebug() << "toggled " << state;
  co_spawn(ioc_, datapoint->setValue(state), asio::detached);
  qDebug() << "done";
}
