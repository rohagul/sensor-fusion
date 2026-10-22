cmake_minimum_required(VERSION 3.14)

project(sensor_fusion)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Matplot++ REQUIRED)

add_executable(sensor_fusion
    main.cpp
    serial_reader.cpp
    plot_data_realtime.cpp
    plot_matrix.cpp
    linear_fit.cpp
    synthetic_mag_data.cpp

)



#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=address -fno-omit-frame-pointer")
#et(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")

find_package(Eigen3 REQUIRED)

target_link_libraries(sensor_fusion PRIVATE Eigen3::Eigen)

target_link_directories(sensor_fusion PRIVATE
    /usr/local/lib
)

# Look for Matplot++ inside the adjacent libraries directory
find_package(Matplot++ REQUIRED 
    PATHS "${CMAKE_CURRENT_SOURCE_DIR}/../libraries/matplotplusplus"
    NO_DEFAULT_PATH # Forces CMake to check your custom path first
)

# Matplot++::matplot doesn't propagate its own transitive dependencies,
# so they're listed explicitly here, after the library that needs them.
target_link_libraries(sensor_fusion PRIVATE
    Matplot++::matplot
    /usr/local/lib/Matplot++/libnodesoup.a
    tiff
    jpeg
    z
)

find_package(Armadillo REQUIRED)

add_executable(allan_variance
    time_series/allan.cpp
    time_series/allan_variance.cpp
)

target_include_directories(allan_variance PRIVATE ${ARMADILLO_INCLUDE_DIRS})

target_link_libraries(allan_variance PRIVATE
    Matplot++::matplot
    /usr/local/lib/Matplot++/libnodesoup.a
    tiff
    jpeg
    z
    ${ARMADILLO_LIBRARIES}
)