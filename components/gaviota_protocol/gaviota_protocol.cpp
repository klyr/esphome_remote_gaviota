#include "gaviota_protocol.h"
#include "esphome/core/log.h"

namespace esphome::remote_base {

static const char *const TAG = "remote.gaviota";

static constexpr uint32_t HEADER_HIGH_US = 4774;
static constexpr uint32_t HEADER_LOW_US = 1514;
static constexpr uint32_t BIT_ZERO_HIGH_US = 351;
static constexpr uint32_t BIT_ZERO_LOW_US = 736;
static constexpr uint32_t BIT_ONE_HIGH_US = 712;
static constexpr uint32_t BIT_ONE_LOW_US = 374;

static constexpr uint8_t ADDRESS_BITS = 28;
static constexpr uint8_t CHANNEL_BITS = 4;
static constexpr uint8_t COMMAND_BITS = 8;
static constexpr uint8_t TOTAL_BITS = ADDRESS_BITS + CHANNEL_BITS + COMMAND_BITS;

struct CommandInfo {
  uint8_t code;
  const char *name;
  const char *bits;
};

static constexpr CommandInfo COMMANDS[] = {
    {0x00, "Unknown",      "???? ????"},
    {0x11, "Up",           "0001 0001"},
    {0x1E, "Release Up",   "0001 1110"},
    {0x33, "Down",         "0011 0011"},
    {0x3C, "Release Down", "0011 1100"},
    {0x55, "Stop",         "0101 0101"},
    {0x80, "Stop + Up",    "1000 0000"},
    {0x81, "Stop + Down",  "1000 0001"},
    {0xCC, "P2",           "1100 1100"},
    {0xC1, "M",            "1100 0001"},
};

static constexpr size_t COMMANDS_COUNT = sizeof(COMMANDS) / sizeof(COMMANDS[0]);

static const CommandInfo *find_command_info(uint8_t command) {
  for (size_t i = 1; i < COMMANDS_COUNT; i++) {
    const auto &entry = COMMANDS[i];
    if (entry.code == command) {
      return &entry;
    }
  }
  return &COMMANDS[0];
}

void GaviotaProtocol::encode(RemoteTransmitData *dst, const GaviotaData &data) {
  dst->set_carrier_frequency(0);
  dst->reserve(2 + TOTAL_BITS * 2u);

  dst->item(HEADER_HIGH_US, HEADER_LOW_US);

  for (uint32_t mask = 1u << (ADDRESS_BITS - 1); mask != 0; mask >>= 1) {
    if (data.address & mask) {
      dst->item(BIT_ONE_HIGH_US, BIT_ONE_LOW_US);
    } else {
      dst->item(BIT_ZERO_HIGH_US, BIT_ZERO_LOW_US);
    }
  }

  for (uint32_t mask = 1u << (CHANNEL_BITS - 1); mask != 0; mask >>= 1) {
    if (data.channel & mask) {
      dst->item(BIT_ONE_HIGH_US, BIT_ONE_LOW_US);
    } else {
      dst->item(BIT_ZERO_HIGH_US, BIT_ZERO_LOW_US);
    }
  }

  for (uint32_t mask = 1u << (COMMAND_BITS - 1); mask != 0; mask >>= 1) {
    if (data.command & mask) {
      dst->item(BIT_ONE_HIGH_US, BIT_ONE_LOW_US);
    } else {
      dst->item(BIT_ZERO_HIGH_US, BIT_ZERO_LOW_US);
    }
  }
}

optional<GaviotaData> GaviotaProtocol::decode(RemoteReceiveData src) {
  GaviotaData out{
      .address = 0,
      .channel = 0,
      .command = 0,
  };
  if (!src.expect_item(HEADER_HIGH_US, HEADER_LOW_US))
    return {};

  for (uint8_t i = 0; i < ADDRESS_BITS; i++) {
    if (src.expect_item(BIT_ONE_HIGH_US, BIT_ONE_LOW_US)) {
      out.address = (out.address << 1) | 1;
    } else if (src.expect_item(BIT_ZERO_HIGH_US, BIT_ZERO_LOW_US)) {
      out.address = (out.address << 1) | 0;
    } else {
      return {};
    }
  }

  for (uint8_t i = 0; i < CHANNEL_BITS; i++) {
    if (src.expect_item(BIT_ONE_HIGH_US, BIT_ONE_LOW_US)) {
      out.channel = (out.channel << 1) | 1;
    } else if (src.expect_item(BIT_ZERO_HIGH_US, BIT_ZERO_LOW_US)) {
      out.channel = (out.channel << 1) | 0;
    } else {
      return {};
    }
  }

  // Parse all command bits except the last one; decode the trailing bit with
  // expect_mark() because the frame can end without a final space.
  for (uint8_t i = 0; i < COMMAND_BITS - 1; i++) {
    if (src.expect_item(BIT_ONE_HIGH_US, BIT_ONE_LOW_US)) {
      out.command = (out.command << 1) | 1;
    } else if (src.expect_item(BIT_ZERO_HIGH_US, BIT_ZERO_LOW_US)) {
      out.command = (out.command << 1) | 0;
    } else {
      return {};
    }
  }

  if (src.expect_mark(BIT_ONE_HIGH_US)) {
    out.command = (out.command << 1) | 1;
  } else if (src.expect_mark(BIT_ZERO_HIGH_US)) {
    out.command = (out.command << 1) | 0;
  } else {
    return {};
  }

  return out;
}

void GaviotaProtocol::dump(const GaviotaData &data) {
  const CommandInfo *command = find_command_info(data.command);
  ESP_LOGI(TAG, "Received Gaviota: address=0x%08" PRIX32 ", channel=%u, command=0x%02X bits=%s (%s)", data.address,
           data.channel, data.command, command->bits, command->name);
}

}  // namespace esphome::remote_base
