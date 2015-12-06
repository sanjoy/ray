include_directories(../include)

add_executable(driver driver.cpp basic.cpp sphere.cpp refraction.cpp)
target_link_libraries(driver ray)
