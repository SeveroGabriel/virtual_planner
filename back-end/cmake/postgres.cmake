# Localiza libpqxx e habilita o adapter PostgreSQL em virtual_planner_core.
# Incluido apenas quando VIRTUAL_PLANNER_WITH_POSTGRES=ON.

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

include(${VIRTUAL_PLANNER_CMAKE_DIR}/sources/postgres.cmake)

target_compile_definitions(virtual_planner_core PUBLIC VIRTUAL_PLANNER_WITH_POSTGRES)
target_link_libraries(virtual_planner_core PUBLIC ${VIRTUAL_PLANNER_PQXX_TARGET})
