#include <cstdint>
#include <knx/datapointtypes/Format.h>
#include "datapoints/KnxIntegerDataPoint.h"

KnxIntegerDataPoint::KnxIntegerDataPoint(
    KnxClientConnection &knxClientConnection, GroupAddress gaRead,
    GroupAddress gaWrite)
    : knxClientConnection{knxClientConnection}, gaRead{gaRead},
      gaWrite{gaWrite} {}

void KnxIntegerDataPoint::addUpdateListener(UpdateCB &&updateCB) {
  listeners.emplace_back(std::move(updateCB));
}

std::uint8_t KnxIntegerDataPoint::getValue() { return this->value; }

asio::awaitable<void> KnxIntegerDataPoint::requestUpdate() {
  co_await knxClientConnection.sendReadGroup(gaRead);
}

asio::awaitable<void> KnxIntegerDataPoint::setValue(std::uint8_t value) {
  std::cout << "KnxIntegerDataPoint::setValue(" << (int)value << " : std::uint8_t)\n";
  co_await knxClientConnection.writeToGroup(
      gaWrite, value, false);
}

void KnxIntegerDataPoint::onGroupReadResponse(
    const IndividualAddress &source, const GroupAddress &ga,
    std::span<const std::uint8_t> data) {
  if (ga == gaRead) {
    std::cout << "Got new value via Group Read Response" << std::endl;
    auto newValue = knx::datapoint::UInt8Format::toValue(data);
    if (newValue != value) {
      value = newValue;
      for(auto& updateCB : listeners) {
        updateCB();
      }
    }
  }
}

void KnxIntegerDataPoint::onGroupWrite(const IndividualAddress &source,
                                       const GroupAddress &ga,
                                       std::span<const std::uint8_t> data) {
  if (ga == gaWrite) {
    std::cout << "Got new value via Group Write Response\n";
    auto newValue = knx::datapoint::UInt8Format::toValue(data);
    if (newValue != value) {
      value = newValue;
      for(auto& updateCB : listeners) {
        updateCB();
      }
    }
  }
}
