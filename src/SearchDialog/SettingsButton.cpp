#include "SettingsButton.h"

SettingsMenu::SettingsMenu(QWidget *parent) : QMenu(parent)
{
    addAction("Option 1");
    addAction("Option 2");
    addAction("Option 3");
}

SettingsButton::SettingsButton(QWidget *parent) : QPushButton(parent)
{
    setMenu(new SettingsMenu(this));
}