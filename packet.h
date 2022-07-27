#ifndef _PACKET_H_
#define _PACKET_H_

#include <memory>
#include <stddef.h>
#include <stdint.h>


class BasePacket
{
protected:
  BasePacket();
  virtual ~BasePacket() = default;

public:
  [[nodiscard]] static std::shared_ptr<BasePacket> createPacket(const uint8_t* buffer, size_t length, size_t& fixed_header_length, size_t& total_length);
  [[nodiscard]] virtual bool parse([[maybe_unused]] const uint8_t* buffer, [[maybe_unused]] size_t length) {return setHasError();} //Consider it an error if parsing gets here and not to any of the subclasses

protected:
  [[nodiscard]] bool parseVariableByteInteger(const uint8_t* buffer, size_t length, size_t& parsed_length, uint32_t& value);

public:
  bool setHasError() {m_has_error = true; return false;}
  [[nodiscard]] bool hasError() const {return m_has_error;}

private:
  bool m_has_error;
};


class ReservedPacket : public BasePacket
{
public:
  ReservedPacket() : BasePacket() {setHasError();}
  virtual ~ReservedPacket() = default;

  [[nodiscard]] virtual bool parse([[maybe_unused]] const uint8_t* buffer, [[maybe_unused]] size_t length) override {return false;}
};


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


class ConnAckPacket : public BasePacket
{
public:
  ConnAckPacket() : BasePacket() {}
  virtual ~ConnAckPacket() = default;

  [[nodiscard]] virtual bool parse([[maybe_unused]] const uint8_t* buffer, [[maybe_unused]] size_t length) override {return false;}
};


class PublishPacket : public BasePacket
{
public:
  PublishPacket() : BasePacket() {}
  virtual ~PublishPacket() = default;

  [[nodiscard]] virtual bool parse([[maybe_unused]] const uint8_t* buffer, [[maybe_unused]] size_t length) override {return false;}
};


class PubAckPacket : public BasePacket
{
public:
  PubAckPacket() : BasePacket() {}
  virtual ~PubAckPacket() = default;

  [[nodiscard]] virtual bool parse([[maybe_unused]] const uint8_t* buffer, [[maybe_unused]] size_t length) override {return false;}
};


class PubRecPacket : public BasePacket
{
public:
  PubRecPacket() : BasePacket() {}
  virtual ~PubRecPacket() = default;

  [[nodiscard]] virtual bool parse([[maybe_unused]] const uint8_t* buffer, [[maybe_unused]] size_t length) override {return false;}
};


class PubRelPacket : public BasePacket
{
public:
  PubRelPacket() : BasePacket() {}
  virtual ~PubRelPacket() = default;

  [[nodiscard]] virtual bool parse([[maybe_unused]] const uint8_t* buffer, [[maybe_unused]] size_t length) override {return false;}
};


class PubCompPacket : public BasePacket
{
public:
  PubCompPacket() : BasePacket() {}
  virtual ~PubCompPacket() = default;

  [[nodiscard]] virtual bool parse([[maybe_unused]] const uint8_t* buffer, [[maybe_unused]] size_t length) override {return false;}
};


class SubscribePacket : public BasePacket
{
public:
  SubscribePacket() : BasePacket() {}
  virtual ~SubscribePacket() = default;

  [[nodiscard]] virtual bool parse([[maybe_unused]] const uint8_t* buffer, [[maybe_unused]] size_t length) override {return false;}
};


class SubAckPacket : public BasePacket
{
public:
  SubAckPacket() : BasePacket() {}
  virtual ~SubAckPacket() = default;

  [[nodiscard]] virtual bool parse([[maybe_unused]] const uint8_t* buffer, [[maybe_unused]] size_t length) override {return false;}
};


class UnsubscribePacket : public BasePacket
{
public:
  UnsubscribePacket() : BasePacket() {}
  virtual ~UnsubscribePacket() = default;

  [[nodiscard]] virtual bool parse([[maybe_unused]] const uint8_t* buffer, [[maybe_unused]] size_t length) override {return false;}
};


class UnsubAckPacket : public BasePacket
{
public:
  UnsubAckPacket() : BasePacket() {}
  virtual ~UnsubAckPacket() = default;

  [[nodiscard]] virtual bool parse([[maybe_unused]] const uint8_t* buffer, [[maybe_unused]] size_t length) override {return false;}
};


class PingReqPacket : public BasePacket
{
public:
  PingReqPacket() : BasePacket() {}
  virtual ~PingReqPacket() = default;

  [[nodiscard]] virtual bool parse([[maybe_unused]] const uint8_t* buffer, [[maybe_unused]] size_t length) override {return false;}
};


class PingRespPacket : public BasePacket
{
public:
  PingRespPacket() : BasePacket() {}
  virtual ~PingRespPacket() = default;

  [[nodiscard]] virtual bool parse([[maybe_unused]] const uint8_t* buffer, [[maybe_unused]] size_t length) override {return false;}
};


class DisconnectPacket : public BasePacket
{
public:
  DisconnectPacket() : BasePacket() {}
  virtual ~DisconnectPacket() = default;

  [[nodiscard]] virtual bool parse([[maybe_unused]] const uint8_t* buffer, [[maybe_unused]] size_t length) override {return false;}
};


class AuthPacket : public BasePacket
{
public:
  AuthPacket() : BasePacket() {}
  virtual ~AuthPacket() = default;

  [[nodiscard]] virtual bool parse([[maybe_unused]] const uint8_t* buffer, [[maybe_unused]] size_t length) override {return false;}
};

#endif // _PACKET_H_
