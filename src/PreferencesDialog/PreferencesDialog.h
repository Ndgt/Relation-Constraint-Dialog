#pragma once

#include "ui_PreferencesDialog.h"

#include <QtWidgets/QDialog>

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = nullptr);
    ~PreferencesDialog() { delete ui; }

private:
    Ui::PreferencesDialog *ui; //!< Pointer to the Widget Container class generated from the .ui file
};
