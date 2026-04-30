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
     * @brief Get the defined path to the config file
     * @return The path where the config file is expected to be located
     */
    static std::filesystem::path configFilePath();

    /**
     * @brief Check if the config file exists in the expected location
     * @return True if the config file exists, false otherwise
     */
    static bool configFileExists();

    /**
     * @brief Read the configuration from the config file
     * @return The RelationDialogConfig struct populated with the settings from the config file
     */
    static RelationDialogConfig readConfig();

    /**
     * @brief Write the given configuration to the config file
     * @param config The RelationDialogConfig struct containing the settings to be written to the config file
     * @return True if the configuration was written successfully, false otherwise
     */
    static bool writeConfig(const RelationDialogConfig &config);
};