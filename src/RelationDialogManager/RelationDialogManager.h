#pragma once

#include <atomic>
#include <mutex>
#include <unordered_map>

#include <fbsdk/fbsdk.h>

#include <QtCore/QList>
#include <QtCore/QPoint>
#include <QtWidgets/QDockWidget>

/**
 * @class RelationDialogManager
 * @brief Custom manager that receives system events
 * @details This class is responsible for installing event filters on OpenGLWidgets contained within Constraint Navigators.
 */
class RelationDialogManager : public FBCustomManager
{
    FBCustomManagerDeclare(RelationDialogManager);

public:
    /**
     * @brief FBComponent creation function
     * @return true if creation is successful
     * @note The instance of FBCustomManager will be created by FBRegisterCustomManager macro.
     */
    virtual bool FBCreate() override;

    /**
     * @brief Clear the internal data - mLastSelectedRelationConstraint and mRelationViewStates
     * @details This is called when the scene is cleared(new file being loaded, File->New, or the application shutdown).
     * @return true (always)
     */
    virtual bool Clear() override;

    /**
     * @brief Connect callbacks to system events
     * @note Ensures connections are only made once.
     */
    void eventConnectionSetup();

    /**
     * @brief Get the singleton instance of RelationDialogManager
     * @return Reference to the singleton instance
     */
    static RelationDialogManager &getInstance() { return *mInstance; }

    /**
     * @brief Get the last scale factor value of the relation view
     * @return The last scale factor, or 1.0 if mLastSelectedRelationConstraint is invalid
     * @note We do not need to pass the currently selected relation constraint as this function internally retrieves it.
     */
    double getLastRelationViewScaleFactor() const;

    /**
     * @brief Get the last selected relation constraint
     * @return Pointer to the last selected FBConstraintRelation, or nullptr if none
     */
    FBConstraintRelation *getLastSelectedRelationConstraint();

    /**
     * @brief Get the current top-left position of the relation view
     * @return QPoint representing the top-left position, or (0,0) if mLastSelectedRelationConstraint is invalid
     * @note We do not need to pass the currently selected relation constraint as this function internally retrieves it.
     */
    QPoint getRelationViewTopLeftPos() const;

    /**
     * @brief Callback to be connected to FBSystem::OnConnectionNotify event
     * @details Monitors for deletion of relation constraints and removes their stored state.
     * @param pSender The sender of the event
     * @param pEvent The event data
     */
    void onRelationDeleted(HISender pSender, HKEvent pEvent);

    /**
     * @brief Callback to be connected to FBSystem::OnConnectionStateNotify event
     * @details Monitors for creation or selection of relation constraints and sets a flag to install the event filter.
     * @param pSender The sender of the event
     * @param pEvent The event data
     * @note It might be more natural to connect to FBSystem::OnConnectionNotify event, but that event is triggered too frequently
     *       when loading a scene with many components, causing performance issues.
     */
    void onRelationSelected(HISender pSender, HKEvent pEvent);

    /**
     * @brief Callback to be connected to FBSystem::OnUIIdle event
     * @details Installs the RelationOpenGLWidgetFilter on any detected OpenGLWidgets within Constraint Navigators
     *          when the installRequired flag is set.
     * @param pSender The sender of the event
     * @param pEvent The event data
     */
    void onUIIdle(HISender pSender, HKEvent pEvent);

    /**
     * @brief Callback to be connected to FBApplication::OnFileExit event
     * @details Disconnect callbacks from system events and delete internal data.
     * @note This is called when the application is exiting, nothing has been destroyed yet.
     */
    void onShutDown(HISender pSender, HKEvent pEvent);

    /**
     * @brief Set the flag to indicate that installation of the event filter is required
     * @note This is called by the MainWindowFilter when the main layout is changed.
     */
    void setFilterInstallRequired() { installRequired = true; }

    /**
     * @brief Store the current state of the relation view
     * @param topLeftPos The current top-left position of the relation view
     * @param scaleFactor The current scale factor of the relation view
     * @note We do not need to pass the currently selected relation constraint as this function internally retrieves it.
     */
    void setRelationViewState(QPoint topLeftPos, double scaleFactor);

private:
    /// @cond
    RelationDialogManager(const RelationDialogManager &) = delete;
    RelationDialogManager &operator=(const RelationDialogManager &) = delete;
    /// @endcond

    /**
     * @brief Install the RelationOpenGLWidgetFilter on the provided list of QDockWidgets
     * @param dockwidgets List of QDockWidgets which may contain OpenGLWidgets to install the filter on
     * @return true if installation was successful at least on one widget, false otherwise
     */
    bool installRelationOpenGLWidgetFilter(QList<QDockWidget *> dockwidgets);

private:
    /**
     * @struct RelationViewState
     * @brief Structure to hold the state of the relation view (position and scale)
     */
    struct RelationViewState
    {
        QPoint mCurrentRelationViewTopLeftPos = {0, 0}; //!<  Current top-left position of the relation view
        double mLastScaleFactor = 1.0;                  //!<  Last scale factor of the relation view
    };

    inline static RelationDialogManager *mInstance = nullptr; //!< Singleton instance pointer
    std::atomic<bool> installRequired = false;                //!< Flag to indicate that installation of the event filter is required
    bool eventConnectionSetupFinished = false;                //!< Flag to ensure event connections are only set up once

    mutable std::mutex mRelationMutex;                                       //!< Mutex to protect access to mLastSelectedRelationConstraint
    HdlFBPlugTemplate<FBConstraintRelation> mLastSelectedRelationConstraint; //!< Handle to the last selected relation constraint

    /// Map to store the view state (position and scale) for each relation constraint
    std::unordered_map<FBConstraintRelation *, RelationViewState> mRelationViewStates;
};