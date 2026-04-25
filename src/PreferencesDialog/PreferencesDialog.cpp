#include "PreferencesDialog.h"
#include "RelationDialogManager.h"
#include <fbsdk/fbsdk.h>

PreferencesDialog::PreferencesDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &PreferencesDialog::onAccepted);

    readConfig();
}

void PreferencesDialog::readConfig()
{
    std::filesystem::path configFilePath = RelationDialogManager::getInstance().getConfigFilePath();
    if (std::filesystem::exists(configFilePath))
    {
        FBConfigFile configFile = FBConfigFile(configFilePath.string().c_str());

        std::string saveToConfiFile = configFile.Get("General", "Save to config file", "No");
        mSaveToConfigFile = (saveToConfiFile == "Yes");

        std::string showHitOperatorFirst =
            configFile.Get("Operator Search Options", "Show Hit Operator First", "Yes");

        if (showHitOperatorFirst == "Yes")
            ui->radioButtonOperator->setChecked(true);
        else
            ui->radioButtonCategory->setChecked(true);

        std::string doNotSearchInNamespaces = configFile.Get("Model Search Options", "Do Not Search In Namespaces", "No");

        ui->checkBoxNotSearchInNamespaces->setChecked(doNotSearchInNamespaces == "Yes");

        std::string searchFilterCameras = configFile.Get("Model Search Filter", "Cameras", "Yes");
        ui->checkBoxCameras->setChecked(searchFilterCameras == "Yes");

        std::string searchFilterNulls = configFile.Get("Model Search Filter", "Nulls", "Yes");
        ui->checkBoxNulls->setChecked(searchFilterNulls == "Yes");

        std::string searchFilterRoots = configFile.Get("Model Search Filter", "Roots", "Yes");
        ui->checkBoxRoots->setChecked(searchFilterRoots == "Yes");

        std::string searchFilterSkeletons = configFile.Get("Model Search Filter", "Skeletons", "Yes");
        ui->checkBoxSkeletons->setChecked(searchFilterSkeletons == "Yes");

        std::string searchFilterMarkers = configFile.Get("Model Search Filter", "Markers", "Yes");
        ui->checkBoxMarkers->setChecked(searchFilterMarkers == "Yes");

        std::string searchFilterOpticals = configFile.Get("Model Search Filter", "Opticals", "Yes");
        ui->checkBoxOpticals->setChecked(searchFilterOpticals == "Yes");

        std::string searchFilterCameraSwitchers = configFile.Get("Model Search Filter", "CameraSwitchers", "No");
        ui->checkBoxCameraSwitchers->setChecked(searchFilterCameraSwitchers == "Yes");

        std::string searchFilterCubes = configFile.Get("Model Search Filter", "Cubes", "No");
        ui->checkBoxCubes->setChecked(searchFilterCubes == "Yes");

        std::string searchFilterPlanes = configFile.Get("Model Search Filter", "Planes", "No");
        ui->checkBoxPlanes->setChecked(searchFilterPlanes == "Yes");

        std::string searchFilterLights = configFile.Get("Model Search Filter", "Lights", "No");
        ui->checkBoxLights->setChecked(searchFilterLights == "Yes");

        std::string searchFilterPath3Ds = configFile.Get("Model Search Filter", "Path3Ds", "No");
        ui->checkBoxPath3Ds->setChecked(searchFilterPath3Ds == "Yes");
    }
}

void PreferencesDialog::writeConfig()
{
    std::filesystem::path configFilePath = RelationDialogManager::getInstance().getConfigFilePath();
    if (std::filesystem::exists(configFilePath))
    {
        FBConfigFile configFile = FBConfigFile(configFilePath.string().c_str());

        configFile.Set("Operator Search Options", "Show Hit Operator First", ui->radioButtonOperator->isChecked() ? "Yes" : "No");

        configFile.Set("Model Search Options", "Do Not Search In Namespaces", ui->checkBoxNotSearchInNamespaces->isChecked() ? "Yes" : "No");

        configFile.Set("Model Search Filter", "Cameras", ui->checkBoxCameras->isChecked() ? "Yes" : "No");
        configFile.Set("Model Search Filter", "Nulls", ui->checkBoxNulls->isChecked() ? "Yes" : "No");
        configFile.Set("Model Search Filter", "Roots", ui->checkBoxRoots->isChecked() ? "Yes" : "No");
        configFile.Set("Model Search Filter", "Skeletons", ui->checkBoxSkeletons->isChecked() ? "Yes" : "No");
        configFile.Set("Model Search Filter", "Markers", ui->checkBoxMarkers->isChecked() ? "Yes" : "No");
        configFile.Set("Model Search Filter", "Opticals", ui->checkBoxOpticals->isChecked() ? "Yes" : "No");
        configFile.Set("Model Search Filter", "Camera Switchers", ui->checkBoxCameraSwitchers->isChecked() ? "Yes" : "No");
        configFile.Set("Model Search Filter", "Cubes", ui->checkBoxCubes->isChecked() ? "Yes" : "No");
        configFile.Set("Model Search Filter", "Planes", ui->checkBoxPlanes->isChecked() ? "Yes" : "No");
        configFile.Set("Model Search Filter", "Lights", ui->checkBoxLights->isChecked() ? "Yes" : "No");
        configFile.Set("Model Search Filter", "Path3Ds", ui->checkBoxPath3Ds->isChecked() ? "Yes" : "No");
    }
}