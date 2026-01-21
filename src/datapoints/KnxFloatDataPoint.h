#pragma once

#include <cstdint>
#include <knx/connection/KnxClientConnection.h>
#include <knx/datapointtypes/Format.h>
#include <knx/headers/KnxAddress.h>
#include <memory>

using namespace knx::connection;
using UpdateCB = std::function<void()>;

class KnxFloatDataPoint final
    : public KnxConnectionListener,  
      std::enable_shared_from_this<KnxFloatDataPoint> {
public:
  KnxFloatDataPoint(KnxClientConnection &knxClientConnection,
                      GroupAddress gaRead, GroupAddress gaWrite);

  double getValue();
  asio::awaitable<void> setValue(double value);
  asio::awaitable<void> requestUpdate();

  void addUpdateListener(UpdateCB&& updateCB);

private:
  void onConnect() override {};
  void onDisconnect() override {};

  void onGroupRead(const IndividualAddress &source,
                   const GroupAddress &ga) override {}

  void onGroupReadResponse(const IndividualAddress &source,
                           const GroupAddress &ga,
                           std::span<const std::uint8_t> data) override;

  void onGroupWrite(const IndividualAddress &source, const GroupAddress &ga,
                    std::span<const std::uint8_t> data) override;

private:
  KnxClientConnection &knxClientConnection;
  GroupAddress gaRead, gaWrite;
  std::vector<UpdateCB> listeners{};
  double value{0.0};
};
