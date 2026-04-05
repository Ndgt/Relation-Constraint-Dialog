#pragma once

#include <QtWidgets/QMenu>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

class SettingsMenu : public QMenu
{
    Q_OBJECT

public:
    explicit SettingsMenu(QWidget *parent = nullptr);
};

class SettingsButton : public QPushButton
{
    Q_OBJECT

public:
    explicit SettingsButton(QWidget *parent = nullptr);
};