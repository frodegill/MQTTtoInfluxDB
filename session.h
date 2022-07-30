#ifndef _SESSION_H_
#define _SESSION_H_

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

  void generateClientId(std::string& client_id) const;

private:
  std::unordered_map<std::string,Session> m_session_map;

};

#endif // _SESSION_H_
