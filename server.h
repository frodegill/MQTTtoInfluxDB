#ifndef _SERVER_H_
#define _SERVER_H_

#include <asio.hpp>
#include <memory>

#include "properties.h"


class Server
{
public:
  Server(const Properties& properties);

private:
  static void session(asio::ip::tcp::socket socket);

public:
  void run();

private:
  asio::io_context m_io_context;
  std::unique_ptr<asio::ip::tcp::acceptor> m_acceptor;
};

#endif // _SERVER_H_
