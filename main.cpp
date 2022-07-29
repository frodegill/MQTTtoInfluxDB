#include <iostream>

#include "properties.h"
#include "server.h"
#include "session.h"


std::shared_ptr<SessionManager> g_session_manager;
std::shared_ptr<SessionManager> getSessionManager() {return g_session_manager;}


int main(int /*argc*/, char */*argv*/[])
{
  g_session_manager = std::make_shared<SessionManager>();

  Properties properties;
  if (!properties.loadFile())
  {
    return EXIT_FAILURE;
  }
  Server server(properties);
  server.run();

  return EXIT_SUCCESS;
}
