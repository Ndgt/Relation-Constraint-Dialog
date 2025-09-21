#include "SuggestionProvider.h"

#include <string>

void SuggestionProvider::collectDefaultOperatorNamesForDisplay()
{
    QStringList operatorNames;

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

            // Create display name formatted as "<Operator Type> - <Operator Name>"
            std::string displayName = operatorTypeName + " - " + std::string(operatorName);

            operatorNames << QString::fromStdString(displayName);
        }
    }

    mDefaultOperatorSuggestions = operatorNames;
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

QStringList SuggestionProvider::getModelSuggestions()
{
    // Get the Scene root model
    FBModel *root = FBSystem().Scene->RootModel;
    if (!root)
        return QStringList();

    QSet<QString> nameSet;
    for (int i = 0; i < root->Children.GetCount(); ++i)
    {
        collectModelLongNamesRecursive(root->Children[i], nameSet);
    }

    // Get string list from the set
    QStringList modelNames = nameSet.values();

    // Sort model name list in ascending order, case-insensitive
    modelNames.sort(Qt::CaseInsensitive);

    return modelNames;
}

QStringList SuggestionProvider::getOperatorSuggestions(FBConstraintRelation *relation)
{
    // Combine standard operators and macro relations
    QStringList allOperators = mDefaultOperatorSuggestions;
    allOperators.append(collectMyMacrosForDisplay(relation));
    allOperators.sort(Qt::CaseInsensitive);
    return allOperators;
}

QStringList SuggestionProvider::collectMyMacrosForDisplay(FBConstraintRelation *relation)
{
    QStringList macroNames;

    if (!relation)
        return macroNames;

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

        // Create display name formatted as "My Macros - <Macro Name>"
        QString displayName = "My Macros - " + QString::fromUtf8(relationConstraint->Name.AsString());

        // Avoid duplicates
        if (macroNames.contains(displayName))
            continue;

        macroNames << displayName;
    }

    return macroNames;
}