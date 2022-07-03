#include "properties.h"

#include <fstream>
#include <iostream>


Properties::Properties()
{
}

bool Properties::loadFile()
{
  std::ifstream input_file(Properties::FILENAME.data());
  std::string line;
  if (!input_file.is_open())
  {
    std::cerr << "Could not open the file " << Properties::FILENAME << "'" << std::endl;
    return false;
  }

  Type current_type = SETTINGS;
  std::string current_server;
  while (std::getline(input_file, line))
  {
    if (line.length()>2 && '['==line.at(0) && ']'==line.at(line.length()-1))
    {
      const std::string id = line.substr(1, line.length()-2);
      if (0 == id.compare("settings"))
      {
        current_type = SETTINGS;
      }
      else if (0 != m_settings.servers.count(id))
      {
        current_type = SERVER;
        current_server = id;
      }
      else
      {
        current_type = RULE;
        m_settings.topics.push_back(Topic());
        m_settings.topics.back().match = id;
      }
    }
    else
    {
      if (SETTINGS == current_type)
      {
        if (0 == line.compare(0, 8, "version="))
        {
          if (1 != std::stoi(line.substr(8)))
          {
            std::cerr << "Unexpected settings version \"" << line << "\"" << std::endl;
          }
        }
        else if (0 == line.compare(0, 10, "mqtt_port="))
        {
          m_settings.mqtt_port = std::stoi(line.substr(10));
        }
        else if (0 == line.compare(0, 14, "mqtt_port_tls="))
        {
          m_settings.mqtt_port_tls = std::stoi(line.substr(14));
        }
        else if (0 == line.compare(0, 8, "servers="))
        {
          const std::string server_list = line.substr(8);
          size_t start=0, end;
          do //Parse ':'-separated string
          {
            end = server_list.find_first_of(':', start);
            const std::string server = server_list.substr(start, end);

            if (0 != m_settings.servers.count(server))
            {
                std::cerr << "Server \"" << server << "\" appears multiple times" << std::endl;
            }

            m_settings.servers.emplace(server, std::make_shared<Server>());

            start = end+1;
          } while (end != std::string::npos);
        }
        else if (line.length() > 0)
        {
          std::cerr << "Unexpected settings line \"" << line << "\"" << std::endl;
        }
      }
      else if (SERVER == current_type)
      {
        auto iter = m_settings.servers.find(current_server);
        if (m_settings.servers.end() != iter)
        {
          if (0 == line.compare(0, 14, "influxdb_host="))
          {
            iter->second->influxdb_host = line.substr(14);
          }
          else if (0 == line.compare(0, 14, "influxdb_port="))
          {
            iter->second->influxdb_port = std::stoi(line.substr(14));
          }
          else if (0 == line.compare(0, 18, "influxdb_database="))
          {
            iter->second->influxdb_database = line.substr(18);
          }
          else if (0 == line.compare(0, 18, "influxdb_username="))
          {
            iter->second->influxdb_username = line.substr(18);
          }
          else if (0 == line.compare(0, 18, "influxdb_password="))
          {
            iter->second->influxdb_password = line.substr(18);
          }
          else if (0 == line.compare(0, 13, "influxdb_tls="))
          {
            iter->second->influxdb_tls = (0 != std::string("disabled").compare(line.substr(13)));
          }
          else if (line.length() > 0)
          {
            std::cerr << "Unexpected server settings line \"" << line << "\"" << std::endl;
          }
        }
      }
      else if (RULE == current_type)
      {
        auto topic = m_settings.topics.back();
        auto index = line.find(" => ");
        if (std::string::npos!=index && 0<index)
        {
          std::string source = line.substr(0, index);
          std::string rest = line.substr(index+4);
          auto server_index = rest.find(":");
          if (std::string::npos!=server_index && 0<server_index)
          {
            std::string server = rest.substr(0, server_index);
            auto server_iter = m_settings.servers.find(server);
            if (m_settings.servers.end() == server_iter)
            {
              std::cerr << "Rule \"" << topic.match << "\" references unknown server \"" << server << "\"" << std::endl;
              return false;
            }

            std::string destination = rest.substr(server_index+1);
            m_settings.topics.back().rules.push_back(Rule());
            m_settings.topics.back().rules.back().source = source;
            m_settings.topics.back().rules.back().server = server_iter->second;
            m_settings.topics.back().rules.back().destination = destination;
          }
        }
      }
    }
  }

  input_file.close();
  return true;
}
