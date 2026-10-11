include(GoogleTest)

add_executable(moonlight_tests
  test_trace.cpp
  test_simulator.cpp
  test_scheduler.cpp
  test_scoreboard.cpp
  test_pipeline.cpp
  test_coalescer.cpp
  test_cache.cpp
)
target_link_libraries(moonlight_tests PRIVATE moonlight_core GTest::gtest_main)
gtest_discover_tests(moonlight_tests)
