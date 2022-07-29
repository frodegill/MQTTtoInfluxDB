#ifndef _PACKET_CONNECT_H_
#define _PACKET_CONNECT_H_

#include "packet.h"

#include <map>


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
                                  m_keep_alive(0),
                                  m_session_expiry_interval(0),
                                  m_receive_maximum(65535),
                                  m_maximum_packet_size(64*1024), //"It is the responsibility of the application to select a suitable Maximum Packet Size value if it chooses to restrict the Maximum Packet Size"
                                  m_topic_alias_maximum(0),
                                  m_request_response_information(0),
                                  m_request_problem_information(1)
                                  {}

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

  uint32_t m_session_expiry_interval;
  uint16_t m_receive_maximum;
  uint32_t m_maximum_packet_size;
  uint16_t m_topic_alias_maximum;
  uint8_t m_request_response_information;
  uint8_t m_request_problem_information;
  std::multimap<std::string,std::string> m_user_properties;
  std::string m_authentication_method;
  std::shared_ptr<uint8_t[]> m_authentication_data;

};

#endif // _PACKET_CONNECT_H_
