#include "packet.h"

#include "packet_connect.h"

#include "../main.h"
#include "../session.h"


/*
 * Any documentation references below, references the document https://docs.oasis-open.org/mqtt/mqtt/v5.0/os/mqtt-v5.0-os.html
 */

BasePacket::BasePacket()
: m_has_error(false)
{
}

std::shared_ptr<BasePacket> BasePacket::createPacket(const uint8_t* buffer, size_t length, size_t& fixed_header_length, size_t& total_length)
{
  ::getSessionManager()->expireOldSessions();

  if (length == 0)
    return std::make_shared<ReservedPacket>();

  size_t parse_pos = 0;

  // 2.1.2, MQTT Control Packet type
  std::shared_ptr<BasePacket> packet;
  uint8_t remaining_bits = buffer[parse_pos] & 0x0F;
  switch((buffer[parse_pos++] & 0xF0) >> 4)
  {
    default: [[fallthrough]];
    case  0: packet=std::make_shared<ReservedPacket>(); break;
    case  1: packet=std::make_shared<ConnectPacket>(); if (remaining_bits != 0) packet->setHasError(); break;
    case  2: packet=std::make_shared<ConnAckPacket>(); if (remaining_bits != 0) packet->setHasError(); break;
    case  3: packet=std::make_shared<PublishPacket>(); break;
    case  4: packet=std::make_shared<PubAckPacket>(); if (remaining_bits != 0) packet->setHasError(); break;
    case  5: packet=std::make_shared<PubRecPacket>(); if (remaining_bits != 0) packet->setHasError(); break;
    case  6: packet=std::make_shared<PubRelPacket>(); if (remaining_bits != 0x02) packet->setHasError(); break;
    case  7: packet=std::make_shared<PubCompPacket>(); if (remaining_bits != 0) packet->setHasError(); break;
    case  8: packet=std::make_shared<SubscribePacket>(); if (remaining_bits != 0x02) packet->setHasError(); break;
    case  9: packet=std::make_shared<SubAckPacket>(); if (remaining_bits != 0) packet->setHasError(); break;
    case 10: packet=std::make_shared<UnsubscribePacket>(); if (remaining_bits != 0x02) packet->setHasError(); break;
    case 11: packet=std::make_shared<UnsubAckPacket>(); if (remaining_bits != 0) packet->setHasError(); break;
    case 12: packet=std::make_shared<PingReqPacket>(); if (remaining_bits != 0) packet->setHasError(); break;
    case 13: packet=std::make_shared<PingRespPacket>(); if (remaining_bits != 0) packet->setHasError(); break;
    case 14: packet=std::make_shared<DisconnectPacket>(); if (remaining_bits != 0) packet->setHasError(); break;
    case 15: packet=std::make_shared<AuthPacket>(); if (remaining_bits != 0) packet->setHasError(); break;
  };

  uint32_t value;
  if (packet->parseVariableByteInteger(buffer, length, parse_pos, value))
  {
    fixed_header_length = parse_pos;
    total_length = fixed_header_length + value;
  }
  else
  {
    fixed_header_length = total_length = 0;
    packet->setHasError();
  }

  return packet;
}
/*
 * If returning true, parse_pos will be incremented by the number of bytes consumed
 */
bool BasePacket::parseVariableByteInteger(const uint8_t* buffer, size_t length, size_t& parse_pos, uint32_t& value)
{
  value = 0;
  uint32_t multiplier = 1;
  uint8_t encoded_byte;
  // Algorithm from 1.5.5, Variable Byte Integer
  size_t offset = 0;
  do
  {
    if ((parse_pos+offset) >= length)
    {
      value = 0;
      return false;
    }

    encoded_byte = buffer[parse_pos + offset++];
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

  parse_pos += offset;
  return true;
}
