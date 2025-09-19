#pragma once

#include <fbsdk/fbsdk.h>

#include <QtCore/QSet>
#include <QtCore/QString>
#include <QtCore/QStringList>

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
     * @return List of operator name suggestions
     */
    QStringList getOperatorSuggestions() { return mOperatorSuggestions; }

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
    SuggestionProvider() { collectOperatorNamesForDisplay(); }

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
     * @brief Collect all relation operator names for display
     * @details The names are formatted as "<Operator Type> - <Operator Name>".
     * @note This function is called during initialization to populate the operator suggestions list.
     */
    void collectOperatorNamesForDisplay();

private:
    QStringList mOperatorSuggestions; //!< List of collected operator name suggestions
};