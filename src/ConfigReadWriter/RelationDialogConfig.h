#pragma once

#include <QtCore/QFlags>
#include <QtCore/QtTypes>

/**
 * @enum OperatorSearchPriority
 * @brief Enum to specify the search priority for operators in SearchDialog
 */
enum class OperatorSearchPriority
{
    CategoryFirst,
    OperatorFirst
};

/**
 * @enum ModelSearchFilter
 * @brief Enum to specify the search filter for models in SearchDialog
 */
enum class ModelSearchFilter : quint32
{
    None = 0x0000,
    FBModelObjects = 0x0001,
    Cameras = 0x0002,
    CameraSwitchers = 0x0004,
    Cubes = 0x0008,
    Lights = 0x0010,
    Markers = 0x0020,
    Nulls = 0x0040,
    Opticals = 0x0080,
    Path3Ds = 0x0100,
    Planes = 0x0200,
    Roots = 0x0400,
    Skeletons = 0x0800
};

Q_DECLARE_FLAGS(ModelSearchFilters, ModelSearchFilter)
Q_DECLARE_OPERATORS_FOR_FLAGS(ModelSearchFilters)

/**
 * @struct RelationDialogConfig
 * @brief Struct to hold the configuration settings for the Relation Constraint Dialog
 */
struct RelationDialogConfig
{
    OperatorSearchPriority operatorSearchPriority = OperatorSearchPriority::OperatorFirst;
    bool modelNamespaceSearchDisabled = false;
    ModelSearchFilters modelSearchFilters = ModelSearchFilter::None;
};