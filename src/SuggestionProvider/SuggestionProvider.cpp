#include "SuggestionProvider.h"

#include <string>

using namespace std;

// Collect operator names from groups under "Boxes/Functions/"
void SuggestionProvider::collectOperatorNames()
{
    QStringList operatorNames;
    const string parentPrefix = "Boxes/Functions/";

    const int groupCount = FBObject_GetGroupCount();

    for (int groupIndex = 0; groupIndex < groupCount; ++groupIndex)
    {
        const char *groupNameStr = FBObject_GetGroupName(groupIndex);
        if (!groupNameStr)
            continue;

        string groupName(groupNameStr);

        // Look for groups whose names start with "Boxes/Functions/"
        if (groupName.rfind(parentPrefix, 0) == 0)
        {
            // Extract the subgroup name by removing the parent prefix
            string subGroup = groupName.substr(parentPrefix.length());

            // Only consider direct subgroups (exclude nested ones with '/')
            if (subGroup.find('/') == string::npos && !subGroup.empty())
            {
                int entryCount = FBObject_GetEntryCount(groupIndex);
                for (int entryIndex = 0; entryIndex < entryCount; ++entryIndex)
                {
                    // Get Entry Name
                    const char *entryName = FBObject_GetEntryName(groupIndex, entryIndex);
                    if (!entryName)
                    {
                        continue;
                    }

                    // Create item text for list in the dialog
                    // Format: "<Relation GroupName> - <EntryName>"
                    std::string fullName = subGroup + " - " + string(entryName);
                    QString displayName = QString::fromUtf8(fullName.c_str(), fullName.length());

                    operatorNames << displayName;
                }
            }
        }
    }

    // Store the collected operator suggestions
    m_operatorSuggestions = operatorNames;
}

// Recursively traverse the scene model hierarchy and collect model names
void SuggestionProvider::collectModelNamesRecursive(FBModel *model, QStringList &nameList, QSet<QString> &nameSet)
{
    if (!model)
        return;

    // Get model's LongName(with namespace)
    const char *modelLongName = model->LongName;
    QString name = QString::fromUtf8(modelLongName);

    // Avoid duplicates using a set
    if (!nameSet.contains(name))
    {
        nameList << name;
        nameSet.insert(name);
    }

    // Recurse into children
    for (int i = 0; i < model->Children.GetCount(); ++i)
    {
        collectModelNamesRecursive(model->Children[i], nameList, nameSet);
    }
}

// Return the previously collected operator name suggestions
QStringList SuggestionProvider::getOperatorSuggestions()
{
    return m_operatorSuggestions;
}

// Collect and return model name suggestions
QStringList SuggestionProvider::getModelSuggestions()
{
    QStringList modelNames;
    QSet<QString> nameSet;

    // Get the Scene root model
    FBModel *root = FBSystem().Scene->RootModel;

    // Start recursive traversal from root's immediate children
    for (int i = 0; i < root->Children.GetCount(); ++i)
    {
        collectModelNamesRecursive(root->Children[i], modelNames, nameSet);
    }

    // Sort model name list in ascending order
    modelNames.sort();

    return modelNames;
}