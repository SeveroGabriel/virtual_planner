# Dependencia HTTP+JSON decidida na ADR-003 (issue #13 / P-27):
# cpp-httplib (MIT) + nlohmann/json (MIT), ambas header-only.
#
# Desligada por padrao para que o build sem rede continue funcionando: com
# VIRTUAL_PLANNER_WITH_HTTP=OFF nenhum FetchContent_Declare e avaliado e
# nenhum download e tentado.
option(VIRTUAL_PLANNER_WITH_HTTP "Habilita o PoC HTTP e a dependencia de rede" OFF)

if(VIRTUAL_PLANNER_WITH_HTTP)
  include(FetchContent)

  # O PoC nao usa TLS nem compressao; desligar evita exigir OpenSSL, zlib e
  # Brotli instalados na maquina.
  set(HTTPLIB_REQUIRE_OPENSSL OFF CACHE BOOL "" FORCE)
  set(HTTPLIB_REQUIRE_ZLIB OFF CACHE BOOL "" FORCE)
  set(HTTPLIB_REQUIRE_BROTLI OFF CACHE BOOL "" FORCE)
  set(JSON_BuildTests OFF CACHE INTERNAL "")

  FetchContent_Declare(httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.53.1
    GIT_SHALLOW TRUE
  )

  # O clone git de nlohmann/json ocupa ~195 MB porque traz testes e dados de
  # benchmark. O tarball da release tem 112 KiB, ja contem o CMakeLists que
  # exporta nlohmann_json::nlohmann_json e e verificavel por hash.
  FetchContent_Declare(nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
    URL_HASH SHA256=42f6e95cad6ec532fd372391373363b62a14af6d771056dbfc86160e6dfff7aa
  )

  FetchContent_MakeAvailable(httplib nlohmann_json)

  # PoC de um unico endpoint. Nao entra no build padrao nem no CTest: existe
  # so para provar que a decisao da ADR-003 compila.
  add_executable(virtual_planner_health_poc
    ${VIRTUAL_PLANNER_SOURCE_DIR}/api/health_poc.cpp
  )

  target_link_libraries(virtual_planner_health_poc
    PRIVATE
      httplib::httplib
      nlohmann_json::nlohmann_json
  )

  virtual_planner_enable_warnings(virtual_planner_health_poc)
endif()
