#include "CustomLineEdit.h"

void CustomLineEdit::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();

    if (key == Qt::Key_Return || key == Qt::Key_Enter)
        emit keyReturnPressed();

    else if (key == Qt::Key_Tab)
        emit keyTabPressed();

    else if (key == Qt::Key_Up || key == Qt::Key_Down)
        emit keyUpDownPressed(key);

    else
        QLineEdit::keyPressEvent(event);
}