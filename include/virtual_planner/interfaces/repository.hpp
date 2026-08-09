#pragma once

#include <optional>
#include <vector>

namespace virtual_planner::interfaces
{

  template <typename Entity, typename Id>
  class Repository
  {
  public:
    virtual ~Repository() = default;

    [[nodiscard]] virtual std::optional<Entity> find_by_id(const Id &id) const = 0;
    [[nodiscard]] virtual std::vector<Entity> find_all() const = 0;
    virtual void save(const Entity &entity) = 0;
    virtual void update(const Entity &entity) = 0;
    virtual void remove(const Id &id) = 0;
  };

}