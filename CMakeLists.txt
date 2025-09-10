cmake_minimum_required(VERSION 3.15...4.1.1)

project(
        ToggleExternalView
VERSION 1.0
DESCRIPTION "Toggle the cockpit view."
LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

#TODO: Add Mac / Linux support

# Preprocesor Macros
add_definitions(-DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1 -DXPLM303=1 -DXPLM400=1 -DXPLM410) # Defines the supported SDK versions
add_definitions(-DAPL=$<BOOL:${APPLE}> -DIBM=$<BOOL:${WIN32}> -DLIN=$<AND:$<BOOL:${UNIX}>,$<NOT:$<BOOL:${APPLE}>>>) # Defines the OS used

# Include XPSDK headers
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/SDK/CHeaders/Widgets")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/SDK/CHeaders/Wrappers")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/SDK/CHeaders/XPLM")

# Adds the source code as the first linkable library
add_library(ToggleExternalView SHARED src/ToggleExternalView.cpp)

# Defines XPLM and XPWidgets as the second and third linkable libraries
add_library(XPLM STATIC IMPORTED)
add_library(XPWidgets STATIC IMPORTED)

# Tells CMake where the above libraries are imported from
set_target_properties(XPLM PROPERTIES
IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/src/SDK/Libraries/Win/XPLM_64.lib")

set_target_properties(XPWidgets PROPERTIES
IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/src/SDK/Libraries/Win/XPWidgets_64.lib")


# Set the options used by the compiler
add_compile_options(-O3 -fPIC)

# Set the target then the libraries linked
target_link_libraries(ToggleExternalView XPLM XPWidgets)

set_target_properties(ToggleExternalView PROPERTIES OUTPUT_NAME "win")
set_target_properties(ToggleExternalView PROPERTIES SUFFIX ".xpl")