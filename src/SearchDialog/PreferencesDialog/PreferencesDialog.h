#pragma once

#include "ui_PreferencesDialog.h"

#include <QtWidgets/QDialog>
#include <QtWidgets/QWidget>

#include "RelationDialogConfig.h"

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
    void onAccepted();

private:
    /**
     * @brief Update the widgets to reflect the given configuration
     * @param config The configuration to update the UI with
     */
    void updateUIFromConfig(const RelationDialogConfig &config);

    /**
     * @brief Gather the current configuration from the widgets
     * @return The configuration gathered from the UI
     */
    RelationDialogConfig gatherConfigFromUI() const;

private:
    Ui::PreferencesDialog *ui; //!< Pointer to the Widget Container class generated from the .ui file
};
