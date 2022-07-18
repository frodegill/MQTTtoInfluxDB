#include "packet.h"

/*
 * Any documentation references below, references the document https://docs.oasis-open.org/mqtt/mqtt/v5.0/os/mqtt-v5.0-os.html
 */

BasePacket::BasePacket()
: m_has_error(false)
{
}

std::shared_ptr<BasePacket> BasePacket::createPacket(const uint8_t* buffer, size_t length, size_t& fixed_header_length, size_t& total_length)
{
  if (length == 0)
    return std::make_shared<ReservedPacket>();

  // 2.1.2, MQTT Control Packet type
  std::shared_ptr<BasePacket> packet;
  switch((*buffer & 0xF0) >> 4)
  {
    default: [[fallthrough]];
    case  0: packet=std::make_shared<ReservedPacket>(); break;
    case  1: packet=std::make_shared<ConnectPacket>(); break;
    case  2: packet=std::make_shared<ConnAckPacket>(); break;
    case  3: packet=std::make_shared<PublishPacket>(); break;
    case  4: packet=std::make_shared<PubAckPacket>(); break;
    case  5: packet=std::make_shared<PubRecPacket>(); break;
    case  6: packet=std::make_shared<PubRelPacket>(); break;
    case  7: packet=std::make_shared<PubCompPacket>(); break;
    case  8: packet=std::make_shared<SubscribePacket>(); break;
    case  9: packet=std::make_shared<SubAckPacket>(); break;
    case 10: packet=std::make_shared<UnsubscribePacket>(); break;
    case 11: packet=std::make_shared<UnsubAckPacket>(); break;
    case 12: packet=std::make_shared<PingReqPacket>(); break;
    case 13: packet=std::make_shared<PingRespPacket>(); break;
    case 14: packet=std::make_shared<DisconnectPacket>(); break;
    case 15: packet=std::make_shared<AuthPacket>(); break;
  };

  size_t parsed_length;
  uint32_t value;
  if (packet->parseVariableByteInteger(buffer+1, length-1, parsed_length, value))
  {
    fixed_header_length = 1 + parsed_length;
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
 * If returning true, pos will be incremented by the number of bytes consumed
 */
bool BasePacket::parseVariableByteInteger(const uint8_t* buffer, size_t length, size_t& parsed_length, uint32_t& value)
{
  parsed_length = 0;
  value = 0;
  uint32_t multiplier = 1;
  uint8_t encoded_byte;
  // Algorithm from 1.5.5, Variable Byte Integer
  size_t offset = 0;
  do
  {
    if (offset >= length)
    {
      value = 0;
      return false;
    }

    encoded_byte = buffer[offset++];
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

  parsed_length = offset;
  return true;
}


bool ConnectPacket::parse([[maybe_unused]] const uint8_t* buffer, size_t length)
{
  if (length == 0)
    return setHasError();

  size_t parse_pos = 0;
  //3.1.1, CONNECT Fixed Header
  if (buffer[parse_pos] != 0x10)
    return setHasError();

  if (++parse_pos >= length) //Skip first byte of Fixexd Header
    return setHasError();

  uint32_t remaining_length;
  if (!parseVariableByteInteger(buffer, length, parse_pos, remaining_length))
    return setHasError();

  // 3.1.2.1, Protocol Name
  if (parse_pos+6 >= length)
    return setHasError();

  if (buffer[parse_pos++] != 0 ||
      buffer[parse_pos++] != 4 ||
      buffer[parse_pos++] != 'M' ||
      buffer[parse_pos++] != 'Q' ||
      buffer[parse_pos++] != 'T' ||
      buffer[parse_pos++] != 'T')
  {
    return setHasError();
  }

  // 3.1.2.2, Protocol Version
  if (parse_pos+1 >= length)
    return setHasError();

  m_client_version = buffer[parse_pos++];

  // 3.1.2.3, Connect Flags
  if (parse_pos+1 >= length)
    return setHasError();

  uint8_t connect_flag = buffer[parse_pos++];
  if ((connect_flag & 0x01) != 0)
  {
    return setHasError();
  }

  //TODO

  return length;
}
