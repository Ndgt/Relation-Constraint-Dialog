#pragma once

#include "ui_PreferencesDialog.h"

#include <QtWidgets/QDialog>
#include <QtWidgets/QWidget>

/**
 * @class PreferencesDialog
 * @brief Dialog for setting user preferences for the Relation Constraint Dialog
 */
class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @details Sets up the UI elements and reads the current configuration.
     * @param parent The parent widget
     */
    explicit PreferencesDialog(QWidget *parent = nullptr);

    /**
     * @brief Destructor
     * @details Deletes the pointer to the Widget Container class generated from the .ui file.
     */
    ~PreferencesDialog() { delete ui; }

private slots:
    /**
     * @brief Handle accepted signal of the dialog
     * @details This slot is called when the dialog is accepted, and it writes the current settings
     *          to the config file.
     */
    void onAccepted() { writeConfig(); }

private:
    /**
     * @brief Read the current configuration from the config file
     */
    void readConfig();

    /**
     * @brief Write the current settings to the config file
     */
    void writeConfig();

private:
    Ui::PreferencesDialog *ui; //!< Pointer to the Widget Container class generated from the .ui file
};
