#include "packet.h"

#include <cstring>
#include <iostream>

#include "packet_connect.h"

#include "../main.h"
#include "../session.h"


/*
 * Any documentation references below, references the document https://docs.oasis-open.org/mqtt/mqtt/v5.0/os/mqtt-v5.0-os.html
 */

BasePacket::BasePacket(std::unique_ptr<Buffer> buffer)
: m_buffer(std::move(buffer))
{
}

std::error_code BasePacket::createPacket(asio::ip::tcp::socket& socket, std::shared_ptr<BasePacket>& packet, size_t& fixed_header_length, size_t& total_length)
{
  packet.reset();
  fixed_header_length = total_length = 0;
  ::getSessionManager()->expireOldSessions();

  std::unique_ptr<Buffer> buffer = std::make_unique<Buffer>(socket);
  if (!buffer)
    return std::make_error_code(std::errc::not_enough_memory);

  std::error_code error_code;
  if (IS_ERROR(error_code=buffer->read()))
    return error_code;

  if (buffer->getReadLength() == 0)
  {
    packet = std::make_shared<ReservedPacket>(std::move(buffer));
    return packet->setHasError(std::make_error_code(std::errc::message_size));
  }

  // 2.1.2, MQTT Control Packet type
  uint8_t fixed_header_control_packet_type;
  uint32_t fixed_header_remaining_length;
  if (IS_ERROR(error_code=buffer->parseUint8(fixed_header_control_packet_type)) ||
      IS_ERROR(error_code=buffer->parseVariableByteInteger(fixed_header_remaining_length)))
  {
    return packet->setHasError(error_code);
  }

  fixed_header_length = buffer->getParsePos();
  total_length = fixed_header_length + fixed_header_remaining_length;

  if (IS_ERROR(error_code=buffer->append(total_length - buffer->getReadLength())))
    return error_code;

  uint8_t control_packet_type_flags = fixed_header_control_packet_type & 0x0F;
  switch((fixed_header_control_packet_type & 0xF0) >> 4)
  {
    default: [[fallthrough]];
    case  0: packet=std::make_shared<ReservedPacket>(std::move(buffer));
             break;
    case  1: packet=std::make_shared<ConnectPacket>(std::move(buffer));
             if (control_packet_type_flags != 0) return packet->setHasError(std::make_error_code(std::errc::illegal_byte_sequence));
             break; //0x10
    case  2: packet=std::make_shared<ConnAckPacket>(std::move(buffer));
             if (control_packet_type_flags != 0) return packet->setHasError(std::make_error_code(std::errc::illegal_byte_sequence));
             break; //0x20
    case  3: packet=std::make_shared<PublishPacket>(std::move(buffer));
             break; //0x3X
    case  4: packet=std::make_shared<PubAckPacket>(std::move(buffer));
             if (control_packet_type_flags != 0) return packet->setHasError(std::make_error_code(std::errc::illegal_byte_sequence));
             break; //0x40
    case  5: packet=std::make_shared<PubRecPacket>(std::move(buffer));
             if (control_packet_type_flags != 0) return packet->setHasError(std::make_error_code(std::errc::illegal_byte_sequence));
             break; //0x50
    case  6: packet=std::make_shared<PubRelPacket>(std::move(buffer));
             if (control_packet_type_flags != 0x02) return packet->setHasError(std::make_error_code(std::errc::illegal_byte_sequence));
             break; //0x62
    case  7: packet=std::make_shared<PubCompPacket>(std::move(buffer));
             if (control_packet_type_flags != 0) return packet->setHasError(std::make_error_code(std::errc::illegal_byte_sequence));
             break; //0x70
    case  8: packet=std::make_shared<SubscribePacket>(std::move(buffer));
             if (control_packet_type_flags != 0x02) return packet->setHasError(std::make_error_code(std::errc::illegal_byte_sequence));
             break; //0x82
    case  9: packet=std::make_shared<SubAckPacket>(std::move(buffer));
             if (control_packet_type_flags != 0) return packet->setHasError(std::make_error_code(std::errc::illegal_byte_sequence));
             break; //0x90
    case 10: packet=std::make_shared<UnsubscribePacket>(std::move(buffer));
             if (control_packet_type_flags != 0x02) return packet->setHasError(std::make_error_code(std::errc::illegal_byte_sequence));
             break; //0xA2
    case 11: packet=std::make_shared<UnsubAckPacket>(std::move(buffer));
             if (control_packet_type_flags != 0) return packet->setHasError(std::make_error_code(std::errc::illegal_byte_sequence));
             break; //0xB0
    case 12: packet=std::make_shared<PingReqPacket>(std::move(buffer));
             if (control_packet_type_flags != 0) return packet->setHasError(std::make_error_code(std::errc::illegal_byte_sequence));
             break; //0xC0
    case 13: packet=std::make_shared<PingRespPacket>(std::move(buffer));
             if (control_packet_type_flags != 0) return packet->setHasError(std::make_error_code(std::errc::illegal_byte_sequence));
             break; //0xD0
    case 14: packet=std::make_shared<DisconnectPacket>(std::move(buffer));
             if (control_packet_type_flags != 0) return packet->setHasError(std::make_error_code(std::errc::illegal_byte_sequence));
             break; //0xE0
    case 15: packet=std::make_shared<AuthPacket>(std::move(buffer));
             if (control_packet_type_flags != 0) return packet->setHasError(std::make_error_code(std::errc::illegal_byte_sequence));
             break; //0xF0
  };

  return error_code;
}
