#include "SearchBoxLineEdit.h"

#include <QtCore/QtConfig>
#include <QtGui/QColor>
#include <QtGui/QPalette>

SearchBoxLineEdit::SearchBoxLineEdit(QWidget *parent) : QLineEdit(parent)
{
    // Fix default placeholder text color (Qt6 issue)
#if QT_VERSION_MAJOR == 6
    QPalette palette = this->palette();
    palette.setColor(QPalette::ColorRole::PlaceholderText, QColor("#c8c8c8"));
    setPalette(palette);
#endif
}

void SearchBoxLineEdit::keyPressEvent(QKeyEvent *event)
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