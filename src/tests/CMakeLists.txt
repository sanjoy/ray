include_directories(
  ../include
  googletest/include
  googletest
  )

add_library(gtest
  googletest/src/gtest-all.cc
)

add_executable(run-tests
  test-euclid.cpp
  run-tests-main.cpp
  )

target_link_libraries(run-tests ray gtest)
