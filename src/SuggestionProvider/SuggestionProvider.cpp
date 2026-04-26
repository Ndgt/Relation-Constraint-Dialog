#include "SuggestionProvider.h"

#include <string>

#include <fbsdk/fbsdk.h>

#include "RelationDialogManager.h"

static ModelSearchFilter modelSearchFilterForTypeId(int typeId)
{
    if (typeId == FBModel::TypeInfo)
        return ModelSearchFilter::FBModelObjects;
    if (typeId == FBCamera::TypeInfo)
        return ModelSearchFilter::Cameras;
    if (typeId == FBCameraSwitcher::TypeInfo)
        return ModelSearchFilter::CameraSwitchers;
    if (typeId == FBModelCube::TypeInfo)
        return ModelSearchFilter::Cubes;
    if (typeId == FBLight::TypeInfo)
        return ModelSearchFilter::Lights;
    if (typeId == FBModelMarker::TypeInfo)
        return ModelSearchFilter::Markers;
    if (typeId == FBModelNull::TypeInfo)
        return ModelSearchFilter::Nulls;
    if (typeId == FBModelOptical::TypeInfo)
        return ModelSearchFilter::Opticals;
    if (typeId == FBModelPath3D::TypeInfo)
        return ModelSearchFilter::Path3Ds;
    if (typeId == FBModelPlane::TypeInfo)
        return ModelSearchFilter::Planes;
    if (typeId == FBModelRoot::TypeInfo)
        return ModelSearchFilter::Roots;
    if (typeId == FBModelSkeleton::TypeInfo)
        return ModelSearchFilter::Skeletons;

    return ModelSearchFilter::None;
}

QStringList SuggestionProvider::getOperatorSuggestions(QStringView queryView) const
{
    const QString query = queryView.toString().trimmed();

    // Combine default operators and macros into a single list of entries
    QList<OperatorEntry> operatorEntries;
    operatorEntries.append(mDefaultOperatorEntriesBeforeMacro);
    collectMyMacrosEntry(operatorEntries);
    operatorEntries.append(mDefaultOperatorEntriesAfterMacro);

    QStringList out;

    // If the query is empty, return all entries without any prioritization.
    if (query.isEmpty())
    {
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

    for (const auto &entry : entryCategoryStarts + entryCategoryContains + entryOperatorStarts + entryOperatorContains)
    {
        addOperatorSuggestion(out, *entry);
    }

    return out;
}

QStringList SuggestionProvider::getModelSuggestions(QStringView queryView) const
{
    const QString query = queryView.toString().trimmed();

    QStringList out;

    for (const auto &entry : mModelEntries)
    {
        // Create long name list for display and filtering
        const QString longName = entry.nameSpace.isEmpty() ? entry.name : entry.nameSpace + ":" + entry.name;
        FBTrace("Checking model entry '%s' with long name '%s' against query '%s'\n", entry.name.toUtf8().constData(), longName.toUtf8().constData(), query.toUtf8().constData());

        // First we check if the query is contained in the name
        // Note: if the query is empty, all entries will be included
        if (!query.isEmpty())
        {
            if (mIsModelNamespaceSearchDisabled)
            {
                if (!entry.name.contains(query, Qt::CaseInsensitive))
                    continue;
            }
            else
            {
                if (!longName.contains(query, Qt::CaseInsensitive))
                    continue;
            }
        }

        const ModelSearchFilter modelTypeFilter = modelSearchFilterForTypeId(entry.typeId);
        if (modelTypeFilter == ModelSearchFilter::None)
            continue;

        // Then we check if the model type is included in the search filters
        if (mModelSearchFilters.testFlag(modelTypeFilter))
            out.push_back(longName);
    }

    return out;
}

void SuggestionProvider::initializeModelSuggestions()
{
    mModelEntries.clear();
    collectModelEntry();
}

void SuggestionProvider::applyConfig(const RelationDialogConfig &config)
{
    mOperatorSearchPriority = config.operatorSearchPriority;
    mIsModelNamespaceSearchDisabled = config.modelNamespaceSearchDisabled;
    mModelSearchFilters = config.modelSearchFilters;
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

void SuggestionProvider::collectModelEntry()
{
    if (!mModelEntries.isEmpty())
        mModelEntries.clear();

    FBModelList modelList;
    FBFindModelsOfType(modelList, FBModel::TypeInfo, FBSystem::TheOne().Scene->RootModel);

    for (int i = 0; i < modelList.GetCount(); ++i)
    {
        FBModel *model = modelList[i];

        // Models collected by FBFindModelsOfType contains the parent model
        // which specified in the third argument of the function
        // We only want to collect the children models, so we skip the scene root model
        if (!model || model == FBSystem::TheOne().Scene->RootModel)
            continue;

        FBNamespace *nameSpace = model->GetOwnerNamespace();
        QString nameSpaceStr = nameSpace ? QString::fromUtf8(nameSpace->Name.AsString()) : QString();

        mModelEntries.push_back(
            ModelEntry{nameSpaceStr,
                       QString::fromUtf8(model->Name.AsString()),
                       model->GetTypeId()});
    }
}