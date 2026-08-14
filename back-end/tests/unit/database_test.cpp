#include "virtual_planner/persistence/database.hpp"
#include "virtual_planner/shared/errors.hpp"

#include "support/expect.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace
{

  class FakeDatabase final : public virtual_planner::persistence::Database
  {
  public:
    int initialized = 0;
    int connected = 0;
    int shutdowns = 0;

  protected:
    void on_initialize() override { ++initialized; }
    void on_connect() override { ++connected; }
    void on_shutdown() override { ++shutdowns; }
  };

  class FailingDatabase final : public virtual_planner::persistence::Database
  {
  protected:
    void on_initialize() override { throw std::runtime_error("cannot initialize"); }
  };

}

int main()
{
  using virtual_planner::persistence::DatabaseState;

  try
  {
    FakeDatabase database;

    VP_EXPECT(database.state() == DatabaseState::NotStarted,
           "database should start as not started");

    database.connect();
    VP_EXPECT(database.is_connected(), "database should be connected");
    VP_EXPECT(database.initialized == 1, "connect should initialize once");
    VP_EXPECT(database.connected == 1, "connect hook should run once");

    database.connect();
    VP_EXPECT(database.connected == 1, "connect should be idempotent when connected");

    database.shutdown();
    VP_EXPECT(database.state() == DatabaseState::Stopped,
           "shutdown should stop the database");
    VP_EXPECT(database.shutdowns == 1, "shutdown hook should run once");

    FailingDatabase failing_database;
    bool initialization_failed = false;
    try
    {
      failing_database.connect();
    }
    catch (const std::runtime_error &)
    {
      initialization_failed = true;
    }

    VP_EXPECT(initialization_failed, "failed initialization should throw");
    VP_EXPECT(failing_database.state() == DatabaseState::Failed,
           "failed initialization should move database to failed state");

    bool failed_state_rejected = false;
    try
    {
      failing_database.connect();
    }
    catch (const virtual_planner::shared::PersistenceError &)
    {
      failed_state_rejected = true;
    }

    VP_EXPECT(failed_state_rejected, "failed state should reject reconnect attempts");
  }
  catch (const std::exception &error)
  {
    std::cerr << error.what() << '\n';
    return 1;
  }

  return 0;
}
