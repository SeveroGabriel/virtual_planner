# Testes do modulo de relatorios.
virtual_planner_add_test(reporting_service_test
  unit/application/reporting/reporting_service_test.cpp)

if(VIRTUAL_PLANNER_WITH_HTTP)
  virtual_planner_add_test(reporting_routes_test
    integration/api/reporting_routes_test.cpp)
  target_link_libraries(reporting_routes_test PRIVATE virtual_planner_http)
endif()
