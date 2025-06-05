
#include "CustomEventFilters.h"
#include "SuggestionProvider.h"

#include <string>

#include <QtWidgets/QDockWidget>
#include <QtWidgets/QMainWindow>

#include <fbsdk/fbsdk.h>

// Library declaration - to use FBLibrary function
FBLibraryDeclare(RelationConstraintDialog) {}
FBLibraryDeclareEnd;

bool FBLibrary::LibInit() { return true; }

bool FBLibrary::LibOpen() { return true; }

// Process on library activation
bool FBLibrary::LibReady()
{
    // Debug Entry
    FBTrace("\n--- Relation Constraint Dialog ---\n\n");

    // Collect Relation Operator names
    SuggestionProvider::instance().collectOperatorNames();

    // Debug to the console
    FBTrace(" - Operator names successfully collected.\n");

    // Get the pointer to MotionBuilder MainWindow as QMainWindow
    QMainWindow *mainWindow = getMobuMainWindow();

    if (mainWindow)
    {
        // Install MainWindow Filter
        MainWindowFilter *mainWindow_filter = new MainWindowFilter(mainWindow);
        mainWindow->installEventFilter(mainWindow_filter);

        // Debug to the console
        FBTrace(" - MainWindow filter successfully installed.\n");

        // Install NavigatorConstraint Filter
        QList<QDockWidget *> dock_widgets = mainWindow->findChildren<QDockWidget *>();
        for (const auto &dock_widget : dock_widgets)
        {
            if (!dock_widget)
                continue;

            QString window_title = dock_widget->windowTitle();

            if (window_title == "Navigator")
            {
                // Install the filter to default navigator window
                dock_widget->installEventFilter(getNavigatorConstraintsFilter());

                // Debug to the console
                FBTrace(" - Default Navigator filter successfully installed.\n");
                FBTrace("\n--- Setup Completed.\n\n");
            }
        }
    }
    else
    {
        // Debug to the console
        FBTrace(" - MainWindow not found.\n");
        FBTrace("\n--- Setup failed.\n\n");
    }

    return true;
}

bool FBLibrary::LibClose() { return true; }

bool FBLibrary::LibRelease() { return true; }