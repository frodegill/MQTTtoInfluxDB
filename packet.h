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
  [[nodiscard]] static std::shared_ptr<BasePacket> createPacket(const uint8_t* buffer, size_t length);
  [[nodiscard]] virtual size_t parse([[maybe_unused]] const uint8_t* buffer, size_t length) {setHasError(); return length;} //Consider it an error if parsing gets here and not to any of the subclasses

public:
  void setHasError() {m_has_error = true;}
  void setIsCompletelyParsed() {m_is_completely_parsed = true;}
  [[nodiscard]] bool hasError() const {return m_has_error;}
  [[nodiscard]] bool isCompletelyParsed() const {return m_is_completely_parsed;}

private:
  bool m_has_error;
  bool m_is_completely_parsed;
};


class ReservedPacket : public BasePacket
{
public:
  ReservedPacket() : BasePacket() {setHasError();}
  virtual ~ReservedPacket() = default;

  [[nodiscard]] virtual size_t parse([[maybe_unused]] const uint8_t* buffer, size_t length) override {return length;}
};


class ConnectPacket : public BasePacket
{
public:
  ConnectPacket() : BasePacket() {}
  virtual ~ConnectPacket() = default;

  [[nodiscard]] virtual size_t parse([[maybe_unused]] const uint8_t* buffer, size_t length) override;
};


class ConnAckPacket : public BasePacket
{
public:
  ConnAckPacket() : BasePacket() {}
  virtual ~ConnAckPacket() = default;

  [[nodiscard]] virtual size_t parse([[maybe_unused]] const uint8_t* buffer, size_t length) override {return length;}
};


class PublishPacket : public BasePacket
{
public:
  PublishPacket() : BasePacket() {}
  virtual ~PublishPacket() = default;

  [[nodiscard]] virtual size_t parse([[maybe_unused]] const uint8_t* buffer, size_t length) override {return length;}
};


class PubAckPacket : public BasePacket
{
public:
  PubAckPacket() : BasePacket() {}
  virtual ~PubAckPacket() = default;

  [[nodiscard]] virtual size_t parse([[maybe_unused]] const uint8_t* buffer, size_t length) override {return length;}
};


class PubRecPacket : public BasePacket
{
public:
  PubRecPacket() : BasePacket() {}
  virtual ~PubRecPacket() = default;

  [[nodiscard]] virtual size_t parse([[maybe_unused]] const uint8_t* buffer, size_t length) override {return length;}
};


class PubRelPacket : public BasePacket
{
public:
  PubRelPacket() : BasePacket() {}
  virtual ~PubRelPacket() = default;

  [[nodiscard]] virtual size_t parse([[maybe_unused]] const uint8_t* buffer, size_t length) override {return length;}
};


class PubCompPacket : public BasePacket
{
public:
  PubCompPacket() : BasePacket() {}
  virtual ~PubCompPacket() = default;

  [[nodiscard]] virtual size_t parse([[maybe_unused]] const uint8_t* buffer, size_t length) override {return length;}
};


class SubscribePacket : public BasePacket
{
public:
  SubscribePacket() : BasePacket() {}
  virtual ~SubscribePacket() = default;

  [[nodiscard]] virtual size_t parse([[maybe_unused]] const uint8_t* buffer, size_t length) override {return length;}
};


class SubAckPacket : public BasePacket
{
public:
  SubAckPacket() : BasePacket() {}
  virtual ~SubAckPacket() = default;

  [[nodiscard]] virtual size_t parse([[maybe_unused]] const uint8_t* buffer, size_t length) override {return length;}
};


class UnsubscribePacket : public BasePacket
{
public:
  UnsubscribePacket() : BasePacket() {}
  virtual ~UnsubscribePacket() = default;

  [[nodiscard]] virtual size_t parse([[maybe_unused]] const uint8_t* buffer, size_t length) override {return length;}
};


class UnsubAckPacket : public BasePacket
{
public:
  UnsubAckPacket() : BasePacket() {}
  virtual ~UnsubAckPacket() = default;

  [[nodiscard]] virtual size_t parse([[maybe_unused]] const uint8_t* buffer, size_t length) override {return length;}
};


class PingReqPacket : public BasePacket
{
public:
  PingReqPacket() : BasePacket() {}
  virtual ~PingReqPacket() = default;

  [[nodiscard]] virtual size_t parse([[maybe_unused]] const uint8_t* buffer, size_t length) override {return length;}
};


class PingRespPacket : public BasePacket
{
public:
  PingRespPacket() : BasePacket() {}
  virtual ~PingRespPacket() = default;

  [[nodiscard]] virtual size_t parse([[maybe_unused]] const uint8_t* buffer, size_t length) override {return length;}
};


class DisconnectPacket : public BasePacket
{
public:
  DisconnectPacket() : BasePacket() {}
  virtual ~DisconnectPacket() = default;

  [[nodiscard]] virtual size_t parse([[maybe_unused]] const uint8_t* buffer, size_t length) override {return length;}
};


class AuthPacket : public BasePacket
{
public:
  AuthPacket() : BasePacket() {}
  virtual ~AuthPacket() = default;

  [[nodiscard]] virtual size_t parse([[maybe_unused]] const uint8_t* buffer, size_t length) override {return length;}
};

#endif // _PACKET_H_
