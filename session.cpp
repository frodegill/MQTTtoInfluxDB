#include "session.h"

#include <algorithm>
#include <cmath>
#include <random>


void Session::disconnect()
{
}


void SessionManager::expireOldSessions()
{
}

void SessionManager::expireSession(const std::string& client_id)
{
  {
    std::lock_guard<std::mutex> lock(m_session_map_lock);
    auto iter = m_session_map.find(client_id);
    if (iter != m_session_map.end())
    {
      std::shared_ptr<Session> session = iter->second;
      session->disconnect();
      m_session_map.erase(client_id);
    }
  }
}

bool SessionManager::createSession(std::string& client_id, std::shared_ptr<Session>& session)
{
  {
    std::lock_guard<std::mutex> lock(m_session_map_lock);
    if (client_id.empty())
      generateClientId(client_id);

    if (m_session_map.contains(client_id))
      return false;

    session = std::make_shared<Session>();
    m_session_map.emplace(client_id, session);
    return true;
  }
}

void SessionManager::generateClientId(std::string& client_id)
{
  int required_id_length = std::min(static_cast<int>(::log10(m_session_map.size()+1)) + 2, 23);
  constexpr int LEGAL_CLIENT_ID_CHARS_length = sizeof(LEGAL_CLIENT_ID_CHARS)/sizeof(LEGAL_CLIENT_ID_CHARS[0]) - 1;

  std::random_device random_device;
  std::default_random_engine random_engine(random_device());
  std::uniform_int_distribution<int> random_provider(0, LEGAL_CLIENT_ID_CHARS_length);

  {
    std::lock_guard<std::mutex> lock(m_session_map_lock);
    do
    {
      client_id.clear();
      for (int i=0; i<required_id_length; i++)
      {
        client_id += LEGAL_CLIENT_ID_CHARS[random_provider(random_engine)];
      }
    } while (m_session_map.contains(client_id));
  }
}
