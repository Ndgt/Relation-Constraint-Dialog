#include "CustomEventFilters.h"
#include "SearchDialog.h"

#include <string>

#include <QtCore/QChildEvent>
#include <QtCore/QPoint>
#include <QtCore/QString>
#include <QtCore/QtConfig>
#include <QtCore/QTimer>
#include <QtGui/QCursor>
#include <QtGui/QKeyEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QDockWidget>

#include <fbsdk/fbsdk.h>

using namespace std;

// Constructor for MainWindowFilter
MainWindowFilter::MainWindowFilter(QObject *parent)
    : QObject(parent)
{
}

// Returns the QOpenGLWidget inside the "Navigator" or "Constraints" dock widget
QOpenGLWidget *MainWindowFilter::getRelationOpenGLWidget()
{
    // Get pointer to MotionBuilder MainWindow
    QMainWindow *mainWindow = getMobuMainWindow();

    if (mainWindow)
    {
        // Find all dock widgets under the main window
        QList<QDockWidget *> dockwidgets = mainWindow->findChildren<QDockWidget *>();
        for (const auto &dockwidget : dockwidgets)
        {
            QString window_title = dockwidget->windowTitle();

            // In case Filtered Navigator, the window title will be "Constraints"
            if (window_title == "Navigator" || window_title == "Constraints")
            {
                return dockwidget->findChild<QOpenGLWidget *>();
            }
        }
    }
    return nullptr;
}

// Event filter logic
bool MainWindowFilter::eventFilter(QObject *obj, QEvent *pEvent)
{
    // Handle Tab key press (as ShortcutOverride event)
    // Use this Event to prevent invoking default application shortcut "tab"
    if (pEvent->type() == QEvent::ShortcutOverride)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(pEvent);

        // Trigger only on initial Tab press, not repeated
        if (keyEvent->key() == Qt::Key_Tab && !keyEvent->isAutoRepeat())
        {
            QOpenGLWidget *widget = getRelationOpenGLWidget();

            if (widget && widget->underMouse())
            {
                // ShortcutOverride Evet must be explicitly accepted by calling accept()
                // in order to prevent propagation
                keyEvent->accept();

                // Show search dialog at the current cursor position
                QPoint cursor_pos = QCursor::pos();
                SearchDialog *lDialog = new SearchDialog(nullptr, cursor_pos);
                lDialog->show();

                // Mark this event as "Filtered"
                return true;
            }
        }
        return false;
    }

    // Detect when new child windows is polished (fully constructed)
    // Do not use QEvent::ChildAdded because windowTitle might not be initialized
    else if (pEvent->type() == QEvent::ChildPolished)
    {
        // Iterate over top-level widgets to detect "Navigator" or "Constraints" dock widget
        QApplication *app = qobject_cast<QApplication *>(QApplication::instance());
        for (int i = 0; i < app->topLevelWidgets().count(); i++)
        {
            QString window_title = app->topLevelWidgets()[i]->windowTitle();

            // Install custom event filter
            if (window_title == "Navigator" || window_title == "Constraints")
            {
                // Debug to the console
                FBTrace("\n--- Relation Constraint Dialog ---\n\n");
                FBTrace(" - Child Navigator Window creation detected.\n");

                app->topLevelWidgets()[i]->installEventFilter(getNavigatorConstraintsFilter());

                FBTrace(" - Navigator Constraint Filter was successfully installed.\n");
                FBTrace("\n--- Setup Completed.\n\n");

                // Do not mark this event as "Filtered" - Do not return true to let event continue
            }
        }
    }

    return false; // Event not handled by this filter
}

// Constructor for ConstraintsNavigatorFilter
ConstraintsNavigatorFilter::ConstraintsNavigatorFilter(QObject *parent)
    : QObject(parent)
{
}

// Event filter logic
bool ConstraintsNavigatorFilter::eventFilter(QObject *obj, QEvent *pEvent)
{
    // Handle Tab key press (as ShortcutOverride event)
    if (pEvent->type() == QEvent::ShortcutOverride)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(pEvent);

        // Trigger only on initial Tab press, not repeated
        if (keyEvent->key() == Qt::Key_Tab && !keyEvent->isAutoRepeat())
        {
            QOpenGLWidget *widget = obj->findChild<QOpenGLWidget *>();

            if (widget && widget->underMouse())
            {
                // Accept event
                keyEvent->accept();

                // Show the search dialog at the cursor location
                QPoint cursor_pos = QCursor::pos();
                SearchDialog *lDialog = new SearchDialog(nullptr, cursor_pos);
                lDialog->show();

                // Mark this event as "Filtered"
                return true;
            }
        }
    }
    return false; // Event not handled by this filter
}

// Singleton accessor for the ConstraintsNavigatorFilter instance
ConstraintsNavigatorFilter *getNavigatorConstraintsFilter()
{
    static ConstraintsNavigatorFilter instance;
    return &instance;
}

// Get MotionBuilder MainWindow
QMainWindow *getMobuMainWindow()
{
    // Obtain the QApplication instance
    QApplication *app = qobject_cast<QApplication *>(QApplication::instance());
    if (!app)
        return nullptr;

    // Iterate over top-level widgets to detect mainwindow
    const QList<QWidget *> topLevelWidgets = app->topLevelWidgets();
    for (QWidget *widget : topLevelWidgets)
    {
        // Check if the widget is a QMainWindow
        QMainWindow *mainWindow = qobject_cast<QMainWindow *>(widget);
        if (mainWindow)
            return mainWindow;
    }

    // Return nullptr if no QMainWindow is found
    return nullptr;
}