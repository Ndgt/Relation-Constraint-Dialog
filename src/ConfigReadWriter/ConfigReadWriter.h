#pragma once

#include <filesystem>

#include "RelationDialogConfig.h"

/**
 * @class ConfigReadWriter
 * @brief Class responsible for reading and writing the Relation Constraint Dialog configuration to a config file
 */
class ConfigReadWriter
{
public:
    /**
     * @brief Read the configuration from the config file
     * @param configFilePath The path to the config file
     * @return The RelationDialogConfig struct populated with the settings from the config file
     */
    static RelationDialogConfig readConfig(const std::filesystem::path &configFilePath);

    /**
     * @brief Write the given configuration to the config file
     * @param configFilePath The path to the config file
     * @param config The RelationDialogConfig struct containing the settings to be written to the config file
     */
    static void writeConfig(const std::filesystem::path &configFilePath, const RelationDialogConfig &config);
};