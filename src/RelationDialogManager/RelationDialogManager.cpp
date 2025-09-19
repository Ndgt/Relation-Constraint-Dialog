#include "CustomEventFilters.h"
#include "RelationDialogManager.h"
#include "Utility.h"

#include <QtCore/QSet>
#include <QtWidgets/QMainWindow>

#if QT_VERSION_MAJOR >= 6
#include <QtOpenGLWidgets/QOpenGLWidget>
#else
#include <QtWidgets/QOpenGLWidget>
#endif

FBRegisterCustomManager(RelationDialogManager);
FBCustomManagerImplementation(RelationDialogManager);

bool RelationDialogManager::FBCreate()
{
    mInstance = this;
    return true;
}

bool RelationDialogManager::Clear()
{
    std::lock_guard<std::mutex> lock(mRelationMutex);

    // Clear internal data
    mLastSelectedRelationConstraint = nullptr;
    mRelationViewStates.clear();

    return true;
}

FBConstraintRelation *RelationDialogManager::getLastSelectedRelationConstraint()
{
    std::lock_guard<std::mutex> lock(mRelationMutex);

    if (mLastSelectedRelationConstraint.Ok())
        return mLastSelectedRelationConstraint;

    return nullptr;
}

void RelationDialogManager::eventConnectionSetup()
{
    if (eventConnectionSetupFinished)
        return;

    // Connect callbacks to system events
    FBSystem::TheOne().OnUIIdle.Add(this, (FBCallback)&RelationDialogManager::onUIIdle);
    FBSystem::TheOne().OnConnectionStateNotify.Add(this, (FBCallback)&RelationDialogManager::onRelationSelected);
    FBSystem::TheOne().OnConnectionNotify.Add(this, (FBCallback)&RelationDialogManager::onRelationDeleted);
    FBApplication::TheOne().OnFileExit.Add(this, (FBCallback)&RelationDialogManager::onShutDown);

    eventConnectionSetupFinished = true;
}

void RelationDialogManager::onRelationDeleted(HISender pSender, HKEvent pEvent)
{
    FBEventConnectionNotify connectionEvent(pEvent);

    if (connectionEvent.Action == kFBDisconnected)
    {
        // IMPORTANT: Preventing crashes when using FBPropertyBaseComponent's operators which result in accessing dangling pointers
        HdlFBPlug srcPlugHandle(&connectionEvent.SrcPlug);
        HdlFBPlug dstPlugHandle(&connectionEvent.DstPlug);
        if (!srcPlugHandle.Ok() || !dstPlugHandle.Ok())
            return;

        // Detect deletion of relation constraints and being removed from the scene
        if (!srcPlugHandle->Is(FBConstraintRelation::TypeInfo) || !dstPlugHandle->Is(FBScene::TypeInfo))
            return;

        DIALOG_DEBUG_START;

        FBConstraintRelation *relation = (FBConstraintRelation *)(dstPlugHandle.GetPlug());
        if (relation)
        {
            DIALOG_DEBUG_MESSAGE("Detected deletion of a Relation Constraint. Constraint Name: %s", relation->Name.AsString());

            std::lock_guard<std::mutex> lock(mRelationMutex);

            // If the deleted relation was the last selected one, clear the handle
            if (relation == mLastSelectedRelationConstraint)
                mLastSelectedRelationConstraint = nullptr;

            // Remove the stored view state for the deleted relation constraint
            auto it = mRelationViewStates.find(relation);
            if (it != mRelationViewStates.end())
            {
                mRelationViewStates.erase(it);
                DIALOG_DEBUG_MESSAGE("Removed stored view state for deleted Relation Constraint.");
            }
        }

        DIALOG_DEBUG_END_NOTFAILURE;
    }
}

void RelationDialogManager::onRelationSelected(HISender pSender, HKEvent pEvent)
{
    FBEventConnectionStateNotify connectionStateEvent(pEvent);

    // Note: kFBSelect is sent both when a relation constraint is created and when it is selected
    if (connectionStateEvent.Action == kFBSelect && connectionStateEvent.Plug->Is(FBConstraintRelation::TypeInfo))
    {
        DIALOG_DEBUG_START;

        FBPlug *plug = connectionStateEvent.Plug;
        FBConstraintRelation *relation = (FBConstraintRelation *)plug;

        DIALOG_DEBUG_MESSAGE("Detected selection or creation of a Relation Constraint. Constraint Name: %s", relation->Name.AsString());

        if (relation)
        {
            std::lock_guard<std::mutex> lock(mRelationMutex);

            // If this relation constraint is not already in the map, add it with default view state
            if (mRelationViewStates.find(relation) == mRelationViewStates.end())
                mRelationViewStates[relation] = RelationViewState();

            // Update the last selected relation constraint if it has changed
            if (relation != mLastSelectedRelationConstraint)
                mLastSelectedRelationConstraint = relation;
        }

        DIALOG_DEBUG_MESSAGE("Installing RelationOpenGLWidgetFilter requested.");

        // Request installation of the event filter in the next UI idle event
        installRequired = true;

        DIALOG_DEBUG_END_NOTFAILURE;
    }
}

void RelationDialogManager::onUIIdle(HISender pSender, HKEvent pEvent)
{
    if (!installRequired)
        return;

    // Look for the constraint navigators
    QList<QDockWidget *> dockwidgets;

    // Gather both docked and floating constraint navigators
    dockwidgets.append(getDockedConstraintNavigators());
    dockwidgets.append(getFloatingConstraintNavigators());

    if (dockwidgets.size() == 0)
        return;

    // Install the RelationOpenGLWidgetFilter on the OpenGL widgets of the found dock widgets
    installRelationOpenGLWidgetFilter(dockwidgets);
}

void RelationDialogManager::onShutDown(HISender pSender, HKEvent pEvent)
{
    // Disconnect callbacks from system events
    FBSystem::TheOne().OnUIIdle.Remove(this, (FBCallback)&RelationDialogManager::onUIIdle);
    FBSystem::TheOne().OnConnectionStateNotify.Remove(this, (FBCallback)&RelationDialogManager::onRelationSelected);
    FBSystem::TheOne().OnConnectionNotify.Remove(this, (FBCallback)&RelationDialogManager::onRelationDeleted);

    // Clear internal data
    std::lock_guard<std::mutex> lock(mRelationMutex);
    mLastSelectedRelationConstraint = nullptr;
    mRelationViewStates.clear();

    // Disconnect from application exit event
    FBApplication::TheOne().OnFileExit.Remove(this, (FBCallback)&RelationDialogManager::onShutDown);
}

QPoint RelationDialogManager::getRelationViewTopLeftPos() const
{
    std::lock_guard<std::mutex> lock(mRelationMutex);

    if (mLastSelectedRelationConstraint.Ok())
    {
        auto it = mRelationViewStates.find(mLastSelectedRelationConstraint);
        if (it != mRelationViewStates.end())
            return it->second.mCurrentRelationViewTopLeftPos;
    }

    return {0, 0};
}

double RelationDialogManager::getLastRelationViewScaleFactor() const
{
    std::lock_guard<std::mutex> lock(mRelationMutex);

    if (mLastSelectedRelationConstraint.Ok())
    {
        auto it = mRelationViewStates.find(mLastSelectedRelationConstraint);
        if (it != mRelationViewStates.end())
            return it->second.mLastScaleFactor;
    }

    return 1.0;
}

void RelationDialogManager::setRelationViewState(QPoint topLeftPos, double scaleFactor)
{
    std::lock_guard<std::mutex> lock(mRelationMutex);

    if (!mLastSelectedRelationConstraint.Ok())
        return;

    auto it = mRelationViewStates.find(mLastSelectedRelationConstraint);
    if (it != mRelationViewStates.end())
    {
        it->second.mCurrentRelationViewTopLeftPos = topLeftPos;
        it->second.mLastScaleFactor = scaleFactor;
    }
}

bool RelationDialogManager::installRelationOpenGLWidgetFilter(QList<QDockWidget *> dockwidgets)
{
    bool success = false;

    // Use QSet to avoid duplicate entries
    QSet<QDockWidget *> uniqueDockWidgets(dockwidgets.begin(), dockwidgets.end());

    for (QDockWidget *dockwidget : uniqueDockWidgets)
    {
        QOpenGLWidget *glWidget = dockwidget->findChild<QOpenGLWidget *>();
        if (!glWidget)
            continue;

        glWidget->installEventFilter(&RelationOpenGLWidgetFilter::getInstance());
        success = true;

        // Reset install request flag
        installRequired = false;
    }

    DIALOG_DEBUG_START;

    if (success)
    {
        DIALOG_DEBUG_MESSAGE("RelationOpenGLWidgetFilter installation completed successfully.");
        DIALOG_DEBUG_END_SUCCESS;
    }
    else
    {
        DIALOG_DEBUG_MESSAGE("RelationOpenGLWidgetFilters are not installed.");
        DIALOG_DEBUG_END_NOTFAILURE;
    }

    return success;
}