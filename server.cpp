#include "server.h"

#include <iostream>

#include "packets/packet.h"


Server::Server(const Properties& properties)
{
  asio::signal_set signals(m_io_context, SIGINT, SIGTERM);
  signals.async_wait([&](auto, auto) {m_io_context.stop();} );

  m_acceptor = std::make_unique<asio::ip::tcp::acceptor>(m_io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), properties.getSettings().mqtt_port));
}

void Server::session(asio::ip::tcp::socket socket)
{
  size_t fixed_header_length;
  size_t total_length;
  std::error_code error_code;
  while (true)
  {
    try
    {
      std::shared_ptr<BasePacket> packet;
      if ((error_code=BasePacket::createPacket(socket, packet, fixed_header_length, total_length)))
        break;

      if ((error_code=packet->parse()))
      {
        break; //Error
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
