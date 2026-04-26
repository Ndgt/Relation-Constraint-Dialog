#include "ConfigReadWriter.h"

#include <fbsdk/fbsdk.h>

RelationDialogConfig ConfigReadWriter::readConfig(const std::filesystem::path &configFilePath)
{
    RelationDialogConfig config;

    FBConfigFile configFile = FBConfigFile(configFilePath.string().c_str());

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

void ConfigReadWriter::writeConfig(const std::filesystem::path &configFilePath, const RelationDialogConfig &config)
{
    FBConfigFile configFile = FBConfigFile(configFilePath.string().c_str());

    configFile.Set("Operator Search Options", "Show Hit Operator First", (config.operatorSearchPriority == OperatorSearchPriority::OperatorFirst) ? "Yes" : "No");
    configFile.Set("Model Search Options", "Do Not Search In Namespaces", config.modelNamespaceSearchDisabled ? "Yes" : "No");

    auto writeModelFilter = [&](const char *key, ModelSearchFilter flag)
    {
        configFile.Set("Model Search Filter", key, config.modelSearchFilters.testFlag(flag) ? "Yes" : "No");
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
}