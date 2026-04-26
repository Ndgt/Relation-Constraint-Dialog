#pragma once

#include <QtGui/QKeyEvent>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>

/**
 * @class CustomLineEdit
 * @brief Custom QLineEdit that emits signals for specific key presses
 */
class CustomLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for CustomLineEdit
     * @param parent The parent widget
     */
    explicit CustomLineEdit(QWidget *parent = nullptr) : QLineEdit(parent) {}

protected:
    /**
     * @brief Overrides the keyPressEvent to emit custom signals for specific keys
     * @param event The key event
     */
    virtual void keyPressEvent(QKeyEvent *event) override;

signals:
    /**
     * @brief Signal emitted when the Return or Enter key is pressed
     */
    void keyReturnPressed();

    /**
     * @brief Signal emitted when the Tab key is pressed
     */
    void keyTabPressed();

    /**
     * @brief Signal emitted when the Up or Down arrow key is pressed
     * @param key The key code (Qt::Key_Up or Qt::Key_Down)
     */
    void keyUpDownPressed(int key);
};