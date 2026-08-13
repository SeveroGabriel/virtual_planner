#include "virtual_planner/infrastructure/postgres/postgres_transaction.hpp"

#if defined(VIRTUAL_PLANNER_WITH_POSTGRES)

#include "virtual_planner/shared/errors.hpp"

#include <exception>
#include <string>

namespace virtual_planner::infrastructure::postgres
{

  PostgresTransaction::PostgresTransaction(pqxx::connection &connection)
      : work_(std::make_unique<pqxx::work>(connection))
  {
  }

  PostgresTransaction::~PostgresTransaction() noexcept
  {
    if (!active_ || work_ == nullptr)
    {
      return;
    }

    try
    {
      work_->abort();
    }
    catch (...)
    {
    }
  }

  pqxx::work &PostgresTransaction::work()
  {
    if (!active_ || work_ == nullptr)
    {
      throw shared::PersistenceError("PostgreSQL transaction is not active");
    }

    return *work_;
  }

  void PostgresTransaction::commit()
  {
    try
    {
      work().commit();
      active_ = false;
    }
    catch (const std::exception &error)
    {
      throw shared::PersistenceError(std::string("failed to commit PostgreSQL transaction: ") +
                                     error.what());
    }
  }

  void PostgresTransaction::rollback()
  {
    if (!active_ || work_ == nullptr)
    {
      return;
    }

    try
    {
      work_->abort();
      active_ = false;
    }
    catch (const std::exception &error)
    {
      throw shared::PersistenceError(std::string("failed to rollback PostgreSQL transaction: ") +
                                     error.what());
    }
  }

}

#endif
