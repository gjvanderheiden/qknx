cmake_minimum_required(VERSION 4.0)

project(qknx LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 23)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON) # Commented this out because I want to place my .ui files in a different folder

include(FindThreads)
find_package(PkgConfig        REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia)
find_package(knx        REQUIRED)
pkg_check_modules(ASIO REQUIRED asio)


set(CMAKE_INCLUDE_CURRENT_DIR ON) 
file(GLOB_RECURSE header_files *.h*)
#file(GLOB_RECURSE CPP_SOURCE_FILES *.cpp)
set(CPP_SOURCE_FILES main.cpp mainwindow.cpp KnxBooleanDataPoint.cpp KnxIntegerDataPoint.cpp)

qt_wrap_ui( UI_FILES mainwindow.ui )

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
   PUBLIC
     FILE_SET headers
     TYPE HEADERS
     BASE_DIRS .
     FILES ${header_files}
   PRIVATE
     ${CPP_SOURCE_FILES} ${UI_FILES} )

target_include_directories(${PROJECT_NAME} PUBLIC ${ASIO_INCLUDE_DIRS}) 

set(QT_LIBS Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Multimedia)

target_link_libraries(
  ${PROJECT_NAME} PUBLIC 
  ${QT_LIBS} knx::knx
)
