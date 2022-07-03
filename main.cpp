#include "properties.h"
#include "server.h"

#include <iostream>


int main(int /*argc*/, char */*argv*/[])
{
  Properties properties;
  if (!properties.loadFile())
  {
    return EXIT_FAILURE;
  }
  Server server(properties);
  server.run();

  return EXIT_SUCCESS;
}
