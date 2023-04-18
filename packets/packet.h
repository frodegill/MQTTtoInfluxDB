#ifndef _PACKET_H_
#define _PACKET_H_

#include <memory>
#include <stddef.h>
#include <stdint.h>
#include <string>

#include "../buffer.h"

#define RETURN_IF_ERROR(x) if(IS_ERROR(error_code=x)){return setHasError(error_code);}
#define RETURN_ERROR(x)    return setHasError(std::make_error_code(std::errc::x));

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
  BasePacket(std::unique_ptr<Buffer> buffer);
  virtual ~BasePacket() = default;

public:
  [[nodiscard]] static std::error_code createPacket(asio::ip::tcp::socket& socket, std::shared_ptr<BasePacket>& packet, size_t& fixed_header_length, size_t& total_length);
  [[nodiscard]] virtual std::error_code parse() {return setHasError(std::make_error_code(std::errc::operation_not_permitted));}

public:
  std::error_code setHasError(std::error_code error_code) {m_error_code=error_code; return m_error_code;}
  [[nodiscard]] std::error_code getError() const {return m_error_code;}

protected:
  std::unique_ptr<Buffer> m_buffer;
  std::error_code m_error_code;
};


class ReservedPacket : public BasePacket
{
public:
  ReservedPacket(std::unique_ptr<Buffer> buffer) : BasePacket(std::move(buffer)) {setHasError(std::make_error_code(std::errc::operation_not_permitted));}
  virtual ~ReservedPacket() = default;

  [[nodiscard]] virtual std::error_code parse() {return setHasError(std::make_error_code(std::errc::function_not_supported));}
};


class ConnAckPacket : public BasePacket
{
public:
  ConnAckPacket(std::unique_ptr<Buffer> buffer) : BasePacket(std::move(buffer)) {}
  virtual ~ConnAckPacket() = default;

  [[nodiscard]] virtual std::error_code parse() {return setHasError(std::make_error_code(std::errc::function_not_supported));}
};


class PublishPacket : public BasePacket
{
public:
  PublishPacket(std::unique_ptr<Buffer> buffer) : BasePacket(std::move(buffer)) {}
  virtual ~PublishPacket() = default;

  [[nodiscard]] virtual std::error_code parse() {return setHasError(std::make_error_code(std::errc::function_not_supported));}
};


class PubAckPacket : public BasePacket
{
public:
  PubAckPacket(std::unique_ptr<Buffer> buffer) : BasePacket(std::move(buffer)) {}
  virtual ~PubAckPacket() = default;

  [[nodiscard]] virtual std::error_code parse() {return setHasError(std::make_error_code(std::errc::function_not_supported));}
};


class PubRecPacket : public BasePacket
{
public:
  PubRecPacket(std::unique_ptr<Buffer> buffer) : BasePacket(std::move(buffer)) {}
  virtual ~PubRecPacket() = default;

  [[nodiscard]] virtual std::error_code parse() {return setHasError(std::make_error_code(std::errc::function_not_supported));}
};


class PubRelPacket : public BasePacket
{
public:
  PubRelPacket(std::unique_ptr<Buffer> buffer) : BasePacket(std::move(buffer)) {}
  virtual ~PubRelPacket() = default;

  [[nodiscard]] virtual std::error_code parse() {return setHasError(std::make_error_code(std::errc::function_not_supported));}
};


class PubCompPacket : public BasePacket
{
public:
  PubCompPacket(std::unique_ptr<Buffer> buffer) : BasePacket(std::move(buffer)) {}
  virtual ~PubCompPacket() = default;

  [[nodiscard]] virtual std::error_code parse() {return setHasError(std::make_error_code(std::errc::function_not_supported));}
};


class SubscribePacket : public BasePacket
{
public:
  SubscribePacket(std::unique_ptr<Buffer> buffer) : BasePacket(std::move(buffer)) {}
  virtual ~SubscribePacket() = default;

  [[nodiscard]] virtual std::error_code parse() {return setHasError(std::make_error_code(std::errc::function_not_supported));}
};


class SubAckPacket : public BasePacket
{
public:
  SubAckPacket(std::unique_ptr<Buffer> buffer) : BasePacket(std::move(buffer)) {}
  virtual ~SubAckPacket() = default;

  [[nodiscard]] virtual std::error_code parse() {return setHasError(std::make_error_code(std::errc::function_not_supported));}
};


class UnsubscribePacket : public BasePacket
{
public:
  UnsubscribePacket(std::unique_ptr<Buffer> buffer) : BasePacket(std::move(buffer)) {}
  virtual ~UnsubscribePacket() = default;

  [[nodiscard]] virtual std::error_code parse() {return setHasError(std::make_error_code(std::errc::function_not_supported));}
};


class UnsubAckPacket : public BasePacket
{
public:
  UnsubAckPacket(std::unique_ptr<Buffer> buffer) : BasePacket(std::move(buffer)) {}
  virtual ~UnsubAckPacket() = default;

  [[nodiscard]] virtual std::error_code parse() {return setHasError(std::make_error_code(std::errc::function_not_supported));}
};


class PingReqPacket : public BasePacket
{
public:
  PingReqPacket(std::unique_ptr<Buffer> buffer) : BasePacket(std::move(buffer)) {}
  virtual ~PingReqPacket() = default;

  [[nodiscard]] virtual std::error_code parse() {return setHasError(std::make_error_code(std::errc::function_not_supported));}
};


class PingRespPacket : public BasePacket
{
public:
  PingRespPacket(std::unique_ptr<Buffer> buffer) : BasePacket(std::move(buffer)) {}
  virtual ~PingRespPacket() = default;

  [[nodiscard]] virtual std::error_code parse() {return setHasError(std::make_error_code(std::errc::function_not_supported));}
};


class DisconnectPacket : public BasePacket
{
public:
  DisconnectPacket(std::unique_ptr<Buffer> buffer) : BasePacket(std::move(buffer)) {}
  virtual ~DisconnectPacket() = default;

  [[nodiscard]] virtual std::error_code parse() {return setHasError(std::make_error_code(std::errc::function_not_supported));}
};


class AuthPacket : public BasePacket
{
public:
  AuthPacket(std::unique_ptr<Buffer> buffer) : BasePacket(std::move(buffer)) {}
  virtual ~AuthPacket() = default;

  [[nodiscard]] virtual std::error_code parse() {return setHasError(std::make_error_code(std::errc::function_not_supported));}
};

#endif // _PACKET_H_
