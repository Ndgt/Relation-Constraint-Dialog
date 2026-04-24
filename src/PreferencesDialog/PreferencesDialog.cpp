#include "PreferencesDialog.h"
#include "RelationDialogManager.h"
#include <fbsdk/fbsdk.h>

PreferencesDialog::PreferencesDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);

    std::filesystem::path configFilePath = RelationDialogManager::getInstance().getConfigFilePath();

    if (std::filesystem::exists(configFilePath))
    {
        FBConfigFile configFile = FBConfigFile(configFilePath.string().c_str());

        ui->checkBoxConfigFile->setChecked(true);

        std::string showHitCategoryFirst =
            configFile.Get("OperatorSearchOptions", "Show Hit Category First", "Yes");

        if (showHitCategoryFirst == "Yes")
        {
            ui->radioButtonCategory->setChecked(true);
        }
        else
        {
            ui->radioButtonOperator->setChecked(true);
        }
    }
}