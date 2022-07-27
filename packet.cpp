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

  size_t parse_pos = 0;

  // 2.1.2, MQTT Control Packet type
  std::shared_ptr<BasePacket> packet;
  switch((buffer[parse_pos++] & 0xF0) >> 4)
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

  if (buffer[parse_pos] != 0 ||
      buffer[parse_pos+1] != 4 ||
      buffer[parse_pos+2] != 'M' ||
      buffer[parse_pos+3] != 'Q' ||
      buffer[parse_pos+4] != 'T' ||
      buffer[parse_pos+5] != 'T')
  {
    return setHasError();
  }
  parse_pos += 6;

  // 3.1.2.2, Protocol Version
  if (parse_pos+1 >= length)
    return setHasError();

  m_client_version = buffer[parse_pos++];
  if (5 != m_client_version)
  {
    //TODO "If the Protocol Version is not 5 and the Server does not want to accept the CONNECT packet, the Server MAY send a CONNACK packet with Reason Code 0x84 (Unsupported Protocol Version) and then MUST close the Network Connection"
  }

  // 3.1.2.3, Connect Flags
  if (parse_pos+1 >= length)
    return setHasError();

  uint8_t connect_flag = buffer[parse_pos++];
  if ((connect_flag & 0x01) != 0)
  {
    return setHasError();
  }

  m_clean_start_flag = (connect_flag & 0x02) >> 1;
  m_will_flag = (connect_flag & 0x04) >> 2;
  m_will_qos_flag = (connect_flag & 0x18) >> 3;
  m_will_retain_flag = (connect_flag & 0x20) >> 5;
  m_password_flag = (connect_flag & 0x40) >> 6;
  m_username_flag = (connect_flag & 0x80) >> 7;

  // 3.1.2.10, Keep Alive
  if (parse_pos+2 >= length)
    return setHasError();

  m_keep_alive = buffer[parse_pos]<<8 | buffer[parse_pos+1];
  parse_pos += 2;

  // 3.1.2.11, Property Length
  uint32_t property_length;
  if (!parseVariableByteInteger(buffer, length, parse_pos, property_length))
    return setHasError();

  //TODO

  return length;
}
