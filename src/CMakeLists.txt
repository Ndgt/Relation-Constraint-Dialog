cmake_minimum_required(VERSION 3.17)
project(RelationConstraintDialog)
set(PLUGIN_VERSION 2.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# === Environment-specific user configuration ===
set(PRODUCT_VERSION 2026)
set(MOBU_ROOT "C:/Program Files/Autodesk/MotionBuilder ${PRODUCT_VERSION}")
set(QT_SOURCE_SEARCH_PATH "C:/Qt/6.5.3/msvc2019_64")
set(DETOURS_ROOT "C:/Detours")

# === Environment Setup ===
if(NOT WIN32)
    message(FATAL_ERROR "This plugin now only supports Windows.")
endif()

if(PRODUCT_VERSION LESS 2020)
    message(FATAL_ERROR "Only MotionBuilder 2020 or above is supported.")
endif()

# === OpenGL Setup ===
find_package(OpenGL REQUIRED)

# === Qt Setup ===
if(PRODUCT_VERSION GREATER_EQUAL 2025)
    set(QT_VERSION_MAJOR 6)
else()
    set(QT_VERSION_MAJOR 5)
endif()

set(QT_COMPONENTS Core Gui Widgets)
if(QT_VERSION_MAJOR EQUAL 6)
    list(APPEND QT_COMPONENTS OpenGLWidgets)
endif()

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_COMPONENTS} PATHS ${QT_SOURCE_SEARCH_PATH})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# === Sources ===
add_library(${PROJECT_NAME} SHARED
    RelationConstraintDialogRegister.cpp
    CustomEventFilters/CustomEventFilters.cpp
    GLHooks/GLHooks.cpp
    RelationDialogManager/RelationDialogManager.cpp
    SearchDialog/CustomLineEdit.cpp
    SearchDialog/SearchDialog.cpp
    SuggestionProvider/SuggestionProvider.cpp
    Utility/Utility.cpp
)

# === Set Plugin Output Name ===
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "${PROJECT_NAME}-${PLUGIN_VERSION}-MB${PRODUCT_VERSION}"
)

# === Compile Definitions & Options ===
target_compile_definitions(${PROJECT_NAME} PRIVATE
    PRODUCT_VERSION=${PRODUCT_VERSION} # for Mobu SDK
    PLUGIN_VERSION_STR="${PLUGIN_VERSION}"
)

target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)

# === Include Paths ===
target_include_directories(${PROJECT_NAME} PRIVATE
    "${MOBU_ROOT}/OpenRealitySDK/include"
    CustomEventFilters
    GLHooks
    RelationDialogManager
    SearchDialog
    SuggestionProvider
    Utility
    "${DETOURS_ROOT}/include"
)

# === Link Libraries ===
target_link_libraries(${PROJECT_NAME} PRIVATE
    "${MOBU_ROOT}/OpenRealitySDK/lib/x64/fbsdk.lib"
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    OpenGL::GL
    OpenGL::GLU
    "${DETOURS_ROOT}/lib.X64/detours.lib"
)

if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::OpenGLWidgets)
endif()

# === Post-build: Copy to Plugin Folder ===
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:${PROJECT_NAME}>
    "${MOBU_ROOT}/bin/x64/plugins/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
)

# Use this if you want to copy PDB file for RelWithDebInfo/Debug builds
#[[
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "$<TARGET_PDB_FILE:${PROJECT_NAME}>"
    "${MOBU_ROOT}/bin/x64/plugins/$<TARGET_PDB_FILE_NAME:${PROJECT_NAME}>"
)
]]