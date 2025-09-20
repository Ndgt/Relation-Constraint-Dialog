#pragma once

#include <QtCore/QSet>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include <fbsdk/fbsdk.h>

#if PRODUCT_VERSION == 2020
// Since Mobu SDK sets the C4946 warning as an error,
// "error C4946: reinterpret_cast used between related classes: 'QMapNodeBase' and 'QMapNode<Key, T>'"
// will be triggered when we use Qt 5.12.5 and MSVC 2017. So we disable this warning here.
#pragma warning(disable : 4946)
#endif

/**
 * @class SuggestionProvider
 * @brief Singleton class that collects and provides suggestion data for SearchDialog
 */
class SuggestionProvider
{
public:
    /**
     * @brief Get collected model LongNames in the scene
     * @return List of LongName suggestions
     * @note The list is sorted as ascending, case-insensitive.
     */
    QStringList getModelSuggestions();

    /**
     * @brief Get collected operator names
     * @param relation Pointer to the FBConstraintRelation which is target of the SearchDialog
     * @return List of operator name suggestions
     */
    QStringList getOperatorSuggestions(FBConstraintRelation *relation);

    /**
     * @brief Get the singleton instance of SuggestionProvider
     * @return Reference to the singleton instance
     */
    static SuggestionProvider &getInstance()
    {
        static SuggestionProvider instance;
        return instance;
    }

private:
    /**
     * @brief Singleton constructor
     * @details Collects operator names for display upon initialization.
     */
    SuggestionProvider() { collectDefaultOperatorNamesForDisplay(); }

    /// @cond
    SuggestionProvider(const SuggestionProvider &) = delete;
    SuggestionProvider &operator=(const SuggestionProvider &) = delete;
    /// @endcond

    /**
     * @brief Helper function recursively collect model LongNames starting from the given model
     * @param model Pointer to the starting FBModel
     * @param nameSet Set to store unique model LongNames
     */
    void collectModelLongNamesRecursive(FBModel *model, QSet<QString> &nameSet);

    /**
     * @brief Collect all default relation operator names for display
     * @details The names are formatted as "<Operator Type> - <Operator Name>".
     * @note The built-in operator and user's plugin operators are collected, and macro relations are excluded.
     */
    void collectDefaultOperatorNamesForDisplay();

    /**
     * @brief Collect user-defined macros and add them to the operator suggestions list
     * @param relation Pointer to the FBConstraintRelation which will use the macro relations
     * @return List of macro relation names for display
     */
    QStringList collectMyMacrosForDisplay(FBConstraintRelation *relation);

private:
    QStringList mDefaultOperatorSuggestions; //!< List of default operator name suggestions
};