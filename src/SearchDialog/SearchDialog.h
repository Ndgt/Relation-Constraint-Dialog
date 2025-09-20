#pragma once

#include "ui_SearchDialog.h"

#include <QtCore/QPoint>
#include <QtCore/QString>
#include <QtGui/QShowEvent>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QWidget>

#include <fbsdk/fbsdk.h>

/**
 * @class SearchDialog
 * @brief Dialog for searching and selecting FBConstraintRelation objects
 */
class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @details Sets up the UI elements and connects the necessary signals and slots.
     * @param cursorPosition The cursor position where the dialog should appear
     * @param relationPosition The position where the new relation object should be created
     * @param selectedConstraint The currently selected FBConstraintRelation object
     */
    explicit SearchDialog(const QPoint &cursorPosition, const QPoint &relationPosition, FBConstraintRelation *selectedConstraint);

    /**
     * @brief Destructor
     * @details Deletes the pointer to the Widget Container class generated from the .ui file.
     */
    ~SearchDialog() { delete ui; }

protected:
    /**
     * @brief Initialize suggestion list and reposition dialog
     * @details This function also set the topmost item in the suggestion list as the current item.
     * @param event The show event
     */
    void showEvent(QShowEvent *event) override;

private slots:
    /**
     * @brief Finalize the dialog selection and create relation objects
     */
    void finalize();

    // Qt Slot: Reset suggestion list on Find Option change

    /**
     * @brief Handle radio button group toggled event
     * @details This slot calls onTextChanged to refresh the suggestion list based on the current text input.
     * @param button The button that was toggled (not used)
     * @param checked The new checked state of the button (not used)
     */
    void onRadioButtonGroupToggled(QAbstractButton *button, bool checked);

    /**
     * @brief Handle item clicked event in the suggestion list
     * @details This slot sets the clicked item as the current item and calls finalize to create the relation object.
     * @param item The item that was clicked
     */
    void onItemClicked(QListWidgetItem *item);

    /**
     * @brief Handle return key pressed event in the line edit
     * @details This slot calls finalize to create the relation object based on the current input.
     */
    void onLineEditKeyReturnPressed();

    /**
     * @brief Handle tab key pressed event in the line edit
     * @details This slot toggles between the operator and model radio buttons.
     */
    void onLineEditKeyTabPressed();

    /**
     * @brief Handle up/down arrow key pressed event in the line edit
     * @details This slot changes the current item in the suggestion list based on the arrow key pressed.
     * @param key The key code of the pressed key (Qt::Key_Up or Qt::Key_Down)
     */
    void onLineEditKeyUpDownPressed(int key);

    /**
     * @brief Handle text changed event in the line edit
     * @details This slot refreshes the suggestion list based on the current text input and find option.
     * @param text The current text in the line edit
     */
    void onTextChanged(const QString &text);

private:
    Ui::Dialog *ui;                                              //!< Pointer to the Widget Container class generated from the .ui file
    QPoint mCursorPosition;                                      //!< The cursor position where the dialog should appear
    QPoint mRelationPosition;                                    //!< The position where the new relation object should be created
    HdlFBPlugTemplate<FBConstraintRelation> mSelectedConstraint; //!< The handle to the currently selected constraint object
};
