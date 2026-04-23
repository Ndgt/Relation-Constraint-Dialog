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
    OperatorFirst
};

enum class ModelSearchFilter
{
    All,
    SkeletonOnly
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
        mModelEntries.clear();
        collectModelEntry();
    }

    OperatorSearchPriority getOperatorSearchPriority() const
    {
        return mOperatorSearchPriority;
    }

    void setOperatorSearchPriority(OperatorSearchPriority priority)
    {
        mOperatorSearchPriority = priority;
    }

    ModelSearchFilter getModelSearchFilter() const
    {
        return mModelSearchFilter;
    }

    void setModelSearchFilter(ModelSearchFilter filter)
    {
        mModelSearchFilter = filter;
    }

private:
    struct OperatorEntry
    {
        QString categoryName;
        QString operatorName;
    };

    struct ModelEntry
    {
        QString nameSpace;
        QString name;
        int typeId = -1; // FBComponent::GetTypeId() value
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

    void collectModelEntry();

private:
    QList<OperatorEntry> mDefaultOperatorEntriesBeforeMacro;
    QList<OperatorEntry> mDefaultOperatorEntriesAfterMacro;
    QList<ModelEntry> mModelEntries;

    OperatorSearchPriority mOperatorSearchPriority = OperatorSearchPriority::OperatorFirst;
    ModelSearchFilter mModelSearchFilter = ModelSearchFilter::All;
};