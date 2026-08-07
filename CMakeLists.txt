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

function(virtual_planner_enable_warnings target_name)
  if(MSVC)
    target_compile_options(${target_name} PRIVATE /W4)
  else()
    target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endfunction()

add_library(virtual_planner_core

    # Core
    src/core/app_config.cpp
    src/domain/enums/category.cpp
    src/domain/enums/priority.cpp
    src/domain/enums/task_status.cpp
    src/domain/enums/goal_status.cpp
    src/domain/enums/goal_period.cpp
    src/domain/enums/reminder_recurrence.cpp
    src/domain/enums/reminder_type.cpp
    src/domain/enums/shift.cpp

    # Domain

    # Value Objects
    src/domain/value_objects/time_slot.cpp
    src/domain/value_objects/date.cpp

    #Entities
    src/domain/entities/user.cpp
    src/domain/entities/reminder.cpp
    src/domain/entities/task.cpp
    src/domain/entities/goal.cpp
    

    # Application
    src/application/goal/create_goal_use_case.cpp
    src/application/goal/update_goal_use_case.cpp
    src/application/goal/delete_goal_use_case.cpp
    src/application/goal/list_goals_use_case.cpp
    src/application/goal/change_goal_status_use_case.cpp

    # Infrastructure
    src/infrastructure/config/environment_config_loader.cpp
    src/infrastructure/postgres/postgres_config.cpp
    
    # Persistence
    src/persistence/database.cpp

    # Shared
    src/shared/errors.cpp
)

target_include_directories(virtual_planner_core
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

virtual_planner_enable_warnings(virtual_planner_core)

if(VIRTUAL_PLANNER_WITH_POSTGRES)
  find_package(libpqxx CONFIG QUIET)
  if(TARGET libpqxx::pqxx)
    set(VIRTUAL_PLANNER_PQXX_TARGET libpqxx::pqxx)
  elseif(TARGET pqxx)
    set(VIRTUAL_PLANNER_PQXX_TARGET pqxx)
  else()
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
      pkg_check_modules(PQXX QUIET IMPORTED_TARGET libpqxx)
    endif()

    if(TARGET PkgConfig::PQXX)
      set(VIRTUAL_PLANNER_PQXX_TARGET PkgConfig::PQXX)
    else()
      message(FATAL_ERROR
        "VIRTUAL_PLANNER_WITH_POSTGRES=ON requires libpqxx. "
        "Install libpqxx with CMake package support or install pkg-config "
        "so CMake can locate libpqxx through pkg-config."
      )
    endif()
  endif()

  target_sources(virtual_planner_core
    PRIVATE
      src/infrastructure/postgres/postgres_database.cpp
      src/infrastructure/postgres/postgres_transaction.cpp
      src/infrastructure/postgres/postgres_goal_repository.cpp

  )
  target_compile_definitions(virtual_planner_core PUBLIC VIRTUAL_PLANNER_WITH_POSTGRES)
  target_link_libraries(virtual_planner_core PUBLIC ${VIRTUAL_PLANNER_PQXX_TARGET})
endif()

add_executable(virtual_planner src/main.cpp)
target_link_libraries(virtual_planner PRIVATE virtual_planner_core)
virtual_planner_enable_warnings(virtual_planner)

enable_testing()

add_executable(app_config_test tests/unit/app_config_test.cpp)
target_link_libraries(app_config_test PRIVATE virtual_planner_core)
virtual_planner_enable_warnings(app_config_test)
add_test(NAME app_config_test COMMAND app_config_test)

add_executable(database_test tests/unit/database_test.cpp)
target_link_libraries(database_test PRIVATE virtual_planner_core)
virtual_planner_enable_warnings(database_test)
add_test(NAME database_test COMMAND database_test)

add_executable(postgres_config_test tests/unit/infrastructure/postgres/postgres_config_test.cpp)
target_link_libraries(postgres_config_test PRIVATE virtual_planner_core)
virtual_planner_enable_warnings(postgres_config_test)
add_test(NAME postgres_config_test COMMAND postgres_config_test)

add_executable(create_goal_use_case_test tests/unit/application/goal/create_goal_use_case_test.cpp)
target_link_libraries(create_goal_use_case_test PRIVATE virtual_planner_core)
virtual_planner_enable_warnings(create_goal_use_case_test)
add_test(NAME create_goal_use_case_test COMMAND create_goal_use_case_test)

add_executable(update_goal_use_case_test tests/unit/application/goal/update_goal_use_case_test.cpp)
target_link_libraries(update_goal_use_case_test PRIVATE virtual_planner_core)
virtual_planner_enable_warnings(update_goal_use_case_test)
add_test(NAME update_goal_use_case_test COMMAND update_goal_use_case_test)

add_executable(delete_goal_use_case_test tests/unit/application/goal/delete_goal_use_case_test.cpp)
target_link_libraries(delete_goal_use_case_test PRIVATE virtual_planner_core)
virtual_planner_enable_warnings(delete_goal_use_case_test)
add_test(NAME delete_goal_use_case_test COMMAND delete_goal_use_case_test)

add_executable(list_goals_use_case_test tests/unit/application/goal/list_goals_use_case_test.cpp)
target_link_libraries(list_goals_use_case_test PRIVATE virtual_planner_core)
virtual_planner_enable_warnings(list_goals_use_case_test)
add_test(NAME list_goals_use_case_test COMMAND list_goals_use_case_test)

add_executable(change_goal_status_use_case_test tests/unit/application/goal/change_goal_status_use_case_test.cpp)
target_link_libraries(change_goal_status_use_case_test PRIVATE virtual_planner_core)
virtual_planner_enable_warnings(change_goal_status_use_case_test)
add_test(NAME change_goal_status_use_case_test COMMAND change_goal_status_use_case_test)

if(VIRTUAL_PLANNER_WITH_POSTGRES)
  add_executable(postgres_integration_test tests/integration/postgres/postgres_integration_test.cpp)
  target_link_libraries(postgres_integration_test PRIVATE virtual_planner_core)
  virtual_planner_enable_warnings(postgres_integration_test)
  add_test(NAME postgres_integration_test COMMAND postgres_integration_test)

  add_executable(
    postgres_goal_repository_test
    tests/integration/postgres/postgres_goal_repository_test.cpp
)

target_link_libraries(
    postgres_goal_repository_test
    PRIVATE virtual_planner_core
)

virtual_planner_enable_warnings(
    postgres_goal_repository_test
)

add_test(
    NAME postgres_goal_repository_test
    COMMAND postgres_goal_repository_test
)
    
endif()
