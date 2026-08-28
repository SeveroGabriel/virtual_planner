#pragma once

#include <stdexcept>
#include <string>

namespace virtual_planner::shared
{

  class ApplicationError : public std::runtime_error
  {
  public:
    explicit ApplicationError(const std::string &message);
  };

  class DomainError final : public ApplicationError
  {
  public:
    explicit DomainError(const std::string &message);
  };

  class ConfigError final : public ApplicationError
  {
  public:
    explicit ConfigError(const std::string &message);
  };

  class PersistenceError final : public ApplicationError
  {
  public:
    explicit PersistenceError(const std::string &message);
  };

  // Recurso pedido nao existe. Vira 404 no mapeamento HTTP (issue #31).
  //
  // Existe porque nao da para responder 404 olhando a mensagem de um
  // std::runtime_error: o status precisa vir do tipo. DomainError ja cobre o
  // papel de erro de validacao (400), entao nao ha um ValidationError
  // separado.
  class NotFoundError final : public ApplicationError
  {
  public:
    explicit NotFoundError(const std::string &message);
  };

  // Conflito com o estado atual, como criar algo que ja existe. Vira 409.
  class ConflictError final : public ApplicationError
  {
  public:
    explicit ConflictError(const std::string &message);
  };

  class InitializationError final : public ApplicationError
  {
  public:
    explicit InitializationError(const std::string &message);
  };

}
