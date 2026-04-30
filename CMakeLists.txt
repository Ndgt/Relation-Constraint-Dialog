cmake_minimum_required(VERSION 3.25)

project(${PLUGIN_NAME} VERSION ${PLUGIN_VERSION} LANGUAGES CXX)

if(POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if(NOT WIN32)
    message(FATAL_ERROR "This plugin now only supports Windows.")
endif()

if(PRODUCT_VERSION LESS 2022 OR PRODUCT_VERSION GREATER 2027)
    message(FATAL_ERROR "Only MotionBuilder 2022 to 2027 are supported.")
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

# === Detours Setup ===
add_library(Detours STATIC IMPORTED)
set(Detours_prefix_path "${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}")
set_target_properties(Detours PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${Detours_prefix_path}/include/detours"
    IMPORTED_LOCATION "${Detours_prefix_path}/lib/detours.lib"
)

# === Sources ===
add_library(${PROJECT_NAME} SHARED
    src/RelationConstraintDialogRegister.cpp
    src/ConfigReadWriter/ConfigReadWriter.cpp
    src/CustomEventFilters/CustomEventFilters.cpp
    src/GLHooks/GLHooks.cpp
    src/RelationDialogManager/RelationDialogManager.cpp
    src/SearchDialog/SearchDialog.cpp
    src/SearchDialog/CustomWidgets/SearchBoxLineEdit.cpp
    src/SearchDialog/PreferencesDialog/PreferencesDialog.cpp
    src/SuggestionProvider/SuggestionProvider.cpp
    src/Utility/Utility.cpp
)

# === Set Plugin Output Name ===
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "${PROJECT_NAME}-${PLUGIN_VERSION}-MB${PRODUCT_VERSION}"
)

# === Compile Definitions & Options ===
target_compile_definitions(${PROJECT_NAME} PRIVATE
    PRODUCT_VERSION=${PRODUCT_VERSION}  # for Mobu SDK
    PLUGIN_VERSION_STR="${PLUGIN_VERSION}"
)

target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)

# === Include Paths ===
target_include_directories(${PROJECT_NAME} PRIVATE
    "${MOBU_ROOT}/OpenRealitySDK/include"
    src/ConfigReadWriter
    src/CustomEventFilters
    src/GLHooks
    src/RelationDialogManager
    src/SearchDialog
    src/SearchDialog/CustomWidgets
    src/SearchDialog/PreferencesDialog
    src/SuggestionProvider
    src/Utility
)

# === Link Libraries ===
target_link_libraries(${PROJECT_NAME} PRIVATE
    "${MOBU_ROOT}/OpenRealitySDK/lib/x64/fbsdk.lib"
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    OpenGL::GL
    OpenGL::GLU
    Detours
)

if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::OpenGLWidgets)
endif()

# === Install ===
install(FILES
    $<TARGET_FILE:${PROJECT_NAME}>
    $<TARGET_PDB_FILE:${PROJECT_NAME}>
    src/RelationConstraintDialogConfig.ini
    DESTINATION "${MOBU_ROOT}/bin/x64/plugins"
    OPTIONAL # for Release builds where a PDB file might not be generated
)