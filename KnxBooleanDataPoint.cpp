#include <knx/datapointtypes/DataPointType.h>
#include <KnxBooleanDataPoint.h>

KnxBooleanDataPoint::KnxBooleanDataPoint(
    KnxClientConnection &knxClientConnection, GroupAddress gaRead,
    GroupAddress gaWrite)
    : knxClientConnection{knxClientConnection}, gaRead{gaRead},
      gaWrite{gaWrite} {}

void KnxBooleanDataPoint::addUpdateListener(UpdateCB &&updateCB) {
  listeners.emplace_back(std::move(updateCB));
}

bool KnxBooleanDataPoint::getValue() { return this->value; }

awaitable<void> KnxBooleanDataPoint::requestUpdate() {
  co_await knxClientConnection.readGroup(gaRead);
}

asio::awaitable<void> KnxBooleanDataPoint::setValue(bool value) {
  co_await knxClientConnection.writeToGroup(
      gaWrite, value, true);
}

void KnxBooleanDataPoint::onGroupReadResponse(
    const IndividualAddress &source, const GroupAddress &ga,
    std::span<const std::uint8_t> data) {
  if (ga == gaRead) {
    std::cout << "Got new value via Group Read Response" << std::endl;
    bool newValue = knx::datapoint::BooleanDataPointType::toValue(data);
    if (newValue != value) {
      value = newValue;
      for(auto& updateCB : listeners) {
        updateCB();
      }
    }
  }
}

void KnxBooleanDataPoint::onGroupWrite(const IndividualAddress &source,
                                       const GroupAddress &ga,
                                       std::span<const std::uint8_t> data) {
  if (ga == gaRead) {
    std::cout << "Got new value via Group Write Response\n";
    bool newValue = knx::datapoint::BooleanDataPointType::toValue(data);
    if (newValue != value) {
      value = newValue;
      for(auto& updateCB : listeners) {
        updateCB();
      }
    }
  }
}
