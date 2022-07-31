#include "packet_connect.h"

#include "../main.h"
#include "../session.h"


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

  if (++parse_pos >= length) //Skip first byte of Fixed Header
    return setHasError();

  uint32_t remaining_length;
  if (!parseVariableByteInteger(buffer, length, parse_pos, remaining_length))
    return setHasError();

  // 3.1.2.1, Protocol Name
  std::string protocol_name;
  if (!parseString(buffer, length, parse_pos, protocol_name) || protocol_name.compare("MQTT")!=0)
    return setHasError();

  // 3.1.2.2, Protocol Version
  if (!parseUint8(buffer, length, parse_pos, m_client_version))
    return setHasError();

  if (m_client_version<4 || m_client_version>5)
  {
    //TODO "If the Protocol Version is not 5 and the Server does not want to accept the CONNECT packet, the Server MAY send a CONNACK packet with Reason Code 0x84 (Unsupported Protocol Version) and then MUST close the Network Connection"
  }

  // 3.1.2.3, Connect Flags
  uint8_t connect_flag;
  if (!parseUint8(buffer, length, parse_pos, connect_flag))
    return setHasError();

  if ((connect_flag & 0x01) != 0) //RESERVED flag
    return setHasError();

  m_clean_start_flag = (connect_flag & 0b00000010) >> 1;
  m_will_flag        = (connect_flag & 0b00000100) >> 2;
  m_will_qos_flag    = (connect_flag & 0b00011000) >> 3;
  m_will_retain_flag = (connect_flag & 0b00100000) >> 5;
  m_password_flag    = (connect_flag & 0b01000000) >> 6;
  m_username_flag    = (connect_flag & 0b10000000) >> 7;

  // 3.1.2.10, Keep Alive
  if (!parseUint16(buffer, length, parse_pos, m_keep_alive))
    return setHasError();

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

  if (!parseString(buffer, length, parse_pos, m_client_id))
    return setHasError();

  if (m_will_flag==1)
  {
    // 3.1.3.2.1 Property Length
    uint32_t will_property_length;
    if (!parseVariableByteInteger(buffer, length, parse_pos, will_property_length))
      return setHasError();

    uint32_t will_property_end = parse_pos + will_property_length;
    if (will_property_end >= length)
      return setHasError();

    m_will_user_properties.clear();
    while (parse_pos < will_property_end)
    {
      switch(buffer[parse_pos++])
      {
        case PropertyIdentifier::WILL_DELAY_INTERVAL:
          if (!parseUint32(buffer, length, parse_pos, m_will_delay_interval)) {return setHasError();}
          break;
        case PropertyIdentifier::PAYLOAD_FORMAT_INDICATOR:
          if (!parseUint8(buffer, length, parse_pos, m_payload_format_indicator) || m_payload_format_indicator>1) {return setHasError();}
          break;
        case PropertyIdentifier::MESSAGE_EXPIRY_INTERVAL:
          if (!parseUint32(buffer, length, parse_pos, m_message_expiry_interval)) {return setHasError();}
          break;
        case PropertyIdentifier::CONTENT_TYPE:
          if (!parseString(buffer, length, parse_pos, m_content_type)) {return setHasError();}
          break;
        case PropertyIdentifier::RESPONSE_TOPIC:
          if (!parseString(buffer, length, parse_pos, m_response_topic)) {return setHasError();}
          break;
        case PropertyIdentifier::CORRELATION_DATA:
          if (!parseBinaryData(buffer, length, parse_pos, m_correlation_data)) {return setHasError();}
          break;
        case PropertyIdentifier::USER_PROPERTY:
        {
          std::string key, value;
          if (!parseString(buffer, length, parse_pos, key) ||
              !parseString(buffer, length, parse_pos, value)) {return setHasError();}
          m_will_user_properties.emplace_back(std::pair<std::string,std::string>(key, value));
          break;
        }

        default: return setHasError();
      }
    }

    if (!parseString(buffer, length, parse_pos, m_will_topic))
      return setHasError();

    if (!parseBinaryData(buffer, length, parse_pos, m_will_payload))
      return setHasError();
  }

  if (m_username_flag==1)
  {
    if (!parseString(buffer, length, parse_pos, m_username))
      return setHasError();
  }

  if (m_password_flag==1)
  {
    if (!parseBinaryData(buffer, length, parse_pos, m_password))
      return setHasError();
  }

  return actions();
}

bool ConnectPacket::actions() // 3.1.4 CONNECT Actions
{
  //TODO

  return true;
}
