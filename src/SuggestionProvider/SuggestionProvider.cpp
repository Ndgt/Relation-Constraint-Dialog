#include "SuggestionProvider.h"

#include <string>

#include "RelationDialogManager.h"

QStringList SuggestionProvider::getOperatorSuggestions(QStringView queryView) const
{
    const QString query = queryView.toString().trimmed();

    // Combine default operators and macros into a single list of entries
    QList<OperatorEntry> operatorEntries;
    operatorEntries.append(mDefaultOperatorEntriesBeforeMacro);
    collectMyMacrosEntry(operatorEntries);
    operatorEntries.append(mDefaultOperatorEntriesAfterMacro);

    // If the query is empty, return all entries without any prioritization.
    if (query.isEmpty())
    {
        QStringList out;

        for (const auto &entry : operatorEntries)
        {
            addOperatorSuggestion(out, entry);
        }

        return out;
    }

    QList<const OperatorEntry *> entryCategoryStarts, entryCategoryContains, entryOperatorStarts, entryOperatorContains;

    for (const auto &entry : operatorEntries)
    {
        const bool categoryStarts = entry.categoryName.startsWith(query, Qt::CaseInsensitive);
        const bool categoryContains = !categoryStarts && entry.categoryName.contains(query, Qt::CaseInsensitive);
        const bool operatorStarts = entry.operatorName.startsWith(query, Qt::CaseInsensitive);
        const bool operatorContains = !operatorStarts && entry.operatorName.contains(query, Qt::CaseInsensitive);

        if (mOperatorSearchPriority == OperatorSearchPriority::CategoryFirst)
        {
            if (categoryStarts)
                entryCategoryStarts.push_back(&entry);
            else if (categoryContains)
                entryCategoryContains.push_back(&entry);
            else if (operatorStarts)
                entryOperatorStarts.push_back(&entry);
            else if (operatorContains)
                entryOperatorContains.push_back(&entry);
        }
        else
        {
            if (operatorStarts)
                entryCategoryStarts.push_back(&entry);
            else if (operatorContains)
                entryCategoryContains.push_back(&entry);
            else if (categoryStarts)
                entryOperatorStarts.push_back(&entry);
            else if (categoryContains)
                entryOperatorContains.push_back(&entry);
        }
    }

    QStringList out;

    for (const auto &entry : entryCategoryStarts + entryCategoryContains + entryOperatorStarts + entryOperatorContains)
    {
        addOperatorSuggestion(out, *entry);
    }

    return out;
}

QStringList SuggestionProvider::getModelSuggestions(QStringView queryView) const
{
    const QString query = queryView.toString().trimmed();

    if (query.isEmpty())
        return mSceneModelLongNames;

    QStringList out;

    for (const auto &name : mSceneModelLongNames)
    {
        if (name.contains(query, Qt::CaseInsensitive))
            out.push_back(name);
    }

    return out;
}

void SuggestionProvider::addOperatorSuggestion(QStringList &suggestions, const OperatorEntry &entry) const
{
    suggestions.push_back(entry.categoryName + QStringLiteral(" - ") + entry.operatorName);
}

void SuggestionProvider::collectDefaultOperatorEntry()
{
    mDefaultOperatorEntriesBeforeMacro.clear();
    mDefaultOperatorEntriesAfterMacro.clear();

    // Operator functions are grouped under "Boxes/Functions/"
    // e.g., "Boxes/Functions/Vector"
    const std::string parentGroupPrefix = "Boxes/Functions/";

    const int groupCount = FBObject_GetGroupCount();
    for (int groupIndex = 0; groupIndex < groupCount; ++groupIndex)
    {
        const char *operatorGroupNameCStr = FBObject_GetGroupName(groupIndex);
        if (!operatorGroupNameCStr)
            continue;

        std::string operatorGroupName(operatorGroupNameCStr);

        // Check if the group name starts with the desired prefix
        if (operatorGroupName.rfind(parentGroupPrefix, 0) != 0)
            continue;

        int entryCount = FBObject_GetEntryCount(groupIndex);
        for (int entryIndex = 0; entryIndex < entryCount; ++entryIndex)
        {
            const char *operatorName = FBObject_GetEntryName(groupIndex, entryIndex);
            if (!operatorName)
                continue;

            std::string operatorTypeName = operatorGroupName.substr(parentGroupPrefix.length());

            // Skip invalid or not default operator types (e.g., macro relations)
            if (operatorTypeName.empty() || operatorTypeName == "My Macros")
                continue;

            if (operatorTypeName < "My Macros")
            {
                mDefaultOperatorEntriesBeforeMacro.push_back(
                    OperatorEntry{QString::fromStdString(operatorTypeName),
                                  QString::fromUtf8(operatorName)});
            }
            else
            {
                mDefaultOperatorEntriesAfterMacro.push_back(
                    OperatorEntry{QString::fromStdString(operatorTypeName),
                                  QString::fromUtf8(operatorName)});
            }
        }
    }
}

void SuggestionProvider::collectMyMacrosEntry(QList<OperatorEntry> &entries) const
{
    FBConstraintRelation *relation = RelationDialogManager::getInstance().getLastSelectedRelationConstraint();
    if (!relation)
        return;

    for (int i = 0; i < FBSystem::TheOne().Scene->Constraints.GetCount(); ++i)
    {
        FBConstraint *constraint = FBSystem::TheOne().Scene->Constraints[i];

        // Check if constraint is valid and of type FBConstraintRelation
        if (FBIS(constraint, FBConstraintRelation) == false)
            continue;

        FBConstraintRelation *relationConstraint = (FBConstraintRelation *)constraint;

        // We cannnot create a macro relation operator in itself
        if (relationConstraint == relation)
            continue;

        entries.push_back(
            OperatorEntry{"My Macros",
                          QString::fromUtf8(relationConstraint->Name.AsString())});
    }
}

void SuggestionProvider::collectSceneModelLongNames()
{
    if (!mSceneModelLongNames.isEmpty())
        mSceneModelLongNames.clear();

    FBModelList modelList;
    FBFindModelsOfType(modelList, FBModel::TypeInfo, FBSystem::TheOne().Scene->RootModel);

    for (int i = 0; i < modelList.GetCount(); ++i)
    {
        FBModel *model = modelList[i];
        if (!model)
            continue;

        // Models collected by FBFindModelsOfType contains the parent model
        // which specified in the third argument of the function
        // We only want to collect the children models, so we skip the scene root model
        if (model != FBSystem::TheOne().Scene->RootModel)
        {
            const char *modelLongName = model->LongName;
            mSceneModelLongNames.push_back(QString::fromUtf8(modelLongName));
        }
    }
}

void SuggestionProvider::collectModelLongNamesRecursive(FBModel *model, QSet<QString> &nameSet)
{
    if (!model)
        return;

    // Get model's LongName (name with namespace)
    const char *modelLongName = model->LongName;
    QString name = QString::fromUtf8(modelLongName);

    // Avoid duplicates using a set
    nameSet.insert(name);

    // Recurse into children
    for (int i = 0; i < model->Children.GetCount(); ++i)
    {
        collectModelLongNamesRecursive(model->Children[i], nameSet);
    }
}
