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

  std::shared_ptr<BasePacket> packet;
  // MQTT Control Packet type, https://docs.oasis-open.org/mqtt/mqtt/v5.0/os/mqtt-v5.0-os.html#_Toc3901022
  switch((*buffer & 0xF0) >> 4)
  {
    default: [[fallthrough]];
    case  0: packet = std::make_shared<ReservedPacket>(); break;
    case  1: packet = std::make_shared<ConnectPacket>(); break;
    case  2: packet = std::make_shared<ConnAckPacket>(); break;
    case  3: packet = std::make_shared<PublishPacket>(); break;
    case  4: packet = std::make_shared<PubAckPacket>(); break;
    case  5: packet = std::make_shared<PubRecPacket>(); break;
    case  6: packet = std::make_shared<PubRelPacket>(); break;
    case  7: packet = std::make_shared<PubCompPacket>(); break;
    case  8: packet = std::make_shared<SubscribePacket>(); break;
    case  9: packet = std::make_shared<SubAckPacket>(); break;
    case 10: packet = std::make_shared<UnsubscribePacket>(); break;
    case 11: packet = std::make_shared<UnsubAckPacket>(); break;
    case 12: packet = std::make_shared<PingReqPacket>(); break;
    case 13: packet = std::make_shared<PingRespPacket>(); break;
    case 14: packet = std::make_shared<DisconnectPacket>(); break;
    case 15: packet = std::make_shared<AuthPacket>(); break;
  };

  packet->parse(buffer, length);
  return packet;
}
