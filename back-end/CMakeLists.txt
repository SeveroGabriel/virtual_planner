cmake_minimum_required(VERSION 3.20)

project(
  virtual_planner
  VERSION 0.1.0
  LANGUAGES CXX
)

option(VIRTUAL_PLANNER_WITH_POSTGRES "Build PostgreSQL adapter with libpqxx" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(VIRTUAL_PLANNER_CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(VIRTUAL_PLANNER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(VIRTUAL_PLANNER_TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

include(${VIRTUAL_PLANNER_CMAKE_DIR}/helpers.cmake)

# ---------------------------------------------------------------------------
# Biblioteca principal
#
# As fontes ficam nos modulos de cmake/sources/. Para adicionar um arquivo
# novo, edite apenas o modulo correspondente.
# ---------------------------------------------------------------------------
add_library(virtual_planner_core)

target_include_directories(virtual_planner_core
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

virtual_planner_enable_warnings(virtual_planner_core)

include(${VIRTUAL_PLANNER_CMAKE_DIR}/sources/core.cmake)
include(${VIRTUAL_PLANNER_CMAKE_DIR}/sources/domain.cmake)
include(${VIRTUAL_PLANNER_CMAKE_DIR}/sources/application.cmake)
include(${VIRTUAL_PLANNER_CMAKE_DIR}/sources/infrastructure.cmake)

if(VIRTUAL_PLANNER_WITH_POSTGRES)
  include(${VIRTUAL_PLANNER_CMAKE_DIR}/postgres.cmake)
endif()

# Dependencia HTTP+JSON e PoC de endpoint (ADR-003). O modulo declara a opcao
# VIRTUAL_PLANNER_WITH_HTTP, OFF por padrao, e so entao baixa qualquer coisa;
# por isso e incluido sem condicao aqui.
include(${VIRTUAL_PLANNER_CMAKE_DIR}/http.cmake)

# ---------------------------------------------------------------------------
# Executavel
# ---------------------------------------------------------------------------
add_executable(virtual_planner src/main.cpp)
target_link_libraries(virtual_planner PRIVATE virtual_planner_core)
virtual_planner_enable_warnings(virtual_planner)

# ---------------------------------------------------------------------------
# Testes
#
# Um modulo por area para que duas pessoas registrando testes de modulos
# diferentes nunca editem o mesmo arquivo. Veja docs/conventions.md.
# ---------------------------------------------------------------------------
enable_testing()

include(${VIRTUAL_PLANNER_CMAKE_DIR}/tests/core.cmake)
include(${VIRTUAL_PLANNER_CMAKE_DIR}/tests/infrastructure.cmake)
include(${VIRTUAL_PLANNER_CMAKE_DIR}/tests/goals.cmake)
include(${VIRTUAL_PLANNER_CMAKE_DIR}/tests/tasks.cmake)
include(${VIRTUAL_PLANNER_CMAKE_DIR}/tests/reminders.cmake)
include(${VIRTUAL_PLANNER_CMAKE_DIR}/tests/users.cmake)
include(${VIRTUAL_PLANNER_CMAKE_DIR}/tests/persistence.cmake)

if(VIRTUAL_PLANNER_WITH_POSTGRES)
  include(${VIRTUAL_PLANNER_CMAKE_DIR}/tests/postgres.cmake)
endif()
