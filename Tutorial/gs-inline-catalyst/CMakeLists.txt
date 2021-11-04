cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)
if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()

project(gray-scott C CXX)

find_package(MPI REQUIRED)
find_package(ADIOS2 REQUIRED)

# We are not using the C++ API of MPI, this will stop the compiler look for it
add_definitions(-DOMPI_SKIP_MPICXX -DMPICH_SKIP_MPICXX)

add_executable(gray-scott
  simulation/main.cpp
  simulation/gray-scott.cpp
  simulation/settings.cpp
  simulation/writer.cpp
  simulation/reader.cpp
)
target_link_libraries(gray-scott 
  PRIVATE
    adios2::cxx11_mpi MPI::MPI_C)
target_include_directories(gray-scott
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/simulation)

add_executable(pdf-calc analysis/pdf-calc.cpp)
target_link_libraries(pdf-calc adios2::cxx11_mpi MPI::MPI_C)

option(USE_CATALYST "Build with ParaView Catalyst enabled" ON)
if (USE_CATALYST)
  find_package(catalyst REQUIRED
    PATHS "${ParaView_DIR}/catalyst")
  target_compile_definitions(gray-scott
    PRIVATE
      "PARAVIEW_IMPL_DIR=\"${ParaView_CATALYST_DIR}\""
        USE_CATALYST=1)
  target_link_libraries(gray-scott
    PRIVATE
      catalyst::catalyst)
  target_include_directories(gray-scott
    PRIVATE
      ${CMAKE_CURRENT_LIST_DIR}/catalyst)
endif()
