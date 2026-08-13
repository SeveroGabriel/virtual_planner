#pragma once

#include "virtual_planner/persistence/transaction.hpp"

#if defined(VIRTUAL_PLANNER_WITH_POSTGRES)
#include <memory>
#include <pqxx/pqxx>
#endif

namespace virtual_planner::infrastructure::postgres
{

#if defined(VIRTUAL_PLANNER_WITH_POSTGRES)
  class PostgresTransaction final : public persistence::Transaction
  {
  public:
    explicit PostgresTransaction(pqxx::connection &connection);
    ~PostgresTransaction() noexcept override;

    [[nodiscard]] pqxx::work &work();
    void commit() override;
    void rollback() override;

  private:
    std::unique_ptr<pqxx::work> work_;
    bool active_{true};
  };
#endif

}
