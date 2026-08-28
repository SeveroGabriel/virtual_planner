# Helpers compartilhados do build do backend.
#
# Este arquivo concentra as funcoes usadas pelos modulos em cmake/sources/ e
# cmake/tests/. Nenhum modulo deve repetir flags de compilacao ou o registro
# manual de um teste: use sempre as funcoes abaixo.

# Instrumenta um alvo para medir cobertura, quando
# VIRTUAL_PLANNER_WITH_COVERAGE=ON.
#
# --coverage precisa entrar na compilacao E na ligacao, e os flags sao PUBLIC
# de proposito: virtual_planner_core e biblioteca estatica, e quem linka ela
# tambem precisa do runtime de gcov. Com PRIVATE a ligacao falha com
# "___llvm_gcov_init ... symbol(s) not found" em todo executavel.
#
# Otimizacao fica desligada porque o mapeamento linha-a-linha de um build
# otimizado nao corresponde ao codigo-fonte.
function(virtual_planner_enable_coverage target_name)
  if(NOT VIRTUAL_PLANNER_WITH_COVERAGE)
    return()
  endif()

  if(MSVC)
    message(WARNING "VIRTUAL_PLANNER_WITH_COVERAGE nao e suportado com MSVC.")
    return()
  endif()

  target_compile_options(${target_name} PUBLIC --coverage -O0 -g)
  target_link_options(${target_name} PUBLIC --coverage)
endfunction()

# Aplica o conjunto padrao de warnings a um alvo.
function(virtual_planner_enable_warnings target_name)
  if(MSVC)
    target_compile_options(${target_name} PRIVATE /W4)
  else()
    target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endfunction()

# Adiciona fontes a biblioteca virtual_planner_core.
#
# Os caminhos sao relativos a back-end/src, para que os modulos permanecam
# curtos e nao dependam do diretorio de onde foram incluidos.
#
#   virtual_planner_add_sources(domain/entities/goal.cpp)
function(virtual_planner_add_sources)
  foreach(source_path IN LISTS ARGN)
    target_sources(virtual_planner_core
      PRIVATE ${VIRTUAL_PLANNER_SOURCE_DIR}/${source_path}
    )
  endforeach()
endfunction()

# Registra um executavel de teste ligado a virtual_planner_core e o publica no
# CTest com o mesmo nome do alvo.
#
# O caminho da fonte e relativo a back-end/tests. O proprio diretorio
# back-end/tests entra no include path do alvo, entao
# `#include "support/expect.hpp"` funciona de qualquer profundidade, sem
# caminho relativo fragil.
#
#   virtual_planner_add_test(create_goal_use_case_test
#     unit/application/goal/create_goal_use_case_test.cpp)
function(virtual_planner_add_test test_name test_source)
  add_executable(${test_name} ${VIRTUAL_PLANNER_TESTS_DIR}/${test_source})
  target_link_libraries(${test_name} PRIVATE virtual_planner_core)
  target_include_directories(${test_name} PRIVATE ${VIRTUAL_PLANNER_TESTS_DIR})
  virtual_planner_enable_warnings(${test_name})
  virtual_planner_enable_coverage(${test_name})
  add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()
