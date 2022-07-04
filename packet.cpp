#include "packet.h"

BasePacket::BasePacket()
: m_has_error(false),
  m_is_completely_parsed(false)
{
}

std::shared_ptr<BasePacket> BasePacket::createPacket(const uint8_t* buffer, size_t length)
{
  if (length == 0)
    return std::make_shared<ReservedPacket>();

  // MQTT Control Packet type, https://docs.oasis-open.org/mqtt/mqtt/v5.0/os/mqtt-v5.0-os.html#_Toc3901022
  switch((*buffer & 0xF0) >> 4)
  {
    default: [[fallthrough]];
    case  0: return std::make_shared<ReservedPacket>();
    case  1: return std::make_shared<ConnectPacket>();
    case  2: return std::make_shared<ConnAckPacket>();
    case  3: return std::make_shared<PublishPacket>();
    case  4: return std::make_shared<PubAckPacket>();
    case  5: return std::make_shared<PubRecPacket>();
    case  6: return std::make_shared<PubRelPacket>();
    case  7: return std::make_shared<PubCompPacket>();
    case  8: return std::make_shared<SubscribePacket>();
    case  9: return std::make_shared<SubAckPacket>();
    case 10: return std::make_shared<UnsubscribePacket>();
    case 11: return std::make_shared<UnsubAckPacket>();
    case 12: return std::make_shared<PingReqPacket>();
    case 13: return std::make_shared<PingRespPacket>();
    case 14: return std::make_shared<DisconnectPacket>();
    case 15: return std::make_shared<AuthPacket>();
  };
}
/*
 * If returning true, pos will be incremented by the number of bytes consumed
 */
bool BasePacket::parseVariableByteInteger(const uint8_t* buffer, size_t length, size_t& pos, uint32_t& value)
{
  size_t offset = 0;
  uint32_t multiplier = 1;
  uint8_t encoded_byte;
  value = 0;
  do
  {
    if ((pos+offset) >= length)
    {
      value = 0;
      return false;
    }

    encoded_byte = buffer[pos+offset++];
    value += (encoded_byte & 0x7F) * multiplier;

    if (multiplier > 128*128*128)
    {
      setHasError();
      value = 0;
      return false;
    }

    multiplier *= 128;
  }
  while ((encoded_byte & 0x80) != 0);

  pos += offset;
  return true;
}


size_t ConnectPacket::parse([[maybe_unused]] const uint8_t* buffer, size_t length)
{
  if (length == 0)
    return 0;

  size_t parse_pos = 0;
  if (buffer[parse_pos] != 0x10) //CONNECT package should start with the binary byte 0b0001.0000
  {
    setHasError();
    return length;
  }

  if (++parse_pos >= length) //Skip first byte of Fixexd Header
    return length;

  uint32_t remaining_length;
  if (!parseVariableByteInteger(buffer, length, parse_pos, remaining_length))
    return length;

  //TODO

  return length;
}
