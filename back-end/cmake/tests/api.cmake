# Testes da camada de API.
#
# Este modulo so e incluido quando VIRTUAL_PLANNER_WITH_JSON=ON, porque os
# alvos daqui dependem de nlohmann/json.
virtual_planner_add_test(shared_json_test unit/api/json/shared_json_test.cpp)
target_link_libraries(shared_json_test PRIVATE virtual_planner_json)

# O teste do servidor sobe uma porta de verdade, entao so existe com a
# camada HTTP compilada.
if(VIRTUAL_PLANNER_WITH_HTTP)
  virtual_planner_add_test(api_server_test integration/api/api_server_test.cpp)
  target_link_libraries(api_server_test PRIVATE virtual_planner_http)
endif()
