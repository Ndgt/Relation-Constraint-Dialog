#pragma once

#include <QtCore/QList>
#include <QtCore/QSet>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QStringView>

#include <fbsdk/fbsdk.h>

enum class OperatorSearchPriority
{
    CategoryFirst,
    OperatorFirst,
};

/**
 * @class SuggestionProvider
 * @brief Singleton class that collects and provides suggestion data for SearchDialog
 */
class SuggestionProvider
{
public:
    QStringList getOperatorSuggestions(QStringView queryView) const;

    QStringList getModelSuggestions(QStringView queryView) const;

    /**
     * @brief Get the singleton instance of SuggestionProvider
     * @return Reference to the singleton instance
     */
    static SuggestionProvider &getInstance()
    {
        static SuggestionProvider instance;
        return instance;
    }

    void initializeModelSuggestions()
    {
        mSceneModelLongNames.clear();
        collectSceneModelLongNames();
    }

    OperatorSearchPriority getOperatorSearchPriority() const
    {
        return mOperatorSearchPriority;
    }

    void setOperatorSearchPriority(OperatorSearchPriority priority)
    {
        mOperatorSearchPriority = priority;
    }

private:
    struct OperatorEntry
    {
        QString categoryName;
        QString operatorName;
    };

    /**
     * @brief Singleton constructor
     * @details Collects operator names for display upon initialization.
     */
    SuggestionProvider() { collectDefaultOperatorEntry(); }

    /// @cond
    SuggestionProvider(const SuggestionProvider &) = delete;
    SuggestionProvider &operator=(const SuggestionProvider &) = delete;
    /// @endcond

    void addOperatorSuggestion(QStringList &suggestions, const OperatorEntry &entry) const;

    void collectDefaultOperatorEntry();

    void collectMyMacrosEntry(QList<OperatorEntry> &entries) const;

    void collectSceneModelLongNames();

    /**
     * @brief Helper function recursively collect model LongNames starting from the given model
     * @param model Pointer to the starting FBModel
     * @param nameSet Set to store unique model LongNames
     */
    void collectModelLongNamesRecursive(FBModel *model, QSet<QString> &nameSet);

private:
    QStringList mSceneModelLongNames;
    QList<OperatorEntry> mDefaultOperatorEntriesBeforeMacro;
    QList<OperatorEntry> mDefaultOperatorEntriesAfterMacro;
    OperatorSearchPriority mOperatorSearchPriority = OperatorSearchPriority::OperatorFirst;
};