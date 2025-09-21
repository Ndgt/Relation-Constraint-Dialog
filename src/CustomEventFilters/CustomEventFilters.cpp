#include "CustomEventFilters.h"
#include "GLHooks.h"
#include "RelationDialogManager.h"
#include "SearchDialog.h"
#include "Utility.h"

#include <array>

#include <QtCore/QSet>
#include <QtCore/QTimer>
#include <QtGui/QCursor>
#include <QtGui/QKeyEvent>
#include <QtGui/qopenglext.h> // GL_DRAW_FRAMEBUFFER_BINDING
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QMainWindow>

#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

/**
 * @brief The spacing between grid lines in the relation view
 * @note This value is constant, even when the relation view is scaled.
 * @note Use the ratio 'relationViewGridSpacing / defaultGLGridSpacing' to convert from GL coordinates to Relation View coordinates.
 */
constexpr double relationViewGridSpacing = 40.0;

bool installNavigatorConstraintFilters()
{
    // Gather all constraint navigators (both floating and docked)
    QList<QDockWidget *> allNavigators;
    allNavigators.append(getFloatingConstraintNavigators());
    allNavigators.append(getDockedConstraintNavigators());

    if (allNavigators.isEmpty())
    {
        DIALOG_DEBUG_MESSAGE("No Constraints Navigator found at this time.");
        return false;
    }

    // Use QSet to avoid duplicate entries
#if QT_VERSION_MAJOR == 5 && QT_VERSION_MINOR == 12
    // QSet constructor accepts two iterators is not available in Qt 5.12.5
    QSet<QDockWidget *> uniqueNavigators = QSet<QDockWidget *>::fromList(allNavigators);
#else
    QSet<QDockWidget *> uniqueNavigators(allNavigators.begin(), allNavigators.end());
#endif

    // Install NavigatorConstraint Filters
    for (QDockWidget *navigator : uniqueNavigators)
    {
        QByteArray windowTitle = navigator->windowTitle().toUtf8();
        DIALOG_DEBUG_MESSAGE("Found Constraints Navigator: '%s'", windowTitle.constData());
        navigator->installEventFilter(&ConstraintsNavigatorFilter::getInstance());
        DIALOG_DEBUG_MESSAGE("Constraints Navigator Filter successfully installed.");
    }

    return true;
}

static double getHookedLastGridSpacing(QOpenGLWidget *widget)
{
    if (!widget)
        return getLastGridSpacing(-1);

    widget->makeCurrent();

    GLint framebuffer;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &framebuffer);

    widget->doneCurrent();

    return getLastGridSpacing(framebuffer);
}

static std::array<double, 4> getHookedBoxRectRange(QOpenGLWidget *widget)
{
    if (!widget)
        return getBoxRectRange(-1);

    widget->makeCurrent();

    GLint framebuffer;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &framebuffer);

    widget->doneCurrent();

    return getBoxRectRange(framebuffer);
}

/**
 * @brief Helper function to unproject the cursor position from QOpenGLWidget to Relation View coordinates
 * @param widget The QOpenGLWidget to unproject from
 * @param localCursorPos The cursor position in local widget coordinates
 * @param outRelationViewPos The output relation view coordinates
 * @param scaleFactor The scale factor to apply to the unprojected coordinates (default is 1.0)
 * @return true if unprojection is successful, false otherwise
 */
static bool unprojectGLWidgetToRelationView(QOpenGLWidget *widget, const QPoint &localCursorPos, QPoint &outRelationViewPos, double scaleFactor = 1.0)
{
    if (!widget)
        return false;

    widget->makeCurrent();

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    GLdouble modelview[16], projection[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);

    double relationViewPosX, relationViewPosY, _relationViewPosZ;
    if (gluUnProject(localCursorPos.x(), viewport[3] - localCursorPos.y(), 0.0, modelview, projection, viewport,
                     &relationViewPosX, &relationViewPosY, &_relationViewPosZ))
    {
        double scaleGLToRelation = relationViewGridSpacing / defaultGLGridSpacing * scaleFactor;
        double relationViewPosScaledX = std::round(relationViewPosX * scaleGLToRelation);
        double relationViewPosScaledY = std::round(relationViewPosY * scaleGLToRelation);

        outRelationViewPos = QPoint(relationViewPosScaledX, relationViewPosScaledY);

        widget->doneCurrent();
        return true;
    }

    widget->doneCurrent();
    return false;
}

/**
 * @brief Helper function to process a ShortcutOverride event to check for Tab key press and open the SearchDialog
 * @param keyEvent The QKeyEvent to be processed
 * @param widget The QOpenGLWidget to check if the mouse is over
 * @return true if the event should be consumed and not propagated further, false otherwise
 * @note This function assumes that the keyEvent is for the Tab key and is not an auto-repeat event.
 */
static bool consumeTabKeyShortcutOverrideEvent(QOpenGLWidget *widget, QKeyEvent *keyEvent)
{
    if (widget && keyEvent && widget->underMouse())
    {
        QPoint localCursorPos = widget->mapFromGlobal(QCursor::pos());
        double scaleFactor = defaultGLGridSpacing / getHookedLastGridSpacing(widget);
        QPoint relationViewPosTemp;

        if (!unprojectGLWidgetToRelationView(widget, localCursorPos, relationViewPosTemp, scaleFactor))
        {
            DIALOG_DEBUG_START;
            DIALOG_DEBUG_MESSAGE("Failed to unproject cursor position to relation view coordinates.");
            DIALOG_DEBUG_END_FAILURE;
            return false;
        }

        QPoint relationViewPos = RelationDialogManager::getInstance().getRelationViewTopLeftPos() + relationViewPosTemp;
        FBConstraintRelation *selectedConstraint = RelationDialogManager::getInstance().getLastSelectedRelationConstraint();

        /**
         * @note If only one box is positioned far away from the origin (0,0), the 'framing' function of the relation view
         *       seems to not work properly. So check if the calculated position is within valid range.
         *       The value 117500 is approximately 99900 in the OpenGL coordinate system(default scale)
         */
        if (std::abs(relationViewPos.x()) > 117500 || std::abs(relationViewPos.y()) > 117500)
        {
            if (selectedConstraint && selectedConstraint->Boxes.GetCount() == 0)
            {
                FBMessageBox("Relation Constraint Dialog",
                             "[Error] The position is outside the valid range of the relation view.\n"
                             "Press 'A' with the mouse over the relation view, and try again.",
                             "OK");

                return false;
            }
        }

        // Create the SearchDialog with the calculated position and retrieved constraint
        SearchDialog *lDialog = new SearchDialog(QCursor::pos(), relationViewPos, selectedConstraint);

        // Show the dialog at the cursor position
        lDialog->show();

        return true; // Event should be consumed
    }

    return false; // Event should not be consumed
}

MainWindowFilter::MainWindowFilter(QObject *parent) : QObject(parent)
{
    connect(this, &MainWindowFilter::keyAPressed, &RelationOpenGLWidgetFilter::getInstance(), &RelationOpenGLWidgetFilter::onKeyAPressed);
}

bool MainWindowFilter::eventFilter(QObject *obj, QEvent *pEvent)
{
    QMainWindow *mainwindow = qobject_cast<QMainWindow *>(obj);
    if (!mainwindow)
        return false;

    // Detect when new child windows is polished (fully constructed)
    // Do not use QEvent::ChildAdded because windowTitle might not be initialized
    if (pEvent->type() == QEvent::ChildPolished)
    {
        DIALOG_DEBUG_START;
        DIALOG_DEBUG_MESSAGE("Child Widget creation detected in MainWindow.");

        if (installNavigatorConstraintFilters())
        {
            // Notify RelationDialogManager to install the RelationOpenGLWidgetFilter
            // Use a short delay to ensure the relation view is fully initialized
            QTimer::singleShot(50, []()
                               { RelationDialogManager::getInstance().setFilterInstallRequired(); });
            DIALOG_DEBUG_END_SUCCESS;
        }
        else
            DIALOG_DEBUG_END_NOTFAILURE;

        // Avoid this event to be processed further
        pEvent->accept();
        return true;
    }

    // Handle Tab key and A key press (as ShortcutOverride event)
    // Note: This ensures that key press is captured even the navigator window is not focused
    else if (pEvent->type() == QEvent::ShortcutOverride)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(pEvent);

        QOpenGLWidget *widget = nullptr;

        // Gather all constraint navigators (both floating and docked)
        QList<QDockWidget *> allNavigators;
        allNavigators.append(getFloatingConstraintNavigators());
        allNavigators.append(getDockedConstraintNavigators(mainwindow));

        // Find the QOpenGLWidget under the mouse cursor
        for (QDockWidget *navigator : allNavigators)
        {
            QOpenGLWidget *currentWidget = navigator->findChild<QOpenGLWidget *>();
            if (currentWidget && currentWidget->underMouse())
            {
                widget = currentWidget;
                break;
            }
        }

        if (keyEvent->key() == Qt::Key_Tab && !keyEvent->isAutoRepeat())
        {
            if (consumeTabKeyShortcutOverrideEvent(widget, keyEvent))
            {
                keyEvent->accept();
                return true;
            }
        }
        else if (keyEvent->key() == Qt::Key_A && !keyEvent->isAutoRepeat())
        {
            if (!widget)
                return false;

            // Emit signal after a short delay to wait for relation view to be completely changed
            QTimer::singleShot(50, [this, widget]()
                               { emit keyAPressed(widget); });

            pEvent->accept();
            return true;
        }
    }

    // Other events are not handled by this filter
    return false;
}

ConstraintsNavigatorFilter::ConstraintsNavigatorFilter(QObject *parent) : QObject(parent)
{
    connect(this, &ConstraintsNavigatorFilter::keyAPressed, &RelationOpenGLWidgetFilter::getInstance(), &RelationOpenGLWidgetFilter::onKeyAPressed);
}

bool ConstraintsNavigatorFilter::eventFilter(QObject *obj, QEvent *pEvent)
{
    if (pEvent->type() == QEvent::ShortcutOverride)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(pEvent);

        QOpenGLWidget *widget = obj->findChild<QOpenGLWidget *>();

        // The same logic as in MainWindowFilter::eventFilter
        if (keyEvent->key() == Qt::Key_Tab && !keyEvent->isAutoRepeat())
        {
            if (consumeTabKeyShortcutOverrideEvent(widget, keyEvent))
            {
                keyEvent->accept();
                return true;
            }
        }
        else if (keyEvent->key() == Qt::Key_A && !keyEvent->isAutoRepeat())
        {
            if (!widget || !widget->underMouse())
                return false;

            QTimer::singleShot(50, [this, widget]()
                               { emit keyAPressed(widget); });

            pEvent->accept();
            return true;
        }
    }

    // Other events are not handled by this filter
    return false;
}

bool RelationOpenGLWidgetFilter::eventFilter(QObject *obj, QEvent *pEvent)
{
    if (pEvent->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(pEvent);

        // Detect Drag Start with Left Mouse Button + Shift or Ctrl
        if (mouseEvent->button() == Qt::LeftButton)
        {
            if (mouseEvent->modifiers() & Qt::ShiftModifier)
            {
                mIsDragging = true;
                mDragStartPosition = mouseEvent->pos();
                mCurrentDragMode = DragMode::Translate;
            }
            else if (mouseEvent->modifiers() & Qt::ControlModifier)
            {
                mIsDragging = true;
                mDragStartPosition = mouseEvent->pos();
                mCurrentDragMode = DragMode::Scale;
            }
            else
                return false;
        }
    }

    // Detect Drag End with Left Mouse Button release
    if (pEvent->type() == QEvent::MouseButtonRelease && mIsDragging)
    {
        double scaleFactor = defaultGLGridSpacing / getHookedLastGridSpacing(qobject_cast<QOpenGLWidget *>(obj));

        if (mCurrentDragMode == DragMode::Translate)
        {
            // Calculate the drag vector in the UI
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(pEvent);
            QPoint dragEndPosition = mouseEvent->pos();
            QPoint dragVectorInGLWidget = dragEndPosition - mDragStartPosition;

            // Calculate the translate offset in relation view coordinates
            QPoint relationViewTopLeftPosOffset;
            if (unprojectGLWidgetToRelationView(qobject_cast<QOpenGLWidget *>(obj), (-1) * dragVectorInGLWidget, relationViewTopLeftPosOffset, scaleFactor))
            {
                // Get the current top-left position of the relation view and apply the offset
                QPoint currentTopLeftPos = RelationDialogManager::getInstance().getRelationViewTopLeftPos();
                currentTopLeftPos += relationViewTopLeftPosOffset;

                // Update the relation view state
                RelationDialogManager::getInstance().setRelationViewState(currentTopLeftPos, scaleFactor);
            }
        }

        else if (mCurrentDragMode == DragMode::Scale)
        {
            // Calculate the drag start position in relation view coordinates
            QPoint dragStartPosInRelationView;
            if (unprojectGLWidgetToRelationView(qobject_cast<QOpenGLWidget *>(obj), mDragStartPosition, dragStartPosInRelationView))
            {
                // Get the current relation view state
                double lastScaleFactor = RelationDialogManager::getInstance().getLastRelationViewScaleFactor();
                QPoint currentTopLeftPos = RelationDialogManager::getInstance().getRelationViewTopLeftPos();

                // Calculate the scale offset and apply it to the top-left position
                currentTopLeftPos += (lastScaleFactor - scaleFactor) * dragStartPosInRelationView;

                // Update the relation view state
                RelationDialogManager::getInstance().setRelationViewState(currentTopLeftPos, scaleFactor);
            }
        }

        // Reset the dragging flag
        mIsDragging = false;
    }

    return false;
}

void RelationOpenGLWidgetFilter::getRelationBoxRectTopLeft(FBConstraintRelation *relation, QPoint &outTopLeft)
{
    if (!relation || relation->Boxes.GetCount() == 0)
        return;

    int leftMin, topMin;
    relation->GetBoxPosition(relation->Boxes[0], leftMin, topMin);

    for (int i = 1; i < relation->Boxes.GetCount(); ++i)
    {
        FBBox *box = relation->Boxes[i];
        if (box)
        {
            int relationX, relationY;
            relation->GetBoxPosition(box, relationX, relationY);

            if (relationX < leftMin)
                leftMin = relationX;

            if (relationY < topMin)
                topMin = relationY;
        }
    }

    outTopLeft = QPoint(leftMin, topMin);
}

void RelationOpenGLWidgetFilter::onKeyAPressed(QOpenGLWidget *widget)
{
    if (!widget)
        return;

    FBConstraintRelation *relation = RelationDialogManager::getInstance().getLastSelectedRelationConstraint();
    if (!relation)
        return;

    if (relation->Boxes.GetCount() > 0)
    {
        QPoint viewportCenter = widget->rect().center();
        QPoint viewportCenterInRelationView;
        double scaleFactor = defaultGLGridSpacing / getHookedLastGridSpacing(widget);

        if (unprojectGLWidgetToRelationView(widget, viewportCenter, viewportCenterInRelationView, scaleFactor))
        {
            // Get the bounding box of all boxes in the relation view
            std::array<double, 4> boxMinTopLeftMaxRightBottom = getHookedBoxRectRange(widget);

            // Calculate the center vector from the top-left corner of the bounding box
            QPoint centerFromTopLeft(
                ((boxMinTopLeftMaxRightBottom[0] + boxMinTopLeftMaxRightBottom[2]) / 2.0 - boxMinTopLeftMaxRightBottom[0]) * relationViewGridSpacing / defaultGLGridSpacing * scaleFactor,
                ((boxMinTopLeftMaxRightBottom[1] + boxMinTopLeftMaxRightBottom[3]) / 2.0 - boxMinTopLeftMaxRightBottom[1]) * relationViewGridSpacing / defaultGLGridSpacing * scaleFactor);

            // Get previous top-left position of the relation view
            QPoint topLeft;
            getRelationBoxRectTopLeft(RelationDialogManager::getInstance().getLastSelectedRelationConstraint(), topLeft);

            // Calculate the new center position in the relation view coordinates
            QPoint currentWorldCenter = topLeft + centerFromTopLeft;

            // Update the relation view state
            RelationDialogManager::getInstance().setRelationViewState(currentWorldCenter - viewportCenterInRelationView, scaleFactor);
        }
    }
    else
    {
        QPoint viewportCenter = widget->rect().center();
        QPoint viewportCenterInRelationView;

        if (unprojectGLWidgetToRelationView(widget, viewportCenter, viewportCenterInRelationView))
        {
            // The new center position will be about (0,0) in relation view coordinates.
            // Update the relation view state
            RelationDialogManager::getInstance().setRelationViewState((-1) * viewportCenterInRelationView, 1.0);
        }
    }
}