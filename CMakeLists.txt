cmake_minimum_required(VERSION 3.19)
project(RelationConstraintDialog)
set(PLUGIN_VERSION 1.0)

# === Environment-specific user configuration ===
set(PRODUCT_VERSION 2026)
set(MOBU_ROOT "C:/Program Files/Autodesk/MotionBuilder ${PRODUCT_VERSION}")
set(CMAKE_PREFIX_PATH "C:/Qt/Qt-6.5.3")

# === Environment Setup ===
if(NOT WIN32)
    message(FATAL_ERROR "This plugin now only supports Windows.")
endif()

if(PRODUCT_VERSION LESS 2020)
    message(FATAL_ERROR "Only MotionBuilder 2020 or above is supported.")
elseif(PRODUCT_VERSION GREATER_EQUAL 2025)
    set(QT_VERSION_MAJOR 6)
else()
    set(QT_VERSION_MAJOR 5)
endif()

# === Qt Setup ===
set(QT_COMPONENTS Core Gui Widgets)
if(QT_VERSION_MAJOR EQUAL 6)
    list(APPEND QT_COMPONENTS OpenGLWidgets)
endif()

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_COMPONENTS})

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

# === Sources ===
add_library(${PROJECT_NAME} SHARED
    src/RelationConstraintDialog_register.cpp
    src/CustomEventFilters/CustomEventFilters.cpp
    src/SearchDialog/SearchDialog.cpp
    src/SuggestionProvider/SuggestionProvider.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "${PROJECT_NAME}-${PLUGIN_VERSION}-MB${PRODUCT_VERSION}"
)

# === Compile Options & Definitions ===
target_compile_definitions(${PROJECT_NAME} PRIVATE PRODUCT_VERSION=${PRODUCT_VERSION}) # for Mobu SDK

if(NOT ${QT_VERSION_MAJOR} EQUAL 6)
    target_compile_options(${PROJECT_NAME} PRIVATE /source-charset:UTF-8)
    # Qt6 will automatically set "-utf8" compile option
endif()

# === Include Paths ===
target_include_directories(${PROJECT_NAME} PRIVATE
    "${MOBU_ROOT}/OpenRealitySDK/include"
    src/CustomEventFilters
    src/SearchDialog
    src/SuggestionProvider
)

# === Link Libraries ===
target_link_libraries(${PROJECT_NAME} PRIVATE
    "${MOBU_ROOT}/OpenRealitySDK/lib/x64/fbsdk.lib"
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
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