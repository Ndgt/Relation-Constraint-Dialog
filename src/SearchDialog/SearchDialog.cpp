#include "CustomLineEdit.h"
#include "RelationDialogManager.h"
#include "SearchDialog.h"
#include "SuggestionProvider.h"

#include <QtCore/QRect>
#include <QtCore/QStringList>
#include <QtCore/QtConfig>
#include <QtCore/QTimer>
#include <QtGui/QPalette>
#include <QtWidgets/QMenu>

#if QT_VERSION_MAJOR >= 6
#include <QtGui/QAction>
#else
#include <QtWidgets/QAction>
#endif

SearchDialog::SearchDialog(const QPoint &cursorPosition, const QPoint &relationPosition, FBConstraintRelation *selectedConstraint)
    : QDialog(nullptr), ui(new Ui::Dialog), mCursorPosition(cursorPosition), mRelationPosition(relationPosition), mSelectedConstraint(selectedConstraint)
{
    if (!mSelectedConstraint.Ok())
    {
        // Display warning if the constraint is not valid
        FBMessageBox("Relation Constraint Dialog",
                     "[Error] No Relation Constraint is selected.\nMake sure the relation is selected in the scene browser.",
                     "OK");

        // close dialog
        QTimer::singleShot(0, this, SLOT(close()));
    }

    // Populate UI elements from ui_SearchDialog.h - generated from .ui file
    ui->setupUi(this);

    // Set the dialog style to be a popup and delete automatically on close
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::Popup);

    // Connect Singals & Slots
    connect(ui->lineEdit, &CustomLineEdit::keyReturnPressed, this, &SearchDialog::onLineEditKeyReturnPressed);
    connect(ui->lineEdit, &CustomLineEdit::keyTabPressed, this, &SearchDialog::onLineEditKeyTabPressed);
    connect(ui->lineEdit, &CustomLineEdit::keyUpDownPressed, this, &SearchDialog::onLineEditKeyUpDownPressed);
    connect(ui->lineEdit, &CustomLineEdit::textChanged, this, &SearchDialog::onTextChanged);
    connect(ui->listWidget, &QListWidget::itemClicked, this, &SearchDialog::onItemClicked);

    // QButtonGroup::buttonToggled signal is overloaded in Qt5
#if QT_VERSION_MAJOR >= 6
    connect(ui->buttonGroup, &QButtonGroup::buttonToggled, this, &SearchDialog::onRadioButtonGroupToggled);
#else
    connect(ui->buttonGroup, QOverload<QAbstractButton *, bool>::of(&QButtonGroup::buttonToggled), this, &SearchDialog::onRadioButtonGroupToggled);
#endif

    // Fix default placeholder text color (Qt6 issue)
#if QT_VERSION_MAJOR == 6
    QPalette palette = ui->lineEdit->palette();
    palette.setColor(QPalette::ColorRole::PlaceholderText, QColor("#c8c8c8"));
    ui->lineEdit->setPalette(palette);
#endif
}

void SearchDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    // Reposition dialog so that the cursor appears aligned with the top of the lineEdit,
    // and the dialog opens with its left edge aligned with the cursor's x-position.
    QPoint openPosition = mCursorPosition - QPoint(0, ui->lineEdit->geometry().y());
    move(openPosition);

    // Populate list with operator suggestions
    QStringList allSuggestions = SuggestionProvider::getInstance().getOperatorSuggestions(mSelectedConstraint);
    ui->listWidget->addItems(allSuggestions);

    // Set the topmost item as the current item
    if (ui->listWidget->count() > 0)
        ui->listWidget->setCurrentRow(0);
}

void SearchDialog::finalize()
{
    QListWidgetItem *item = ui->listWidget->currentItem();

    if (!item && ui->listWidget->count() > 0)
        item = ui->listWidget->item(0); // fallback: use the top item

    if (!item)
    {
        accept();
        return;
    }

    // Get item name selected
    QString selectedItemText = item->text();

    // Operator
    if (ui->radioButtonOperator->isChecked())
    {
        // Parse "<Operator Type> - <Operator Name>" format
        const static QString separator = " - ";
        int index = selectedItemText.indexOf(separator);
        if (index != -1)
        {
            QString operatorTypeName = selectedItemText.left(index);
            QString operatorName = selectedItemText.mid(index + separator.length());

            if (mSelectedConstraint.Ok())
            {
                QByteArray typeNameBytes = operatorTypeName.toUtf8();
                QByteArray nameBytes = operatorName.toUtf8();

                // Create Relation Object
                FBBox *box_operator = mSelectedConstraint->CreateFunctionBox(typeNameBytes.constData(), nameBytes.constData());
                if (box_operator)
                    mSelectedConstraint->SetBoxPosition(box_operator, mRelationPosition.x(), mRelationPosition.y());
            }
        }

        // Close Dialog
        accept();
    }

    // Model
    else
    {
        // Get Scene model with selected item name
        QByteArray selectedItemBytes = selectedItemText.toUtf8();
        FBModel *selectedItemModel = FBFindModelByLabelName(selectedItemBytes.constData());
        if (!selectedItemModel)
        {
            QString errorStr = "[Error] Model \"" + selectedItemText + "\" is not found in the scene.";
            QByteArray errorStrBytes = errorStr.toUtf8();
            FBMessageBox("Relation Constraint Dialog", errorStrBytes.constData(), "OK");

            // Close Dialog
            accept();
        }
        else
        {
            // Configure custom menu for relation object type selection
            QMenu *menu = new QMenu(this);
            QAction *action1 = menu->addAction("Set as Source Object");
            QAction *action2 = menu->addAction("Constrain Object");
            menu->setActiveAction(action1); // default selection

            // Show menu next to the current item
            QRect itemRect = ui->listWidget->visualItemRect(item);
            QAction *action = menu->exec(ui->listWidget->mapToGlobal(QPoint(itemRect.x() + itemRect.width(), itemRect.y())));

            if (action && mSelectedConstraint.Ok())
            {
                FBBox *selectedItemModelBox = nullptr;

                if (action == action1)
                    selectedItemModelBox = mSelectedConstraint->SetAsSource(selectedItemModel);
                else if (action == action2)
                    selectedItemModelBox = mSelectedConstraint->ConstrainObject(selectedItemModel);

                if (selectedItemModelBox)
                    mSelectedConstraint->SetBoxPosition(selectedItemModelBox, mRelationPosition.x(), mRelationPosition.y());

                // Close Dialog
                accept();
            }
            else
            {
                // No type selected: Close menu only, not close dialog
            }
        }
    }
}

void SearchDialog::onItemClicked(QListWidgetItem *item)
{
    if (item)
    {
        ui->listWidget->setCurrentItem(item);
        finalize();
    }
}

void SearchDialog::onLineEditKeyReturnPressed()
{
    finalize();
}

void SearchDialog::onLineEditKeyTabPressed()
{
    if (ui->radioButtonOperator->isChecked())
        ui->radioButtonModel->setChecked(true);
    else
        ui->radioButtonOperator->setChecked(true);
}

void SearchDialog::onLineEditKeyUpDownPressed(int key)
{
    int row = ui->listWidget->currentRow();
    int count = ui->listWidget->count();

    if (count == 0)
        return;

    // Move suggestion list selection up/down
    if (key == Qt::Key_Down)
        row = (row + 1) % count;
    else if (key == Qt::Key_Up)
        row = (row - 1 + count) % count;

    ui->listWidget->setCurrentRow(row);
}

void SearchDialog::onRadioButtonGroupToggled(QAbstractButton *button, bool checked)
{
    Q_UNUSED(button);
    Q_UNUSED(checked);
    onTextChanged(ui->lineEdit->text());
}

void SearchDialog::onTextChanged(const QString &text)
{
    ui->listWidget->clear();

    QStringList allSuggestions;

    // Get suggestion name list
    if (ui->radioButtonOperator->isChecked())
        allSuggestions = SuggestionProvider::getInstance().getOperatorSuggestions(mSelectedConstraint);
    else
        allSuggestions = SuggestionProvider::getInstance().getModelSuggestions();

    // Update dialog list with matching items
    for (const QString &suggestName : allSuggestions)
    {
        // Not consider by case-sensitive
        if (suggestName.contains(text, Qt::CaseInsensitive))
            ui->listWidget->addItem(suggestName);
    }

    // Set current row on the top of list
    if (ui->listWidget->count() > 0)
        ui->listWidget->setCurrentRow(0);
}