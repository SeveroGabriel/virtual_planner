#pragma once

namespace virtual_planner::persistence
{

  class Transaction
  {
  public:
    virtual ~Transaction() = default;

    virtual void commit() = 0;
    virtual void rollback() = 0;
  };

}
