#pragma once

#include "esphome/core/component.h"
#include "esphome/components/remote_base/remote_base.h"

#include <cinttypes>

namespace esphome::remote_base {

struct GaviotaData {
  uint32_t address;
  uint8_t channel;
  uint8_t command;

  bool operator==(const GaviotaData &rhs) const {
    return address == rhs.address &&
           channel == rhs.channel &&
           command == rhs.command;
  }
};

class GaviotaProtocol : public RemoteProtocol<GaviotaData> {
 public:
  void encode(RemoteTransmitData *dst, const GaviotaData &data) override;
  optional<GaviotaData> decode(RemoteReceiveData src) override;
  void dump(const GaviotaData &data) override;
};

DECLARE_REMOTE_PROTOCOL(Gaviota)

template<typename... Ts>
class GaviotaAction : public RemoteTransmitterActionBase<Ts...> {
 public:
  TEMPLATABLE_VALUE(uint32_t, address)
  TEMPLATABLE_VALUE(uint8_t, channel)
  TEMPLATABLE_VALUE(uint8_t, command)

  void encode(RemoteTransmitData *dst, Ts... x) override {
    GaviotaData data{};
    data.address = this->address_.value(x...);
    data.channel = this->channel_.value(x...);
    data.command = this->command_.value(x...);
    GaviotaProtocol().encode(dst, data);
  }
};

}  // namespace esphome::remote_base
