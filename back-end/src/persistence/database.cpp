#include "virtual_planner/persistence/database.hpp"

#include "virtual_planner/shared/errors.hpp"

namespace virtual_planner::persistence
{

  void Database::initialize()
  {
    if (state_ == DatabaseState::Started || state_ == DatabaseState::Connected)
    {
      return;
    }

    if (state_ == DatabaseState::Failed)
    {
      throw shared::PersistenceError("database is in failed state");
    }

    try
    {
      on_initialize();
      state_ = DatabaseState::Started;
    }
    catch (...)
    {
      state_ = DatabaseState::Failed;
      throw;
    }
  }

  void Database::connect()
  {
    if (state_ == DatabaseState::Connected)
    {
      return;
    }

    if (state_ == DatabaseState::Failed)
    {
      throw shared::PersistenceError("database is in failed state");
    }

    if (state_ == DatabaseState::NotStarted || state_ == DatabaseState::Stopped)
    {
      initialize();
    }

    try
    {
      on_connect();
      state_ = DatabaseState::Connected;
    }
    catch (...)
    {
      state_ = DatabaseState::Failed;
      throw;
    }
  }

  void Database::shutdown()
  {
    if (state_ == DatabaseState::NotStarted || state_ == DatabaseState::Stopped)
    {
      return;
    }

    try
    {
      on_shutdown();
      state_ = DatabaseState::Stopped;
    }
    catch (...)
    {
      state_ = DatabaseState::Failed;
      throw;
    }
  }

  DatabaseState Database::state() const noexcept { return state_; }

  bool Database::is_connected() const noexcept
  {
    return state_ == DatabaseState::Connected;
  }

  void Database::on_initialize() {}

  void Database::on_connect() {}

  void Database::on_shutdown() {}

}
