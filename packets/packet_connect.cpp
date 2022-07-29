#include "packet_connect.h"

/*
 * Any documentation references below, references the document https://docs.oasis-open.org/mqtt/mqtt/v5.0/os/mqtt-v5.0-os.html
 */

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
  if (m_client_version<4 || m_client_version>5)
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

  if (m_client_version >= 5)
  {
    // 3.1.2.11, Property Length
    uint32_t property_length;
    if (!parseVariableByteInteger(buffer, length, parse_pos, property_length))
      return setHasError();

    uint32_t property_end = parse_pos + property_length;
    if (property_end >= length)
      return setHasError();

    m_user_properties.clear();
    while (parse_pos < property_end)
    {
      switch(buffer[parse_pos++])
      {
        case PropertyIdentifier::SESSION_EXPIRY_INTERVAL:
          if (!parseUint32(buffer, length, parse_pos, m_session_expiry_interval)) {return setHasError();}
          break;
        case PropertyIdentifier::RECEIVE_MAXIMUM:
          if (!parseUint16(buffer, length, parse_pos, m_receive_maximum)) {return setHasError();}
          break;
        case PropertyIdentifier::MAXIMUM_PACKET_SIZE:
          if (!parseUint32(buffer, length, parse_pos, m_maximum_packet_size) || m_maximum_packet_size==0) {return setHasError();}
          break;
        case PropertyIdentifier::TOPIC_ALIAS_MAXIMUM:
          if (!parseUint16(buffer, length, parse_pos, m_topic_alias_maximum)) {return setHasError();}
          break;
        case PropertyIdentifier::REQUEST_RESPONSE_INFORMATION:
          if (!parseUint8(buffer, length, parse_pos, m_request_response_information) || m_request_response_information>1) {return setHasError();}
          break;
        case PropertyIdentifier::REQUEST_PROBLEM_INFORMATION:
          if (!parseUint8(buffer, length, parse_pos, m_request_problem_information) || m_request_problem_information>1) {return setHasError();}
          break;
        case PropertyIdentifier::USER_PROPERTY:
        {
          std::string key, value;
          if (!parseString(buffer, length, parse_pos, key) ||
              !parseString(buffer, length, parse_pos, value)) {return setHasError();}
          m_user_properties.emplace(key, value);
          break;
        }
        case PropertyIdentifier::AUTHENTICATION_METHOD:
          if (!parseString(buffer, length, parse_pos, m_authentication_method)) {return setHasError();}
          break;
        case PropertyIdentifier::AUTHENTICATION_DATA:
          if (!parseBinaryData(buffer, length, parse_pos, m_authentication_data)) {return setHasError();}
          break;

        default: return setHasError();
      }
    }
  }

  //TODO

  return length;
}
