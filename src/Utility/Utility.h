#pragma once

#include <string>

#include <QtCore/QList>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QMainWindow>

#include <fbsdk/fbsdk.h>

#if PRODUCT_VERSION == 2020
// Since Mobu SDK sets the C4946 warning as an error,
// "error C4946: reinterpret_cast used between related classes: 'QMapNodeBase' and 'QMapNode<Key, T>'"
// will be triggered when we use Qt 5.12.5 and MSVC 2017. So we disable this warning here.
#pragma warning(disable : 4946)
#endif

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
 * @brief Gets the list of all docked Constraint Navigator QDockWidgets
 * @param mainwindow Pointer to the QMainWindow where the docked widget is located
 * @return List of pointers to QDockWidgets if found, otherwise an empty list
 * @note If mainwindow is nullptr, the function will attempt to find the main window automatically
 */
QList<QDockWidget *> getDockedConstraintNavigators(QMainWindow *mainwindow = nullptr);

/**
 * @brief Gets the list of all floating Constraint Navigator QDockWidgets
 * @return List of pointers to floating QDockWidgets if found, otherwise an empty list
 */
QList<QDockWidget *> getFloatingConstraintNavigators();

/**
 * @brief Retrieves the main QMainWindow instance of the application
 * @return Pointer to the main QMainWindow, or nullptr if not found
 * @note This function is a workaround for MotionBuilder 2020, as 'FBGetMainWindow' is available only from MotionBuilder 2022 onwards.
 */
QMainWindow *getMobuMainWindow();

/**
 * @brief Finds a FBConstraintRelation by its name in the current scene
 * @param name The name of the FBConstraintRelation to find
 * @return Pointer to the FBConstraintRelation if found, otherwise nullptr
 */
FBConstraintRelation *getConstraintRelationFromName(std::string name);

/**
 * @brief Checks if adding a macro to a relation would create a recursive loop
 * @param currentRelationName The name of the current FBConstraintRelation
 * @param macroCandidateName The name of the macro FBConstraintRelation to be added
 * @return true if adding the macro would create a recursion, false otherwise
 */
bool checkMacroRecursivity(std::string currentRelationName, std::string macroCandidateName);