#ifndef _PACKET_CONNECT_H_
#define _PACKET_CONNECT_H_

#include "packet.h"


class ConnectPacket : public BasePacket
{
public:
  ConnectPacket() : BasePacket(), m_client_version(0),
                                  m_clean_start_flag(0),
                                  m_will_flag(0),
                                  m_will_qos_flag(0),
                                  m_will_retain_flag(0),
                                  m_password_flag(0),
                                  m_username_flag(0),
                                  m_keep_alive(0) {}

  virtual ~ConnectPacket() = default;

  [[nodiscard]] virtual bool parse([[maybe_unused]] const uint8_t* buffer, size_t length) override;

private:
  uint8_t m_client_version;

  bool m_clean_start_flag;
  bool m_will_flag;
  uint8_t m_will_qos_flag;
  bool m_will_retain_flag;
  bool m_password_flag;
  bool m_username_flag;

  uint16_t m_keep_alive;
};

#endif // _PACKET_CONNECT_H_
