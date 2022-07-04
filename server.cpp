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
  const size_t MAX_LENGTH = 1024;
  uint8_t data[MAX_LENGTH];
  size_t length;
  std::error_code error;
  try
  {
    while (true)
    {
      std::shared_ptr<BasePacket> packet;

      do
      {
        length = socket.read_some(asio::buffer(data), error);
        if (error)
          break;

        if (!packet.get())
        {
          packet = BasePacket::createPacket(data, length);
        }
        size_t read_bytes = packet->parse(data, length);
        if (read_bytes != length)
        {
        }
      }
      while (length>0 && !packet->hasError() && !packet->isCompletelyParsed());
    }
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception in thread: " << e.what() << "\n";
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
