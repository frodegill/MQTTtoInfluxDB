#include "packet_connect.h"

#include "../main.h"
#include "../session.h"


/*
 * Any documentation references below, references the document https://docs.oasis-open.org/mqtt/mqtt/v5.0/os/mqtt-v5.0-os.html
 */

std::error_code ConnectPacket::parse()
{
  std::error_code error_code;

  //3.1.1, CONNECT Fixed Header
  //already taken care of in BasePacket::createPacket

  // 3.1.2.1, Protocol Name
  std::string protocol_name;
  RETURN_IF_ERROR(m_buffer->parseString(protocol_name));

  if (protocol_name.compare("MQTT")!=0)
    RETURN_ERROR(protocol_not_supported);

  // 3.1.2.2, Protocol Version
  RETURN_IF_ERROR(m_buffer->parseUint8(m_protocol_version));

  if (m_protocol_version<4 || m_protocol_version>5)
  {
    //"If the Protocol Version is not 5 and the Server does not want to accept the CONNECT packet, the Server MAY send a CONNACK packet with Reason Code 0x84 (Unsupported Protocol Version) and then MUST close the Network Connection"
    //TODO, send CONNACK with Reason code 0x84
    //TODO, CLose connection
    RETURN_ERROR(protocol_not_supported);
  }

  // 3.1.2.3, Connect Flags
  uint8_t connect_flag;
  RETURN_IF_ERROR(m_buffer->parseUint8(connect_flag));

  if ((connect_flag & 0x01) != 0) //RESERVED flag
    RETURN_ERROR(illegal_byte_sequence);

  m_clean_start_flag = (connect_flag & 0b00000010) >> 1;
  m_will_flag        = (connect_flag & 0b00000100) >> 2;
  m_will_qos_flag    = (connect_flag & 0b00011000) >> 3;
  m_will_retain_flag = (connect_flag & 0b00100000) >> 5;
  m_password_flag    = (connect_flag & 0b01000000) >> 6;
  m_username_flag    = (connect_flag & 0b10000000) >> 7;

  // 3.1.2.10, Keep Alive
  RETURN_IF_ERROR(m_buffer->parseUint16(m_keep_alive));

  if (m_protocol_version >= 5)
  {
    // 3.1.2.11, Property Length
    uint32_t property_length;
    RETURN_IF_ERROR(m_buffer->parseVariableByteInteger(property_length));

    uint32_t property_end = m_buffer->getParsePos() + property_length;

    m_user_properties.clear();
    while (m_buffer->getParsePos() < property_end)
    {
      uint8_t property_identifier;
      RETURN_IF_ERROR(m_buffer->parseUint8(property_identifier));

      switch(property_identifier)
      {
        case PropertyIdentifier::SESSION_EXPIRY_INTERVAL:
          RETURN_IF_ERROR(m_buffer->parseUint32(m_session_expiry_interval));
          break;
        case PropertyIdentifier::RECEIVE_MAXIMUM:
          RETURN_IF_ERROR(m_buffer->parseUint16(m_receive_maximum));
          break;
        case PropertyIdentifier::MAXIMUM_PACKET_SIZE:
          RETURN_IF_ERROR(m_buffer->parseUint32(m_maximum_packet_size));
          if (m_maximum_packet_size==0) RETURN_ERROR(illegal_byte_sequence);
          break;
        case PropertyIdentifier::TOPIC_ALIAS_MAXIMUM:
          RETURN_IF_ERROR(m_buffer->parseUint16(m_topic_alias_maximum));
          break;
        case PropertyIdentifier::REQUEST_RESPONSE_INFORMATION:
          RETURN_IF_ERROR(m_buffer->parseUint8(m_request_response_information));
          if (m_request_response_information>1) RETURN_ERROR(illegal_byte_sequence);
          break;
        case PropertyIdentifier::REQUEST_PROBLEM_INFORMATION:
          RETURN_IF_ERROR(m_buffer->parseUint8(m_request_problem_information));
          if (m_request_problem_information>1) RETURN_ERROR(illegal_byte_sequence);
          break;
        case PropertyIdentifier::USER_PROPERTY:
        {
          std::string key, value;
          RETURN_IF_ERROR(m_buffer->parseString(key));
          RETURN_IF_ERROR(m_buffer->parseString(value));
          m_user_properties.emplace(key, value);
          break;
        }
        case PropertyIdentifier::AUTHENTICATION_METHOD:
          RETURN_IF_ERROR(m_buffer->parseString(m_authentication_method));
          break;
        case PropertyIdentifier::AUTHENTICATION_DATA:
          RETURN_IF_ERROR(m_buffer->parseBinaryData(m_authentication_data));
          break;

        default: RETURN_ERROR(illegal_byte_sequence);
      }
    }
  }

  RETURN_IF_ERROR(m_buffer->parseString(m_client_id));

  if (m_will_flag==1)
  {
    // 3.1.3.2.1 Property Length
    uint32_t will_property_length;
    RETURN_IF_ERROR(m_buffer->parseVariableByteInteger(will_property_length));

    uint32_t will_property_end = m_buffer->getParsePos() + will_property_length;

    m_will_user_properties.clear();
    while (m_buffer->getParsePos() < will_property_end)
    {
      uint8_t property_identifier;
      RETURN_IF_ERROR(m_buffer->parseUint8(property_identifier));

      switch(property_identifier)
      {
        case PropertyIdentifier::WILL_DELAY_INTERVAL:
          RETURN_IF_ERROR(m_buffer->parseUint32(m_will_delay_interval));
          break;
        case PropertyIdentifier::PAYLOAD_FORMAT_INDICATOR:
          RETURN_IF_ERROR(m_buffer->parseUint8(m_payload_format_indicator));
          if (m_payload_format_indicator>1) RETURN_ERROR(illegal_byte_sequence);
          break;
        case PropertyIdentifier::MESSAGE_EXPIRY_INTERVAL:
          RETURN_IF_ERROR(m_buffer->parseUint32(m_message_expiry_interval));
          break;
        case PropertyIdentifier::CONTENT_TYPE:
          RETURN_IF_ERROR(m_buffer->parseString(m_content_type));
          break;
        case PropertyIdentifier::RESPONSE_TOPIC:
          RETURN_IF_ERROR(m_buffer->parseString(m_response_topic));
          break;
        case PropertyIdentifier::CORRELATION_DATA:
          RETURN_IF_ERROR(m_buffer->parseBinaryData(m_correlation_data));
          break;
        case PropertyIdentifier::USER_PROPERTY:
        {
          std::string key, value;
          RETURN_IF_ERROR(m_buffer->parseString(key));
          RETURN_IF_ERROR(m_buffer->parseString(value));
          m_will_user_properties.emplace_back(std::pair<std::string,std::string>(key, value));
          break;
        }

        default: RETURN_ERROR(illegal_byte_sequence);
      }
    }

    RETURN_IF_ERROR(m_buffer->parseString(m_will_topic));
    RETURN_IF_ERROR(m_buffer->parseBinaryData(m_will_payload));
  }

  if (m_username_flag==1)
  {
    RETURN_IF_ERROR(m_buffer->parseString(m_username));
  }

  if (m_password_flag==1)
  {
    RETURN_IF_ERROR(m_buffer->parseBinaryData(m_password));
  }

  return actions();
}

std::error_code ConnectPacket::actions() // 3.1.4 CONNECT Actions
{
  //TODO

  return std::error_code();
}
