#include "PreferencesDialog.h"
#include "RelationDialogManager.h"
#include "SuggestionProvider.h"
#include <fbsdk/fbsdk.h>

PreferencesDialog::PreferencesDialog(QWidget *parent) : QDialog(parent), ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &PreferencesDialog::onAccepted);

    readConfig();
}

void PreferencesDialog::readConfig()
{
    std::filesystem::path configFilePath = RelationDialogManager::getInstance().getConfigFilePath();
    if (!std::filesystem::exists(configFilePath))
        return;

    FBConfigFile configFile = FBConfigFile(configFilePath.string().c_str());

    std::string showHitOperatorFirst = configFile.Get("Operator Search Options", "Show Hit Operator First", "Yes");

    if (showHitOperatorFirst == "Yes")
        ui->radioButtonOperator->setChecked(true);
    else
        ui->radioButtonCategory->setChecked(true);

    std::string doNotSearchInNamespaces = configFile.Get("Model Search Options", "Do Not Search In Namespaces", "No");
    ui->checkBoxNotSearchInNamespaces->setChecked(doNotSearchInNamespaces == "Yes");

    ModelSearchFilters modelFilters;

    auto readModelFilter = [&](const char *key, ModelSearchFilter flag, const char *defaultValue)
    {
        const std::string value = configFile.Get("Model Search Filter", key, defaultValue);
        modelFilters.setFlag(flag, value == "Yes");
    };

    readModelFilter("FBModel Objects", ModelSearchFilter::FBModelObjects, "Yes");
    readModelFilter("Cameras", ModelSearchFilter::Cameras, "Yes");
    readModelFilter("Camera Switchers", ModelSearchFilter::CameraSwitchers, "Yes");
    readModelFilter("Cubes", ModelSearchFilter::Cubes, "Yes");
    readModelFilter("Lights", ModelSearchFilter::Lights, "Yes");
    readModelFilter("Markers", ModelSearchFilter::Markers, "Yes");
    readModelFilter("Nulls", ModelSearchFilter::Nulls, "Yes");
    readModelFilter("Opticals", ModelSearchFilter::Opticals, "Yes");
    readModelFilter("Path3Ds", ModelSearchFilter::Path3Ds, "Yes");
    readModelFilter("Planes", ModelSearchFilter::Planes, "Yes");
    readModelFilter("Roots", ModelSearchFilter::Roots, "Yes");
    readModelFilter("Skeletons", ModelSearchFilter::Skeletons, "Yes");

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

    SuggestionProvider &suggestionProvider = SuggestionProvider::getInstance();
    suggestionProvider.setOperatorSearchPriority(
        ui->radioButtonOperator->isChecked() ? OperatorSearchPriority::OperatorFirst : OperatorSearchPriority::CategoryFirst);

    suggestionProvider.setModelNamespaceSearchDisabled(ui->checkBoxNotSearchInNamespaces->isChecked());

    suggestionProvider.setModelSearchFilters(modelFilters);
}

void PreferencesDialog::writeConfig()
{
    std::filesystem::path configFilePath = RelationDialogManager::getInstance().getConfigFilePath();
    FBConfigFile configFile = FBConfigFile(configFilePath.string().c_str());

    configFile.Set("Operator Search Options", "Show Hit Operator First", ui->radioButtonOperator->isChecked() ? "Yes" : "No");
    configFile.Set("Model Search Options", "Do Not Search In Namespaces", ui->checkBoxNotSearchInNamespaces->isChecked() ? "Yes" : "No");

    ModelSearchFilters modelFilters;

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

    auto writeModelFilter = [&](const char *key, ModelSearchFilter flag)
    {
        configFile.Set("Model Search Filter", key, modelFilters.testFlag(flag) ? "Yes" : "No");
    };

    writeModelFilter("FBModel Objects", ModelSearchFilter::FBModelObjects);
    writeModelFilter("Cameras", ModelSearchFilter::Cameras);
    writeModelFilter("Camera Switchers", ModelSearchFilter::CameraSwitchers);
    writeModelFilter("Cubes", ModelSearchFilter::Cubes);
    writeModelFilter("Lights", ModelSearchFilter::Lights);
    writeModelFilter("Markers", ModelSearchFilter::Markers);
    writeModelFilter("Nulls", ModelSearchFilter::Nulls);
    writeModelFilter("Opticals", ModelSearchFilter::Opticals);
    writeModelFilter("Path3Ds", ModelSearchFilter::Path3Ds);
    writeModelFilter("Planes", ModelSearchFilter::Planes);
    writeModelFilter("Roots", ModelSearchFilter::Roots);
    writeModelFilter("Skeletons", ModelSearchFilter::Skeletons);

    SuggestionProvider &suggestionProvider = SuggestionProvider::getInstance();
    suggestionProvider.setOperatorSearchPriority(
        ui->radioButtonOperator->isChecked() ? OperatorSearchPriority::OperatorFirst : OperatorSearchPriority::CategoryFirst);

    suggestionProvider.setModelNamespaceSearchDisabled(ui->checkBoxNotSearchInNamespaces->isChecked());

    suggestionProvider.setModelSearchFilters(modelFilters);
}