include_directories(../include)

add_executable(basic basic.cpp)
target_link_libraries(basic ray)

add_executable(sphere sphere.cpp)
target_link_libraries(sphere ray)
