#ifndef _PACKET_H_
#define _PACKET_H_

#include <memory>
#include <stddef.h>
#include <stdint.h>
#include <string>


enum PropertyIdentifier
{
  PAYLOAD_FORMAT_INDICATOR = 0x01,
  MESSAGE_EXPIRY_INTERVAL = 0x02,
  CONTENT_TYPE = 0x03,
  RESPONSE_TOPIC = 0x08,
  CORRELATION_DATA = 0x09,
  SUBSCRIPTION_IDENTIFIER = 0x0B,
  SESSION_EXPIRY_INTERVAL = 0x11,
  ASSIGNED_CLIENT_IDENTIFIER = 0x12,
  SERVER_KEEP_ALIVE = 0x13,
  AUTHENTICATION_METHOD = 0x15,
  AUTHENTICATION_DATA = 0x16,
  REQUEST_PROBLEM_INFORMATION = 0x17,
  WILL_DELAY_INTERVAL = 0x18,
  REQUEST_RESPONSE_INFORMATION = 0x19,
  RESPONSE_INFORMATION = 0x1A,
  SERVER_REFERENCE = 0x1C,
  REASON_STRING = 0x1F,
  RECEIVE_MAXIMUM = 0x21,
  TOPIC_ALIAS_MAXIMUM = 0x22,
  TPOIC_ALIAS = 0x23,
  MAXIMUM_QOS = 0x24,
  RETAIN_AVAILABLE = 0x25,
  USER_PROPERTY = 0x26,
  MAXIMUM_PACKET_SIZE = 0x27,
  WILDCARD_SUBSCRIPTION_AVAILABLE = 0x28,
  SUBSCRIPTION_IDENTIFIER_AVAILABLE = 0x29,
  SHARED_SUBSCRIPTION_AVAILABLE = 0x2A
};

class BasePacket
{
protected:
  BasePacket();
  virtual ~BasePacket() = default;

public:
  [[nodiscard]] static std::shared_ptr<BasePacket> createPacket(const uint8_t* buffer, size_t length, size_t& fixed_header_length, size_t& total_length);
  [[nodiscard]] virtual bool parse([[maybe_unused]] const uint8_t* buffer, [[maybe_unused]] size_t length) {return setHasError();} //Consider it an error if parsing gets here and not to any of the subclasses

protected:
  [[nodiscard]] bool parseString(const uint8_t* buffer, size_t length, size_t& parsed_length, std::string& value);
  [[nodiscard]] bool parseUint8(const uint8_t* buffer, size_t length, size_t& parsed_length, uint8_t& value);
  [[nodiscard]] bool parseUint16(const uint8_t* buffer, size_t length, size_t& parsed_length, uint16_t& value);
  [[nodiscard]] bool parseUint32(const uint8_t* buffer, size_t length, size_t& parsed_length, uint32_t& value);
  [[nodiscard]] bool parseVariableByteInteger(const uint8_t* buffer, size_t length, size_t& parsed_length, uint32_t& value);
  [[nodiscard]] bool parseBinaryData(const uint8_t* buffer, size_t length, size_t& parsed_length, std::shared_ptr<uint8_t[]>& value);

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
