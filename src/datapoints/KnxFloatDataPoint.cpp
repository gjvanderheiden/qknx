#include <cstdint>
#include <knx/datapointtypes/Format.h>
#include "datapoints/KnxFloatDataPoint.h"

KnxFloatDataPoint::KnxFloatDataPoint(
    KnxClientConnection &knxClientConnection, GroupAddress gaRead,
    GroupAddress gaWrite)
    : knxClientConnection{knxClientConnection}, gaRead{gaRead},
      gaWrite{gaWrite} {}

void KnxFloatDataPoint::addUpdateListener(UpdateCB &&updateCB) {
  listeners.emplace_back(std::move(updateCB));
}

double KnxFloatDataPoint::getValue() { return this->value; }

awaitable<void> KnxFloatDataPoint::requestUpdate() {
  co_await knxClientConnection.sendReadGroup(gaRead);
}

asio::awaitable<void> KnxFloatDataPoint::setValue(double value) {
  std::cout << "KnxFloatDataPoint::setValue(" << value << " : double)\n";
  co_await knxClientConnection.writeToGroup(
      gaWrite, knx::datapoint::KnxFloat16Format::toData(value));
}

void KnxFloatDataPoint::onGroupReadResponse(
    const IndividualAddress &source, const GroupAddress &ga,
    std::span<const std::uint8_t> data) {
  if (ga == gaRead) {
    std::cout << "Got new value via Group Read Response" << std::endl;
    auto newValue = knx::datapoint::KnxFloat16Format::toValue(data);
    if (newValue != value) {
      value = newValue;
      for(auto& updateCB : listeners) {
        updateCB();
      }
    }
  }
}

void KnxFloatDataPoint::onGroupWrite(const IndividualAddress &source,
                                       const GroupAddress &ga,
                                       std::span<const std::uint8_t> data) {
  if (ga == gaWrite) {
    std::cout << "Got new value via Group Write Response\n";
    auto newValue = knx::datapoint::KnxFloat16Format::toValue(data);
    if (newValue != value) {
      value = newValue;
      for(auto& updateCB : listeners) {
        updateCB();
      }
    }
  }
}
