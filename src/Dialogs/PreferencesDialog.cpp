#include "PreferencesDialog.h"

#include <fbsdk/fbsdk.h>

#include "ConfigReadWriter.h"
#include "SuggestionProvider.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) : QDialog(parent), ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &PreferencesDialog::onAccepted);

    if (ConfigReadWriter::configFileExists())
    {
        ui->lineEditConfigFilePath->setPathText(
            QString::fromStdString(ConfigReadWriter::configFilePath().string()));
    }

    // Read the current configuration and update the UI to reflect it
    updateUIFromConfig(ConfigReadWriter::readConfig());
}

void PreferencesDialog::updateUIFromConfig(const RelationDialogConfig &config)
{
    ui->radioButtonOperator->setChecked(config.operatorSearchPriority == OperatorSearchPriority::OperatorFirst);
    ui->radioButtonCategory->setChecked(config.operatorSearchPriority == OperatorSearchPriority::CategoryFirst);
    ui->checkBoxNotSearchInNamespaces->setChecked(config.modelNamespaceSearchDisabled);

    const ModelSearchFilters &modelFilters = config.modelSearchFilters;

    ui->checkBoxFBModelObjetcs->setChecked(modelFilters.testFlag(ModelSearchFilter::FBModelObjects));
    ui->checkBoxCameras->setChecked(modelFilters.testFlag(ModelSearchFilter::Cameras));
    ui->checkBoxCameraSwitchers->setChecked(modelFilters.testFlag(ModelSearchFilter::CameraSwitchers));
    ui->checkBoxCubes->setChecked(modelFilters.testFlag(ModelSearchFilter::Cubes));
    ui->checkBoxLights->setChecked(modelFilters.testFlag(ModelSearchFilter::Lights));
    ui->checkBoxMarkers->setChecked(modelFilters.testFlag(ModelSearchFilter::Markers));
    ui->checkBoxNulls->setChecked(modelFilters.testFlag(ModelSearchFilter::Nulls));
    ui->checkBoxOpticals->setChecked(modelFilters.testFlag(ModelSearchFilter::Opticals));
    ui->checkBoxPath3Ds->setChecked(modelFilters.testFlag(ModelSearchFilter::Path3Ds));
    ui->checkBoxPlanes->setChecked(modelFilters.testFlag(ModelSearchFilter::Planes));
    ui->checkBoxRoots->setChecked(modelFilters.testFlag(ModelSearchFilter::Roots));
    ui->checkBoxSkeletons->setChecked(modelFilters.testFlag(ModelSearchFilter::Skeletons));
}

RelationDialogConfig PreferencesDialog::gatherConfigFromUI() const
{
    RelationDialogConfig config;

    config.operatorSearchPriority = ui->radioButtonOperator->isChecked() ? OperatorSearchPriority::OperatorFirst : OperatorSearchPriority::CategoryFirst;
    config.modelNamespaceSearchDisabled = ui->checkBoxNotSearchInNamespaces->isChecked();

    ModelSearchFilters &modelFilters = config.modelSearchFilters;

    modelFilters.setFlag(ModelSearchFilter::FBModelObjects, ui->checkBoxFBModelObjetcs->isChecked());
    modelFilters.setFlag(ModelSearchFilter::Cameras, ui->checkBoxCameras->isChecked());
    modelFilters.setFlag(ModelSearchFilter::CameraSwitchers, ui->checkBoxCameraSwitchers->isChecked());
    modelFilters.setFlag(ModelSearchFilter::Cubes, ui->checkBoxCubes->isChecked());
    modelFilters.setFlag(ModelSearchFilter::Lights, ui->checkBoxLights->isChecked());
    modelFilters.setFlag(ModelSearchFilter::Markers, ui->checkBoxMarkers->isChecked());
    modelFilters.setFlag(ModelSearchFilter::Nulls, ui->checkBoxNulls->isChecked());
    modelFilters.setFlag(ModelSearchFilter::Opticals, ui->checkBoxOpticals->isChecked());
    modelFilters.setFlag(ModelSearchFilter::Path3Ds, ui->checkBoxPath3Ds->isChecked());
    modelFilters.setFlag(ModelSearchFilter::Planes, ui->checkBoxPlanes->isChecked());
    modelFilters.setFlag(ModelSearchFilter::Roots, ui->checkBoxRoots->isChecked());
    modelFilters.setFlag(ModelSearchFilter::Skeletons, ui->checkBoxSkeletons->isChecked());

    return config;
}

void PreferencesDialog::onAccepted()
{
    RelationDialogConfig config = gatherConfigFromUI();

    // Update the suggestion provider with the new configuration
    SuggestionProvider::getInstance().applyConfig(config);

    // Save the new configuration to the config file
    if (!ConfigReadWriter::writeConfig(config))
    {
        FBMessageBox("Relation Constraint Dialog",
                     "[Error] One or more settings failed to save to the config file.",
                     "OK");
    }
}