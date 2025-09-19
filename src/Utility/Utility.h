#pragma once

#include <QtWidgets/QDockWidget>
#include <QtWidgets/QMainWindow>

/**
 * @def DIALOG_DEBUG_START
 * @brief Macro to log the start of the setup process
 * @note If PLUGIN_VERSION_STR is defined, this macro will include the version string in the log
 */
#ifdef PLUGIN_VERSION_STR
#define DIALOG_DEBUG_START FBTrace("\n--- Relation Constraint Dialog v%s ---\n", PLUGIN_VERSION_STR)
#else
#define DIALOG_DEBUG_START FBTrace("\n--- Relation Constraint Dialog v? ---\n")
#endif

/**
 * @def DIALOG_DEBUG_MESSAGE
 * @brief Macro to log a debug message during the setup process
 * @param msg The format string for the message
 */
#define DIALOG_DEBUG_MESSAGE(msg, ...) FBTrace(" - " msg "\n", ##__VA_ARGS__)

/**
 * @def DIALOG_DEBUG_END_SUCCESS
 * @brief Macro to log the end of a successful setup process
 */
#define DIALOG_DEBUG_END_SUCCESS FBTrace("--- Setup Completed.\n\n")

/**
 * @def DIALOG_DEBUG_END_FAILURE
 * @brief Macro to log the end of a failed setup process
 */
#define DIALOG_DEBUG_END_FAILURE FBTrace("--- Setup failed.\n\n")

/**
 * @def DIALOG_DEBUG_END_NOTFAILURE
 * @brief Macro to log the end of the setup process when not a failure
 * @note This is used when the setup is skipped or not required
 */
#define DIALOG_DEBUG_END_NOTFAILURE FBTrace("--- \n\n")

/**
 * @brief Gets the docked Constraint Navigator QDockWidget
 * @param mainwindow Pointer to the QMainWindow where the docked widget is located
 * @return Pointer to the QDockWidget if found, otherwise nullptr
 * @note If mainwindow is nullptr, the function will attempt to find the main window automatically
 */
QDockWidget *getDockedConstraintNavigator(QMainWindow *mainwindow = nullptr);

/**
 * @brief Gets the floating Constraint Navigator QDockWidget
 * @return Pointer to the QDockWidget if found, otherwise nullptr
 */
QDockWidget *getFloatingConstraintNavigator();

/**
 * @brief Retrieves the main QMainWindow instance of the application
 * @return Pointer to the main QMainWindow, or nullptr if not found
 * @note This function is a workaround for MotionBuilder 2020, as 'FBGetMainWindow' is available only from MotionBuilder 2022 onwards.
 */
QMainWindow *getMobuMainWindow();