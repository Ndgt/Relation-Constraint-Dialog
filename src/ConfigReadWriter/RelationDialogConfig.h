#pragma once

#include <QtCore/QFlags>
#include <QtCore/QtTypes>

/**
 * @enum OperatorSearchPriority
 * @brief Enum to specify the search priority for operators in SearchDialog
 */
enum class OperatorSearchPriority
{
    CategoryFirst, //!< Search operator categories first, then operator names
    OperatorFirst  //!< Search operator names first, then operator categories
};

/**
 * @enum ModelSearchFilter
 * @brief Enum to specify the search filter for models in SearchDialog
 */
enum class ModelSearchFilter : quint32
{
    None = 0x0000,
    FBModelObjects = 0x0001,  //!< FBModel Class objects
    Cameras = 0x0002,         //!< FBCamera Class objects
    CameraSwitchers = 0x0004, //!< FBCameraSwitcher Class objects
    Cubes = 0x0008,           //!< FBModelCube Class objects
    Lights = 0x0010,          //!< FBLight Class objects
    Markers = 0x0020,         //!< FBModelMarker Class objects
    Nulls = 0x0040,           //!< FBModelNull Class objects
    Opticals = 0x0080,        //!< FBModelOptical Class objects
    Path3Ds = 0x0100,         //!< FBModelPath3D Class objects
    Planes = 0x0200,          //!< FBModelPlane Class objects
    Roots = 0x0400,           //!< FBModelRoot Class objects
    Skeletons = 0x0800        //!< FBModelSkeleton Class objects
};

Q_DECLARE_FLAGS(ModelSearchFilters, ModelSearchFilter)
Q_DECLARE_OPERATORS_FOR_FLAGS(ModelSearchFilters)

/**
 * @struct RelationDialogConfig
 * @brief Struct to hold the configuration settings for the Relation Constraint Dialog
 */
struct RelationDialogConfig
{
    /// The search priority for operators in the SearchDialog
    OperatorSearchPriority operatorSearchPriority = OperatorSearchPriority::OperatorFirst;

    /// Whether to disable namespace search for models in the SearchDialog
    bool modelNamespaceSearchDisabled = false;

    /// The search filters for models in the SearchDialog
    ModelSearchFilters modelSearchFilters = ModelSearchFilter::None;
};