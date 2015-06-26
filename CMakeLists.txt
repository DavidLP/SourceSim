cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(SourceSimulation)

find_package(Geant4 REQUIRED ui_all vis_all)

include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

add_executable(SourceSimulation SourceSimulation.cc ${sources} ${headers})
target_link_libraries(SourceSimulation ${Geant4_LIBRARIES})

# Copy scripts and data files to the build directory
set(SOURCESIMULATION_SCRIPTS
  icons.mac
  gui.mac
  run.png
  init.mac
  source.mac
  unit_test_1.mac
  unit_test_2.mac
  vis.mac
  beam.mac
  run.mac
  Sr_90_energy.dat
  Sr_90_1mmShield_energy.dat
  )
foreach(_script ${SOURCESIMULATION_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

# Copy unit test folder to the build directory
file(COPY unit_test DESTINATION ${PROJECT_BINARY_DIR})

# Copy converter folder to the build directory
file(COPY tools DESTINATION ${PROJECT_BINARY_DIR})

install(TARGETS SourceSimulation DESTINATION bin)