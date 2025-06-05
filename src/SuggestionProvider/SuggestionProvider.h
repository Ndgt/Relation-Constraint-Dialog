#ifndef SUGGESTION_PROVIDER_H
#define SUGGESTION_PROVIDER_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QSet>

#include <fbsdk/fbsdk.h>

// Singleton class that collects and provides suggestion data for SearchDialog
class SuggestionProvider
{
public:
    // Returns the singleton instance of the class
    static SuggestionProvider &instance()
    {
        static SuggestionProvider instance;
        return instance;
    }

    // Collect suggest data
    void collectOperatorNames();
    void collectModelNamesRecursive(FBModel *model, QStringList &nameList, QSet<QString> &nameSet);

    // Get suggest data
    QStringList getOperatorSuggestions();
    QStringList getModelSuggestions();

private:
    // Private constructor to enforce singleton pattern
    SuggestionProvider() = default;

    // Disable copy constructor
    SuggestionProvider(const SuggestionProvider &) = delete;

    // Disable assignment operator
    SuggestionProvider &operator=(const SuggestionProvider &) = delete;

private:
    // List of collected operator name suggestions
    QStringList m_operatorSuggestions;
};

#endif // SUGGESTION_PROVIDER_H