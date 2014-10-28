cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(SourceSimulation)

find_package(Geant4 REQUIRED ui_all vis_all)

include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

add_executable(SourceSimulation SourceSimulation.cc ${sources} ${headers})
target_link_libraries(SourceSimulation ${Geant4_LIBRARIES})

# Copy scripts to the build directory
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
  )
foreach(_script ${SOURCESIMULATION_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

install(TARGETS SourceSimulation DESTINATION bin)
