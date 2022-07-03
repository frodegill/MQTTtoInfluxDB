#ifndef _PROPERTIES_H_
#define _PROPERTIES_H_

#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>


class Properties
{
  struct Server {
    std::string influxdb_host;
    int influxdb_port = 8086;
    std::string influxdb_database;
    std::string influxdb_username;
    std::string influxdb_password;
    bool influxdb_tls;
  };

  struct Rule {
    std::string source;
    std::shared_ptr<Server> server;
    std::string destination;
  };

  struct Topic {
    std::string match;
    std::vector<Rule> rules;
  };

  struct Settings {
    int mqtt_port = 1883;
    int mqtt_port_tls = 8883;
    std::map<std::string,std::shared_ptr<Server>> servers;
    std::vector<Topic> topics;
  };

  enum Type {
    SETTINGS,
    SERVER,
    RULE
  };

private:
  static constexpr std::string_view FILENAME{"application.properties"};

public:
  Properties();

  [[nodiscard]] const Settings& getSettings() const {return m_settings;}

public:
  [[nodiscard]] bool loadFile();

private:
  Settings m_settings;
};

#endif // _PROPERTIES_H_
