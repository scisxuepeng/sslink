project(ff_fib)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
add_definitions(-std=c++11)
#add_definitions(-g)
add_definitions(-O2)
add_definitions(-DUSING_SPIN_STEAL_QUEUE)
#add_definitions(-DUSING_WORK_STEALING_QUEUE)
add_definitions(-DNDEBUG)
OPTION(SSLINK "Use sslink" ON) # Enabled by default
IF(SSLINK)
  ADD_DEFINITIONS(-DSSLINK)
ENDIF(SSLINK)

include_directories(${PROJECT_SOURCE_DIR}/../ff/include)
include_directories(${PROJECT_SOURCE_DIR}/../statistic_mutex_time/)
message(${PROJECT_SOURCE_DIR}/../ff/include)

set(LIBS_LINK_LIBS ff pthread)
link_directories(${PROJECT_SOURCE_DIR}/../bin)

add_executable(ff_fib main.cpp)
target_link_libraries(ff_fib ${LIBS_LINK_LIBS})

