#pragma once

#include <QtCore/QFlags>
#include <QtCore/QList>
#include <QtCore/QSet>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QStringView>
#include <QtCore/QtTypes>

#include <fbsdk/fbsdk.h>

enum class OperatorSearchPriority
{
    CategoryFirst,
    OperatorFirst
};

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

    ModelSearchFilters getModelSearchFilters() const
    {
        return mModelSearchFilters;
    }

    void setModelSearchFilters(ModelSearchFilters filters)
    {
        mModelSearchFilters = filters;
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
    ModelSearchFilters mModelSearchFilters = ModelSearchFilter::None;
};