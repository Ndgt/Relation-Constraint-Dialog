
#include "CustomEventFilters.h"
#include "GLHooks.h"
#include "RelationDialogManager.h"
#include "Utility.h"

#include <QtWidgets/QMainWindow>

#include <fbsdk/fbsdk.h>

FBLibraryDeclare(RelationConstraintDialog)
{
    FBLibraryRegister(RelationDialogManager);
}
FBLibraryDeclareEnd;

bool FBLibrary::LibInit() { return true; }

bool FBLibrary::LibOpen() { return true; }

bool FBLibrary::LibReady()
{
    // Debug Entry
    DIALOG_DEBUG_START;

    // Start hooking OpenGL functions
    if (!startHook())
    {
        DIALOG_DEBUG_MESSAGE("OpenGL function hooking start failed.");
        DIALOG_DEBUG_END_FAILURE;
        return false;
    }

    DIALOG_DEBUG_MESSAGE("OpenGL functions successfully hooked.");

    // Get the pointer to the MotionBuilder MainWindow
    QMainWindow *mainwindow = getMobuMainWindow();
    if (!mainwindow)
    {
        DIALOG_DEBUG_MESSAGE("MainWindow not found.");
        DIALOG_DEBUG_END_FAILURE;
        return false;
    }

    // Install the MainWindow's event filter
    MainWindowFilter *mainwindowFilter = new MainWindowFilter(mainwindow);
    mainwindow->installEventFilter(mainwindowFilter);
    DIALOG_DEBUG_MESSAGE("MainWindow filter successfully installed.");

    // Install the Constraint Navigator's event filters
    if (!installNavigatorConstraintFilters())
    {
        DIALOG_DEBUG_END_FAILURE;
        return false;
    }
    else
    {
        // Setup the RelationDialogManager event connections
        RelationDialogManager::getInstance().eventConnectionSetup();
        DIALOG_DEBUG_MESSAGE("RelationDialogManager callbacks successfully connected to System and Application Events.");
        DIALOG_DEBUG_END_SUCCESS;
        return true;
    }
}

bool FBLibrary::LibClose()
{
    // Stop hooking OpenGL functions
    endHook();
    return true;
}

bool FBLibrary::LibRelease() { return true; }