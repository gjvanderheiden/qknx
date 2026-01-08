#pragma once

#include <cstdint>
#include <hap/coroutines/Coroutines.h>
#include <knx/connection/KnxClientConnection.h>
#include <knx/datapointtypes/Format.h>
#include <knx/headers/KnxAddress.h>
#include <memory>

using namespace knx::connection;
using UpdateCB = std::function<void()>;

class KnxIntegerDataPoint final
    : public KnxConnectionListener,  
      std::enable_shared_from_this<KnxIntegerDataPoint> {
public:
  KnxIntegerDataPoint(KnxClientConnection &knxClientConnection,
                      GroupAddress gaRead, GroupAddress gaWrite);

  std::uint8_t getValue();
  awaitable<void> setValue(std::uint8_t value);
  awaitable<void> requestUpdate();

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
  std::uint8_t value{false};
};
