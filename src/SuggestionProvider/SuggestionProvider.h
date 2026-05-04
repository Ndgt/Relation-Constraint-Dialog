#pragma once

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QStringView>

#include "RelationDialogConfig.h"

/**
 * @class SuggestionProvider
 * @brief Singleton class that collects and provides suggestion data for SearchDialog
 */
class SuggestionProvider
{
public:
    /**
     * @brief Get operator suggestions based on the query string
     * @details This function is called by SearchDialog to retrieve operator suggestions matching the user's query.
     *          It combines default operators and "My Macros" operators, applies the search priority and filtering
     *          based on the query, and returns a list of formatted suggestion strings.
     * @param queryView The query string to filter operator suggestions
     * @return A list of operator suggestions matching the query, formatted as "Category - Operator"
     */
    QStringList getOperatorSuggestions(QStringView queryView) const;

    /**
     * @brief Get model suggestions based on the query string and search filters
     * @details This function is called by SearchDialog to retrieve model suggestions matching the user's query.
     *          It applies the search filters and returns a list of formatted suggestion strings.
     * @param queryView The query string to filter model suggestions
     * @return A list of model suggestions matching the query and search filters, formatted as "Namespace:Name"
     *         or "Name" if namespace is empty
     */
    QStringList getModelSuggestions(QStringView queryView) const;

    /**
     * @brief Initialize operator suggestions by collecting all system and plugin operators
     * @note This function is called by RelationDialogManager when all the basic initialization is done
     *       to collect all operators including those from plugins.
     */
    void initializeOperatorSuggestions();

    /**
     * @brief Initialize model suggestions by collecting all scene models
     * @note This function is called when the dialog is opened to ensure that the model suggestions are up-to-date
     *       with the current scene content.
     */
    void initializeModelSuggestions();

    /**
     * @brief Apply the given configuration to the SuggestionProvider
     * @param config The RelationDialogConfig struct containing the settings to be applied to the SuggestionProvider
     */
    void applyConfig(const RelationDialogConfig &config);

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
     * @struct OperatorEntry
     * @brief Struct to hold operator entry data for suggestions
     */
    struct OperatorEntry
    {
        QString categoryName; //!< Category name for the operator (e.g., "Boolean", "Converters")
        QString operatorName; //!< Name of the operator
    };

    /**
     * @struct ModelEntry
     * @brief Struct to hold model entry data for suggestions
     */
    struct ModelEntry
    {
        QString nameSpace; //!< Namespace of the model
        QString name;      //!< Name of the model
        int typeId = -1;   //!< FBComponent::GetTypeId() value
    };

    /**
     * @brief Singleton constructor
     */
    SuggestionProvider() = default;

    /// @cond
    SuggestionProvider(const SuggestionProvider &) = delete;
    SuggestionProvider &operator=(const SuggestionProvider &) = delete;
    /// @endcond

    /**
     * @brief Create operator suggestion string and add it to the suggestions list
     * @details Combines category and operator name as "Category - Operator" for display in SearchDialog
     *          and append it to the specified suggestions list.
     * @param suggestions List of suggestion strings to add to
     * @param entry OperatorEntry containing category and operator name
     */
    void addOperatorSuggestion(QStringList &suggestions, const OperatorEntry &entry) const;

    /**
     * @brief Collect default operators as OperatorEntry for suggestions
     * @details Collects system operators grouped under "Boxes/Functions/" and categorizes them based on their category name.
     * @note This function only collects system operators and does not include "My Macros" operators.
     */
    void collectDefaultOperatorEntry();

    /**
     * @brief Collect "My Macros" operators as OperatorEntry for suggestions
     * @details Collects macro relation operators based on the currently existing relation constraint in the scene.
     */
    void collectMyMacrosEntry(QList<OperatorEntry> &entries) const;

    /**
     * @brief Collect all scene models as ModelEntry for suggestions
     */
    void collectModelEntry();

private:
    QList<OperatorEntry> mDefaultOperatorEntriesBeforeMacro; //!< Operator entries that are always shown before macro operators
    QList<OperatorEntry> mDefaultOperatorEntriesAfterMacro;  //!< Operator entries that are always shown after macro operators
    QList<ModelEntry> mModelEntries;                         //!< Model entries collected from the scene

    OperatorSearchPriority mOperatorSearchPriority = OperatorSearchPriority::OperatorFirst; //!< Search priority for operators in SearchDialog
    ModelSearchFilters mModelSearchFilters = ModelSearchFilter::None;                       //!< Search filters for models in SearchDialog
    bool mIsModelNamespaceSearchDisabled = false;                                           //!< Flag to indicate whether model namespace search is disabled in SearchDialog
};