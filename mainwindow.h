#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "KnxBooleanDataPoint.h"
#include "KnxIntegerDataPoint.h"
#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <asio/steady_timer.hpp>
#include <knx/connection/KnxClientConnection.h>
#include <knx/connection/KnxConnectionFactory.h>
#include <thread>
#include <asio/awaitable.hpp>
#include <asio/io_context.hpp>


namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

public slots:
    void on_lampjeBankCheckBox_toggled(bool state);
    void on_horizontalSlider_sliderReleased();
private:
    void valueChanged();
    void valueLightChanged();

private:
    asio::io_context ioc_;
    Ui::MainWindow* ui_;
    knx::connection::KnxClientConnection knxConnection;
    std::thread* t_;
    std::unique_ptr<asio::steady_timer> timer1_;
    std::shared_ptr<KnxBooleanDataPoint> datapoint;
    std::shared_ptr<KnxIntegerDataPoint> light;
};

#endif // MAINWINDOW_H
