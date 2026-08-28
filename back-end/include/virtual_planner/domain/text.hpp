#pragma once

// Predicados de texto usados na validacao das entidades de dominio
// (issue #93).
//
// Existiam tres copias identicas de `is_blank`, uma em cada entidade que
// valida descricao (Goal, Task, Reminder), e `User` nao tinha nenhuma — por
// isso aceitava nome e email formados so de espacos. Uma copia por entidade
// significa que a proxima entidade tambem vai esquecer.

#include <string_view>

namespace virtual_planner::domain {

// True quando o texto e vazio ou so tem espaco em branco.
//
// Considera espaco, tab, quebra de linha, retorno de carro, form feed e
// vertical tab — o mesmo conjunto que as copias locais usavam.
[[nodiscard]] bool is_blank(std::string_view value);

} // namespace virtual_planner::domain
