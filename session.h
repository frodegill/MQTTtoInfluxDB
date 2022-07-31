#ifndef _SESSION_H_
#define _SESSION_H_

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>


class Session
{
};


class SessionManager
{
private:
  static constexpr char LEGAL_CLIENT_ID_CHARS[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

public:
  void expireOldSessions();
  void expireSession(const std::string& client_id);
  [[nodiscard]] bool createSession(std::string& client_id, std::shared_ptr<Session>& session);

  void generateClientId(std::string& client_id);

private:
  std::mutex m_session_map_lock;
  std::unordered_map<std::string,std::shared_ptr<Session>> m_session_map;

};

#endif // _SESSION_H_
