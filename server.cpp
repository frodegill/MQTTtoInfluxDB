#include "server.h"

#include <iostream>

#include "packet.h"


Server::Server(const Properties& properties)
{
  asio::signal_set signals(m_io_context, SIGINT, SIGTERM);
  signals.async_wait([&](auto, auto) {m_io_context.stop();} );

  m_acceptor = std::make_unique<asio::ip::tcp::acceptor>(m_io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), properties.getSettings().mqtt_port));
}

void Server::session(asio::ip::tcp::socket socket)
{
  const size_t MAX_LENGTH = 20;
  std::shared_ptr<uint8_t[]> databuffer(new uint8_t[MAX_LENGTH]);
  size_t length;
  size_t fixed_header_length;
  size_t total_length;
  std::error_code error;
  while (true)
  {
    try
    {
      length = socket.read_some(asio::buffer(databuffer.get(), MAX_LENGTH), error);
      if (error)
        break;

      std::shared_ptr<BasePacket> packet = BasePacket::createPacket(databuffer.get(), length, fixed_header_length, total_length);
      if (packet->hasError())
        break;

      if (length >= total_length)
      {
        if (!packet->parse(databuffer.get(), length))
        {
          break; //Error
        }
      }
      else
      {
        std::shared_ptr<uint8_t[]> large_databuffer(new uint8_t[total_length]);
        if (!large_databuffer.get())
          break; //OOM

        std::memcpy(large_databuffer.get(), databuffer.get(), length);
        length += asio::read(socket, asio::buffer(large_databuffer.get()+length, total_length-length), error);
        if (error || (length < total_length))
          break;

        if (!packet->parse(large_databuffer.get(), length))
        {
          break; //Error
        }
      }

      //TODO Handle packet
    }
    catch (std::exception& e)
    {
      std::cerr << "Exception in thread: " << e.what() << "\n";
    }
  }
}

void Server::run()
{
  while (true)
  {
    asio::ip::tcp::socket socket(m_io_context);
    m_acceptor->accept(socket);
    std::thread(Server::session, std::move(socket)).detach();
  }
}
