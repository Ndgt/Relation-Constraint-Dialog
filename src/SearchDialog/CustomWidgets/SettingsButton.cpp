#include "SettingsButton.h"

SettingsMenu::SettingsMenu(QWidget *parent) : QMenu(parent)
{
    addAction("Option 1");
    addAction("Option 2");
    addAction("Option 3");
}

SettingsButton::SettingsButton(QWidget *parent) : QPushButton(parent)
{
}

void SettingsButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        SettingsMenu menu(this);
        menu.exec(mapToGlobal(rect().topRight()));
    }
}