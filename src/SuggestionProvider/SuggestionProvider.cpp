#include "SuggestionProvider.h"

#include <string>

void SuggestionProvider::collectOperatorNamesForDisplay()
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

            // Create display name formatted as "<Operator Type> - <Operator Name>"
            std::string displayName = operatorGroupName.substr(parentGroupPrefix.length()) + " - " + std::string(operatorName);

            operatorNames << QString::fromStdString(displayName);
        }
    }

    mOperatorSuggestions = operatorNames;
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