#pragma once

#include <fbsdk/fbsdk.h>

#include <QtCore/QEvent>
#include <QtCore/QObject>
#include <QtCore/QPoint>
#include <QtCore/QtConfig>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

#if QT_VERSION_MAJOR >= 6
#include <QtOpenGLWidgets/QOpenGLWidget>
#else
#include <QtWidgets/QOpenGLWidget>
#endif

/**
 * @brief Install custom event filters on the navigator windows
 * @param mainwindow The main application window. If nullptr, it will be retrieved automatically
 * @return true if at least one navigator window was found and filter installed, false otherwise
 * @note This function uses DIALOG_DEBUD_MESSAGE to log its operations, so ensure that DIALOG_DEBUG_START is called before this function
 *       and DIALOG_DEBUG_END_SUCCESS/FAILURE/NOTFAILURE is called after this function to properly log the debug messages.
 */
bool installNavigatorConstraintFilters(QMainWindow *mainwindow = nullptr);

/**
 * @class MainWindowFilter
 * @brief Custom event filter for the main window
 * @details This filter is responsible for detecting when navigator windows are fully constructed,
 *          and installs the ConstraintsNavigatorFilter on them. It is also responsible for intercepting
 *          Tab key presses to open the SearchDialog, and A key presses to reproduce the default 'framing' boxes function
 *          when the mouse cursor is over the relation view.
 */
class MainWindowFilter : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @details Connects keyAPressed signal to the RelationOpenGLWidgetFilter::onKeyAPressed slot.
     * @param parent The parent QObject, default is nullptr
     */
    MainWindowFilter(QObject *parent = nullptr);

signals:
    /**
     * @brief Signal emitted when the 'A' key is pressed
     * @param widget The QOpenGLWidget which shows the relation view
     */
    void keyAPressed(QOpenGLWidget *widget);

protected:
    /**
     * @brief Event filter method
     * @param obj The object receiving the event
     * @param pEvent The event to be processed
     * @return true if the event is handled and should not be propagated further, false otherwise
     */
    bool eventFilter(QObject *obj, QEvent *pEvent) override;
};

/**
 * @class ConstraintsNavigatorFilter
 * @brief Custom event filter for the Navigator window
 * @details Implemented as a singleton. This filter is responsible for intercepting Tab key presses to open the SearchDialog
 *          and A key presses to reproduce the default 'framing' boxes function when the mouse cursor is over the relation view.
 * @note Relation OpenGLWidget view seems not be opened in multiple navigator windows at the same time,
 *       that is, we can assume only one navigator window is open at a time.
 * @note QObject::installEventFilter(QObject * filterObj) will not install the same filterObj multiple times on the same QObject,
 *       that's why we use a singleton here.
 */
class ConstraintsNavigatorFilter : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Get the singleton instance of the ConstraintsNavigatorFilter
     * @return Pointer to the singleton instance
     */
    static ConstraintsNavigatorFilter &getInstance()
    {
        static ConstraintsNavigatorFilter instance;
        return instance;
    }

signals:
    /**
     * @brief Signal emitted when the 'A' key is pressed
     * @param widget The QOpenGLWidget which shows the relation view in the navigator window
     */
    void keyAPressed(QOpenGLWidget *widget);

protected:
    /**
     * @brief Event filter method
     * @param obj The object receiving the event
     * @param pEvent The event to be processed
     * @return true if the event is handled and should not be propagated further, false otherwise
     */
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    /**
     * @brief Singleton constructor
     * @details Connects keyAPressed signal to the RelationOpenGLWidgetFilter::onKeyAPressed slot.
     * @param parent The parent QObject, default is nullptr
     */
    ConstraintsNavigatorFilter(QObject *parent = nullptr);

    /// @cond
    ConstraintsNavigatorFilter(const ConstraintsNavigatorFilter &) = delete;
    ConstraintsNavigatorFilter &operator=(const ConstraintsNavigatorFilter &) = delete;
    /// @endcond
};

/**
 * @class RelationOpenGLWidgetFilter
 * @brief Custom event filter for the QOpenGLWidget within the Relation View
 * @details Implemented as a singleton. This filter handles mouse events for panning and zooming the view within the QOpenGLWidget.
 * @note Relation OpenGLWidget view seems not be opened in multiple navigator windows at the same time,
 *       that is, we can assume only one navigator window is open at a time.
 * @note QObject::installEventFilter(QObject * filterObj) will not install the same filterObj multiple times on the same QObject,
 *       that's why we use a singleton here.
 */
class RelationOpenGLWidgetFilter : public QObject
{
    Q_OBJECT

public slots:
    /**
     * @brief Slot to handle 'A' key press events
     * @details Reproduces the default 'framing boxes' function of the relation view.
     * @param widget The QOpenGLWidget which shows the relation view
     * @note If there are no boxes, the scale factor will be reset to 1.0. The top-left position
     *       will be set so that the viewport center maps to (0,0) in relation view coordinates,
     *       but note that this y-coordinate value seems not to be exactly 0.
     */
    void onKeyAPressed(QOpenGLWidget *widget);

public:
    /**
     * @brief Get the singleton instance of the RelationOpenGLWidgetFilter
     * @return Pointer to the singleton instance
     */
    static RelationOpenGLWidgetFilter &getInstance()
    {
        static RelationOpenGLWidgetFilter instance;
        return instance;
    }

protected:
    /**
     * @brief Event filter method
     * @details Monitors mouse events for panning and zooming the relation view.
     * @param obj The object receiving the event
     * @param pEvent The event to be processed
     * @return Always returns false to avoid blocking the default mouse event processing
     */
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    /**
     * @brief Singleton constructor
     * @param parent The parent QObject, default is nullptr
     */
    RelationOpenGLWidgetFilter(QObject *parent = nullptr) : QObject(parent) {}

    /// @cond
    RelationOpenGLWidgetFilter(const RelationOpenGLWidgetFilter &) = delete;
    RelationOpenGLWidgetFilter &operator=(const RelationOpenGLWidgetFilter &) = delete;
    /// @endcond

    /**
     * @brief Helper function to get the top-left position of the bounding box containing all boxes in the relation view
     * @param relation The FBConstraintRelation object
     * @param outTopLeft Output parameter to receive the top-left position as a QPoint
     * @note We must not use this function if there are no boxes in the relation view as the outTopLeft value will be meaningless.
     */
    void getRelationBoxRectTopLeft(FBConstraintRelation *relation, QPoint &outTopLeft);

private:
    /**
     * @enum DragMode
     * @brief Enumeration for the current mouse drag mode
     */
    enum class DragMode
    {
        Translate,
        Scale
    };

    DragMode mCurrentDragMode; //!< Temporary state for mouse drag mode
    bool mIsDragging = false;  //!< Temporary state for mouse dragging
    QPoint mDragStartPosition; //!< Temporary state for mouse drag start position
};
