#include "ConfigReadWriter.h"

#include <string>

#include <fbsdk/fbsdk.h>

const static std::string CONFIG_FILE_NAME = "RelationConstraintDialogConfig.ini";

std::filesystem::path ConfigReadWriter::configFilePath()
{
    // The config file is expected to be located in the user's config directory
    // (e.g. '%USERPROFILE%\Documents\MB\<version>\config')

    std::string configFileDir = FBSystem::TheOne().UserConfigPath.AsString();

    return std::filesystem::path(configFileDir) / CONFIG_FILE_NAME;
}

bool ConfigReadWriter::configFileExists()
{
    return std::filesystem::exists(configFilePath());
}

RelationDialogConfig ConfigReadWriter::readConfig()
{
    RelationDialogConfig config;

    // If the config file does not exist, create one with default values
    // and return the default config
    if (!configFileExists())
    {
        writeConfig(config);
        return config;
    }

    FBConfigFile configFile = FBConfigFile(configFilePath().filename().string().c_str(),
                                           configFilePath().parent_path().string().c_str());

    // Note: The third parameter of the `FBConfigFile::Get` function behaves as the default value
    //       in case the config file or the specific item is not found

    std::string showHitOperatorFirst = configFile.Get("Operator Search Options", "Show Hit Operator First", "Yes");
    config.operatorSearchPriority = (showHitOperatorFirst == "Yes") ? OperatorSearchPriority::OperatorFirst : OperatorSearchPriority::CategoryFirst;

    std::string doNotSearchInNamespaces = configFile.Get("Model Search Options", "Do Not Search In Namespaces", "No");
    config.modelNamespaceSearchDisabled = (doNotSearchInNamespaces == "Yes");

    auto readModelFilter = [&](const char *key, ModelSearchFilter flag, const char *defaultValue)
    {
        const std::string value = configFile.Get("Model Search Filter", key, defaultValue);
        config.modelSearchFilters.setFlag(flag, value == "Yes");
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

    return config;
}

bool ConfigReadWriter::writeConfig(const RelationDialogConfig &config)
{
    FBConfigFile configFile = FBConfigFile(configFilePath().filename().string().c_str(),
                                           configFilePath().parent_path().string().c_str());

    bool anyWriteFailed = false;

    auto writeConfigItem = [&](const char *section, const char *key, const std::string &value)
    {
        bool success = configFile.Set(section, key, value.c_str());
        if (!success)
            anyWriteFailed = true;
    };

    writeConfigItem("Operator Search Options", "Show Hit Operator First",
                    (config.operatorSearchPriority == OperatorSearchPriority::OperatorFirst) ? "Yes" : "No");

    writeConfigItem("Model Search Options", "Do Not Search In Namespaces",
                    config.modelNamespaceSearchDisabled ? "Yes" : "No");

    auto writeModelFilter = [&](const char *key, ModelSearchFilter flag)
    {
        writeConfigItem("Model Search Filter", key, config.modelSearchFilters.testFlag(flag) ? "Yes" : "No");
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

    return !anyWriteFailed;
}