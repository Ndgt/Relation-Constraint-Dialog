#ifndef SEARCH_DIALOG_H
#define SEARCH_DIALOG_H

#include "ui_SearchDialog.h"

#include <string>

#include <QtCore/QEvent>
#include <QtCore/QObject>
#include <QtCore/QPoint>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtGui/QShowEvent>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QWidget>

#include <fbsdk/fbsdk.h>

// Main Dialog class in the Relation Constraint
class SearchDialog : public QDialog, private Ui_Dialog
{
    Q_OBJECT
public:
    // Constructor taking parent widget and cursor position for dialog placement
    explicit SearchDialog(QWidget *pParent = nullptr, QPoint cursor_pos = QPoint());

protected:
    // Override showEvent to reposition and initialize UI content
    void showEvent(QShowEvent *event) override;

public slots:
    // Qt Slot: Update suggestion list
    void onTextChanged(const QString &text);

    // Qt Slot: Create relation objects when dialog item clicked
    void onItemClicked(QListWidgetItem *item);

    // Qt Slot: Reset suggestion list on Find Option change
    void onButtonToggled(QAbstractButton *_button, bool _checked);

    // Qt Slot: Finalize Dialog selection and create relation objects
    void finalize();

public:
    // Find the currently selected FBConstraintRelation in the scene
    // Returns nullptr if no valid relation is selected.
    FBConstraintRelation *getCurrentSelectedRelation();

    // UI element getters for external access by lineEdit
    QRadioButton *getOperatorRadioButton() { return radioButton_Operator; }
    QRadioButton *getModelRadioButton() { return radioButton_Model; }
    QListWidget *getListWidget() { return listWidget; }

private:
    QPoint m_CursorPosition;
    FBConstraintRelation *m_SelectedConstraint = nullptr;
};

/*
 * Event filter for lineEdit in the dialog defines all key control
 *   - Tab - Switch find option
 *   - Up/Down - Dialog item selection
 *   - Enter - Finalize item selection and create relation object
 */
class LineEditFilter : public QObject
{
    Q_OBJECT
public:
    LineEditFilter(SearchDialog *parent = nullptr);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    // Reference to the parent SearchDialog
    SearchDialog *dialog = nullptr;
};

#endif // SEARCH_DIALOG_H