#include "Utility.h"

#include <QtCore/QString>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QList<QDockWidget *> getFloatingConstraintNavigators()
{
    QList<QDockWidget *> foundNavigators;

    // Iterate over top-level widgets to detect "Navigator" or "Constraints" dock widget
    QApplication *app = qobject_cast<QApplication *>(QApplication::instance());
    for (int i = 0; i < app->topLevelWidgets().count(); i++)
    {
        // In case Filtered Navigator, the window title will be "Constraints"
        QString windowTitle = app->topLevelWidgets()[i]->windowTitle();
        if (windowTitle == "Navigator" || windowTitle == "Constraints")
        {
            if (QDockWidget *dockwidget = qobject_cast<QDockWidget *>(app->topLevelWidgets()[i]))
                foundNavigators.append(dockwidget);
        }
    }

    return foundNavigators;
}

QList<QDockWidget *> getDockedConstraintNavigators(QMainWindow *mainwindow)
{
    QList<QDockWidget *> foundNavigators;

    if (!mainwindow)
        mainwindow = getMobuMainWindow();

    if (mainwindow)
    {
        // Find all dock widgets under the main window
        QList<QDockWidget *> dockwidgets = mainwindow->findChildren<QDockWidget *>();
        for (const auto &dockwidget : dockwidgets)
        {
            QString windowTitle = dockwidget->windowTitle();
            if (windowTitle == "Navigator" || windowTitle == "Constraints")
                foundNavigators.append(dockwidget);
        }
    }

    return foundNavigators;
}

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
        QMainWindow *mainwindow = qobject_cast<QMainWindow *>(widget);
        if (mainwindow)
            return mainwindow;
    }

    // Return nullptr if no QMainWindow is found
    return nullptr;
}