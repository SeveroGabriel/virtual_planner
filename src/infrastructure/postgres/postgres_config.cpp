#include "virtual_planner/infrastructure/postgres/postgres_config.hpp"

#include "virtual_planner/shared/errors.hpp"

#include <cstdlib>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace virtual_planner::infrastructure::postgres
{

  namespace
  {

    std::string read_environment_or(const char *name, std::string fallback)
    {
      const char *value = std::getenv(name);
      if (value == nullptr)
      {
        return fallback;
      }

      return value;
    }

    std::uint16_t parse_uint16(const std::string &value, const std::string &field)
    {
      try
      {
        std::size_t parsed = 0;
        const auto number = std::stoul(value, &parsed);
        if (parsed != value.size() || number > std::numeric_limits<std::uint16_t>::max())
        {
          throw shared::ConfigError("invalid PostgreSQL " + field + ": " + value);
        }

        return static_cast<std::uint16_t>(number);
      }
      catch (const std::invalid_argument &)
      {
        throw shared::ConfigError("invalid PostgreSQL " + field + ": " + value);
      }
      catch (const std::out_of_range &)
      {
        throw shared::ConfigError("invalid PostgreSQL " + field + ": " + value);
      }
    }

    std::string read_required(const core::AppConfig &config, std::string key)
    {
      auto value = config.get(key);
      if (!value.has_value() || value->empty())
      {
        throw shared::ConfigError("missing PostgreSQL configuration: " + key);
      }

      return *value;
    }

    void require_not_empty(const std::string &value, const std::string &field)
    {
      if (value.empty())
      {
        throw shared::ConfigError("missing PostgreSQL configuration: " + field);
      }
    }

    std::string quote_value(const std::string &value)
    {
      std::string quoted;
      quoted.reserve(value.size() + 2);
      quoted.push_back('\'');
      for (const auto character : value)
      {
        if (character == '\'' || character == '\\')
        {
          quoted.push_back('\\');
        }
        quoted.push_back(character);
      }
      quoted.push_back('\'');
      return quoted;
    }

  }

  PostgresConfig::PostgresConfig(std::string host,
                                 std::uint16_t port,
                                 std::string database,
                                 std::string user,
                                 std::string password,
                                 std::string sslmode,
                                 std::uint16_t connect_timeout,
                                 std::string application_name)
      : host_(std::move(host)), port_(port), database_(std::move(database)),
        user_(std::move(user)), password_(std::move(password)),
        sslmode_(std::move(sslmode)), connect_timeout_(connect_timeout),
        application_name_(std::move(application_name))
  {
  }

  PostgresConfig PostgresConfig::from_app_config(const core::AppConfig &config)
  {
    const auto port = parse_uint16(config.get_or("postgres.port", "5432"), "port");
    const auto connect_timeout = parse_uint16(
        config.get_or("postgres.connect_timeout", "5"), "connect_timeout");

    return PostgresConfig(
        config.get_or("postgres.host", "localhost"),
        port,
        read_required(config, "postgres.database"),
        read_required(config, "postgres.user"),
        read_required(config, "postgres.password"),
        config.get_or("postgres.sslmode", "disable"),
        connect_timeout,
        config.get_or("postgres.application_name", "virtual-planner"));
  }

  PostgresConfig PostgresConfig::from_environment()
  {
    core::AppConfig config;
    config.set("postgres.host", read_environment_or("POSTGRES_HOST", "localhost"));
    config.set("postgres.port", read_environment_or("POSTGRES_PORT", "5432"));
    config.set("postgres.database", read_environment_or("POSTGRES_DB", ""));
    config.set("postgres.user", read_environment_or("POSTGRES_USER", ""));
    config.set("postgres.password", read_environment_or("POSTGRES_PASSWORD", ""));
    config.set("postgres.sslmode", read_environment_or("POSTGRES_SSLMODE", "disable"));
    config.set("postgres.connect_timeout",
               read_environment_or("POSTGRES_CONNECT_TIMEOUT", "5"));
    config.set("postgres.application_name",
               read_environment_or("POSTGRES_APPLICATION_NAME", "virtual-planner"));
    return from_app_config(config);
  }

  const std::string &PostgresConfig::host() const noexcept { return host_; }

  std::uint16_t PostgresConfig::port() const noexcept { return port_; }

  const std::string &PostgresConfig::database() const noexcept { return database_; }

  const std::string &PostgresConfig::user() const noexcept { return user_; }

  const std::string &PostgresConfig::password() const noexcept { return password_; }

  const std::string &PostgresConfig::sslmode() const noexcept { return sslmode_; }

  std::uint16_t PostgresConfig::connect_timeout() const noexcept
  {
    return connect_timeout_;
  }

  const std::string &PostgresConfig::application_name() const noexcept
  {
    return application_name_;
  }

  void PostgresConfig::validate() const
  {
    require_not_empty(host_, "host");
    require_not_empty(database_, "database");
    require_not_empty(user_, "user");
    require_not_empty(password_, "password");
    require_not_empty(sslmode_, "sslmode");
    require_not_empty(application_name_, "application_name");
    if (port_ == 0)
    {
      throw shared::ConfigError("invalid PostgreSQL port: 0");
    }
    if (connect_timeout_ == 0)
    {
      throw shared::ConfigError("invalid PostgreSQL connect_timeout: 0");
    }
  }

  std::string PostgresConfig::connection_string() const
  {
    validate();
    std::ostringstream output;
    output << "host=" << quote_value(host_)
           << " port=" << port_
           << " dbname=" << quote_value(database_)
           << " user=" << quote_value(user_)
           << " password=" << quote_value(password_)
           << " sslmode=" << quote_value(sslmode_)
           << " connect_timeout=" << connect_timeout_
           << " application_name=" << quote_value(application_name_);
    return output.str();
  }

  std::string PostgresConfig::masked_connection_string() const
  {
    validate();
    std::ostringstream output;
    output << "host=" << quote_value(host_)
           << " port=" << port_
           << " dbname=" << quote_value(database_)
           << " user=" << quote_value(user_)
           << " password='***'"
           << " sslmode=" << quote_value(sslmode_)
           << " connect_timeout=" << connect_timeout_
           << " application_name=" << quote_value(application_name_);
    return output.str();
  }

}
