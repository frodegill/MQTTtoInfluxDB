#include "server.h"


Server::Server(const Properties& properties)
{
  m_acceptor = std::make_unique<asio::ip::tcp::acceptor>(m_io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v6(), properties.getSettings().mqtt_port));
}

void Server::run()
{
  m_io_context.run();
}
