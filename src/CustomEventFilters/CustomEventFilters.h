#ifndef CUSTOM_EVENTFILTERS_H
#define CUSTOM_EVENTFILTERS_H

#include <QtCore/QtConfig>
#include <QtCore/QEvent>
#include <QtCore/QObject>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

#if QT_VERSION_MAJOR >= 6
#include <QtOpenGLWidgets/QOpenGLWidget>
#else
#include <QtWidgets/QOpenGLWidget>
#endif

#include <fbsdk/fbsdk.h>

/*
 * Qt event filter for the MotionBuilder main window.
 *
 * - Detects when dock widgets (e.g., Navigator or Constraints) are added to the main window
 *   and installs a ConstraintsNavigatorFilter on them.
 * - When the Navigator is docked to the main window, it intercepts Tab key presses
 *   and opens the SearchDialog if the mouse is over the corresponding QOpenGLWidget.
 */
class MainWindowFilter : public QObject
{
    Q_OBJECT

public:
    MainWindowFilter(QObject *parent = nullptr);
    QOpenGLWidget *getRelationOpenGLWidget();

protected:
    bool eventFilter(QObject *obj, QEvent *pEvent) override;
};

/*
 * Event filter for Navigator or Filtered Constraints Navigator.
 *
 * - Intercepts Tab key events and opens the SearchDialog
 *   if the mouse cursor is over the window's QOpenGLWidget.
 */
class ConstraintsNavigatorFilter : public QObject
{
    Q_OBJECT
public:
    ConstraintsNavigatorFilter(QObject *parent = nullptr);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

/* Returns a singleton instance of ConstraintsNavigatorFilter
 * Only one corresponding QOpenGLWidget is displayed even if multiple windows are open.
 */
ConstraintsNavigatorFilter *getNavigatorConstraintsFilter();

/*
 * Returns a pointer to the main QMainWindow instance of the application.
 *
 * - This function is implemented as a workaround for MotionBuilder 2020,
 *    since the function 'FBGetMainWindow' was only introduced in MotionBuilder 2022.
 *
 * - It iterates over all top-level widgets to find the first QMainWindow instance.
 *   Returns nullptr if no main window is found.
 */
QMainWindow *getMobuMainWindow();

#endif // CUSTOM_EVENTFILTERS_H