#include "session.h"

#include <algorithm>
#include <cmath>
#include <random>


void SessionManager::expireOldSessions()
{
}

void SessionManager::generateClientId(std::string& client_id) const
{
  int required_id_length = std::min(static_cast<int>(::log10(m_session_map.size()+1)) + 2, 23);
  constexpr int LEGAL_CLIENT_ID_CHARS_length = sizeof(LEGAL_CLIENT_ID_CHARS)/sizeof(LEGAL_CLIENT_ID_CHARS[0]) - 1;

  std::random_device random_device;
  std::default_random_engine random_engine(random_device());
  std::uniform_int_distribution<int> random_provider(0, LEGAL_CLIENT_ID_CHARS_length);

  do
  {
    client_id.clear();
    for (int i=0; i<required_id_length; i++)
    {
      client_id += LEGAL_CLIENT_ID_CHARS[random_provider(random_engine)];
    }
  } while (m_session_map.contains(client_id));
}
