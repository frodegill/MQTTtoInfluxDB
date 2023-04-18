#include "buffer.h"


Buffer::Buffer(asio::ip::tcp::socket& socket) noexcept
: m_socket(&socket),
  m_capacity(0L),
  m_length(0L),
  m_parse_pos(0L)
{
}

std::error_code Buffer::grow(size_t capacity)
{
  if (capacity > m_capacity)
  {
    std::unique_ptr databuffer = std::make_unique<uint8_t[]>(capacity);
    if (!databuffer)
      return std::make_error_code(std::errc::not_enough_memory);

    if (m_databuffer && m_length>0)
    {
      std::memcpy(databuffer.get(), m_databuffer.get(), m_length);
    }
    m_databuffer = std::move(databuffer);
    m_capacity = capacity;
  }

  return std::error_code();
}

void Buffer::clear()
{
  m_length = 0;
}

std::error_code Buffer::read(size_t length)
{
  clear();
  return append(length);
}

std::error_code Buffer::append(size_t length)
{
  if (!m_socket)
  {
    return std::make_error_code(std::errc::not_a_socket);
  }

  std::error_code error_code;
  try
  {
    if (IS_OK(error_code=grow(m_length+length)))
    {
      m_length += m_socket->read_some(asio::buffer(m_databuffer.get()+m_length, m_capacity-m_length), error_code);
    }

    return error_code;
  }
  catch (std::exception& e)
  {
    return std::make_error_code(std::errc::io_error);
  }
}


// If returning OK, parsed_length will be incremented by the number of bytes consumed
std::error_code Buffer::parseString(const uint8_t* buffer, size_t length, size_t& parse_pos, std::string& value)
{
  size_t local_parse_pos = parse_pos;
  uint16_t string_length;
  std::error_code error_code;
  if (IS_ERROR(error_code=parseUint16(buffer, length, local_parse_pos, string_length)))
  {
    value = "";
    return error_code;
  }

  if (local_parse_pos+string_length > length)
  {
    value = "";
    return std::make_error_code(std::errc::message_size);
  }

  if (string_length == 0)
  {
    value = "";
  }
  else
  {
    value.assign(reinterpret_cast<const char*>(buffer+local_parse_pos), string_length);
  }

  parse_pos = local_parse_pos + string_length;
  return error_code;
}

// If returning OK, parsed_length will be incremented by the number of bytes consumed
std::error_code Buffer::parseUint8(const uint8_t* buffer, size_t length, size_t& parse_pos, uint8_t& value)
{
  if (parse_pos+1 >= length)
  {
    value = 0;
    return std::make_error_code(std::errc::message_size);
  }

  value = buffer[parse_pos];
  parse_pos++;
  return std::error_code();
}

// If returning OK, parsed_length will be incremented by the number of bytes consumed
std::error_code Buffer::parseUint16(const uint8_t* buffer, size_t length, size_t& parse_pos, uint16_t& value)
{
  if (parse_pos+2 >= length)
  {
    value = 0;
    return std::make_error_code(std::errc::message_size);
  }

  value = buffer[parse_pos]<<8 | buffer[parse_pos+1];
  parse_pos += 2;
  return std::error_code();
}

// If returning OK, parsed_length will be incremented by the number of bytes consumed
std::error_code Buffer::parseUint32(const uint8_t* buffer, size_t length, size_t& parse_pos, uint32_t& value)
{
  if (parse_pos+4 >= length)
  {
    value = 0;
    return std::make_error_code(std::errc::message_size);
  }

  value = buffer[parse_pos]<<24 | buffer[parse_pos+1]<<16 | buffer[parse_pos+2]<<8 | buffer[parse_pos+3];
  parse_pos += 4;
  return std::error_code();
}

// If returning OK, parsed_length will be incremented by the number of bytes consumed
std::error_code Buffer::parseVariableByteInteger(const uint8_t* buffer, size_t length, size_t& parse_pos, uint32_t& value)
{
  value = 0;
  uint32_t multiplier = 1;
  uint8_t encoded_byte;
  // Algorithm from 1.5.5, Variable Byte Integer
  size_t offset = 0;
  do
  {
    if ((parse_pos+offset) >= length)
    {
      value = 0;
      return std::make_error_code(std::errc::message_size);
    }

    encoded_byte = buffer[parse_pos + offset++];
    value += (encoded_byte & 0x7F) * multiplier;

    if (multiplier > 128*128*128)
    {
      value = 0;
      return std::make_error_code(std::errc::illegal_byte_sequence);
    }

    multiplier *= 128;
  }
  while ((encoded_byte & 0x80) != 0);

  parse_pos += offset;
  return std::error_code();
}

// If returning OK, parsed_length will be incremented by the number of bytes consumed
std::error_code Buffer::parseBinaryData(const uint8_t* buffer, size_t length, size_t& parse_pos, std::shared_ptr<uint8_t[]>& value)
{
  size_t local_parse_pos = parse_pos;
  uint16_t data_length;
  std::error_code error_code;
  if (IS_ERROR(error_code=parseUint16(buffer, length, local_parse_pos, data_length)))
    return error_code;

  if (local_parse_pos+data_length > length)
  {
    return std::make_error_code(std::errc::message_size);
  }

  value = std::shared_ptr<uint8_t[]>(new uint8_t[data_length]);
  std::memcpy(value.get(), buffer+local_parse_pos, data_length);
  parse_pos = local_parse_pos + data_length;
  return error_code;
}
