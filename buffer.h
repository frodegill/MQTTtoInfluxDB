#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <asio.hpp>
#include <memory>


class Buffer
{
private:
  static constexpr size_t DEFAULT_LENGTH = 20L;

public:
  Buffer(asio::ip::tcp::socket& socket) noexcept;

private:
  [[nodiscard]] std::error_code grow(size_t capacity);

public:
  void clear();
  [[nodiscard]] std::error_code read(size_t length=DEFAULT_LENGTH);
  [[nodiscard]] std::error_code append(size_t length=DEFAULT_LENGTH);

  [[nodiscard]] size_t getReadLength() const {return m_length;}
  [[nodiscard]] size_t getParsePos() const {return m_parse_pos;}
  [[nodiscard]] size_t getUnparsedLength() const {return m_length-m_parse_pos;}

public:
  [[nodiscard]] std::error_code parseString(std::string& value) {return parseString(m_databuffer.get(), m_length, m_parse_pos, value);}
  [[nodiscard]] std::error_code parseUint8(uint8_t& value) {return parseUint8(m_databuffer.get(), m_length, m_parse_pos, value);}
  [[nodiscard]] std::error_code parseUint16(uint16_t& value) {return parseUint16(m_databuffer.get(), m_length, m_parse_pos, value);}
  [[nodiscard]] std::error_code parseUint32(uint32_t& value) {return parseUint32(m_databuffer.get(), m_length, m_parse_pos, value);}
  [[nodiscard]] std::error_code parseVariableByteInteger(uint32_t& value) {return parseVariableByteInteger(m_databuffer.get(), m_length, m_parse_pos, value);}
  [[nodiscard]] std::error_code parseBinaryData(std::shared_ptr<uint8_t[]>& value) {return parseBinaryData(m_databuffer.get(), m_length, m_parse_pos, value);}
private:
  [[nodiscard]] std::error_code parseString(const uint8_t* buffer, size_t length, size_t& parse_pos, std::string& value);
  [[nodiscard]] std::error_code parseUint8(const uint8_t* buffer, size_t length, size_t& parse_pos, uint8_t& value);
  [[nodiscard]] std::error_code parseUint16(const uint8_t* buffer, size_t length, size_t& parse_pos, uint16_t& value);
  [[nodiscard]] std::error_code parseUint32(const uint8_t* buffer, size_t length, size_t& parse_pos, uint32_t& value);
  [[nodiscard]] std::error_code parseVariableByteInteger(const uint8_t* buffer, size_t length, size_t& parse_pos, uint32_t& value);
  [[nodiscard]] std::error_code parseBinaryData(const uint8_t* buffer, size_t length, size_t& parse_pos, std::shared_ptr<uint8_t[]>& value);

private:
  asio::ip::tcp::socket* m_socket;
  std::unique_ptr<uint8_t[]> m_databuffer;
  size_t m_capacity;
  size_t m_length;

  size_t m_parse_pos;
};

#endif // _BUFFER_H_
