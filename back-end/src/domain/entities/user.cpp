#include "virtual_planner/domain/entities/user.hpp"

#include "virtual_planner/domain/text.hpp"

#include <stdexcept>
#include <string>
#include <utility>

namespace virtual_planner::domain {

namespace {

// Retorna a posicao do unico '@' em `value`, ou std::string::npos se houver
// zero ou mais de uma ocorrencia. Usado para decidir "exatamente um '@'"
// sem puxar <regex> para uma checagem tao simples (P-17.4).
std::string::size_type single_at_position(const std::string& value)
{
    const auto first = value.find('@');
    if (first == std::string::npos)
    {
        return std::string::npos;
    }

    if (value.find('@', first + 1) != std::string::npos)
    {
        return std::string::npos;
    }

    return first;
}

} // namespace

    User::User(std::uint64_t id, std::string name, std::string email)
        : id_(id)
    {
        validate_name(name);
        validate_email(email);
        name_ = std::move(name);
        email_ = std::move(email);
    }

    std::uint64_t User::id() const
    {
        return id_;
    }

    const std::string& User::name() const
    {
        return name_;
    }

    const std::string& User::email() const
    {
        return email_;
    }

    void User::update_name(std::string new_name)
    {
        validate_name(new_name);
        name_ = std::move(new_name);
    }

    void User::update_email(std::string new_email)
    {
        validate_email(new_email);
        email_ = std::move(new_email);
    }

    void User::validate_name(const std::string& name) const
    {
        if (is_blank(name))
        {
            throw std::invalid_argument(
                "User name cannot be empty or blank.");
        }
    }

    // Regra de validacao de e-mail
    //
    //   - Nao pode ser vazio nem so espaco em branco.
    //   - Deve conter exatamente um '@' (zero ou mais de um e invalido).
    //   - Deve haver texto antes do '@' (parte local nao pode ser vazia
    //      nem so espaco).
    //   - A parte depois do '@' (dominio) deve conter um '.' que nao seja
    //      o primeiro nem o ultimo caractere, ou seja, com texto dos dois
    //      lados (rejeita "a@dominio", "a@.com" e "a@dominio.").
    //
    // O que essa regra deliberadamente não valida (mudar isso exige nova
    // decisao, nao ajuste silencioso aqui): caracteres permitidos na parte
    // local, tamanho maximo, TLD conhecido, espaco no meio do endereco,
    // mais de um '.' seguido. Coberta por teste em user_test.cpp.
    void User::validate_email(const std::string& email) const
    {
        if (is_blank(email))
        {
            throw std::invalid_argument(
                "User email cannot be empty or blank.");
        }

        const auto at_pos = single_at_position(email);
        if (at_pos == std::string::npos)
        {
            throw std::invalid_argument(
                "Invalid email: must contain exactly one '@'.");
        }

        const std::string local_part = email.substr(0, at_pos);
        const std::string domain_part = email.substr(at_pos + 1);

        if (is_blank(local_part))
        {
            throw std::invalid_argument(
                "Invalid email: missing text before '@'.");
        }

        const auto last_dot = domain_part.rfind('.');
        const bool has_surrounded_dot =
            last_dot != std::string::npos &&
            last_dot != 0 &&
            last_dot != domain_part.size() - 1;

        if (is_blank(domain_part) || !has_surrounded_dot)
        {
            throw std::invalid_argument(
                "Invalid email: domain must contain a '.' with text on "
                "both sides.");
        }
    }
} // namespace virtual_planner::domain
