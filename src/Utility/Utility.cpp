#include "Utility.h"

#include <functional>
#include <set>

#include <QtCore/QString>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QList<QDockWidget *> getFloatingConstraintNavigators()
{
    QList<QDockWidget *> foundNavigators;

    // Iterate over top-level widgets to detect "Navigator" or "Constraints" dock widget
    QApplication *app = qobject_cast<QApplication *>(QApplication::instance());
    for (QWidget *widget : app->topLevelWidgets())
    {
        // In case Filtered Navigator, the window title will be "Constraints"
        QString windowTitle = widget->windowTitle();
        if (windowTitle == "Navigator" || windowTitle == "Constraints")
        {
            if (QDockWidget *dockwidget = qobject_cast<QDockWidget *>(widget))
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
    for (QWidget *widget : app->topLevelWidgets())
    {
        // Check if the widget is a QMainWindow
        QMainWindow *mainwindow = qobject_cast<QMainWindow *>(widget);
        if (mainwindow)
            return mainwindow;
    }

    // Return nullptr if no QMainWindow is found
    return nullptr;
}

FBConstraintRelation *getConstraintRelationFromName(std::string name)
{
    // Iterate over all constraints in the scene
    for (int i = 0; i < FBSystem().Scene->Constraints.GetCount(); ++i)
    {
        FBConstraint *constraint = FBSystem().Scene->Constraints[i];

        if (FBIS(constraint, FBConstraintRelation))
        {
            FBConstraintRelation *relation = (FBConstraintRelation *)constraint;
            if (std::string(relation->Name.AsString()) == name)
            {
                return relation; // Return the matching relation
            }
        }
    }

    return nullptr; // Return nullptr if no matching relation is found
}

bool checkMacroRecursivity(std::string currentRelationName, std::string macroCandidateName)
{
    FBConstraintRelation *currentRelation = getConstraintRelationFromName(currentRelationName);
    if (!currentRelation)
        return false;

    FBConstraintRelation *macroCandidate = getConstraintRelationFromName(macroCandidateName);
    if (!macroCandidate)
        return false;

    // The set to keep track of visited nodes in the current path
    std::set<FBConstraintRelation *> visitedPath;

    std::function<bool(FBConstraintRelation *)> checkRecursively =
        [&](FBConstraintRelation *relationToCheck) -> bool
    {
        // Add the current node to the visited path
        visitedPath.insert(relationToCheck);

        for (int i = 0; i < relationToCheck->GetSrcCount(); ++i)
        {
            FBPlug *srcPlug = relationToCheck->GetSrc(i);

            // Check if the source plug is a FBConstraintRelation
            if (FBIS(srcPlug, FBConstraintRelation) == false)
                continue;

            FBConstraintRelation *relation = (FBConstraintRelation *)srcPlug;

            // Recursion detected
            if (relation == currentRelation)
                return true;

            // Avoid cycles by checking if the node is already visited
            if (visitedPath.find(relation) == visitedPath.end())
            {
                if (checkRecursively(relation))
                    return true;
            }
        }

        // Backtrack: remove the current node from the visited path
        visitedPath.erase(relationToCheck);

        return false;
    };

    return checkRecursively(macroCandidate);
}