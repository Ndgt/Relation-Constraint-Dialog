#include "SearchDialog.h"
#include "SuggestionProvider.h"

#include <random>
#include <string>

#include <QtCore/QRect>
#include <QtCore/QSize>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QtConfig>
#include <QtCore/QtGlobal>
#include <QtCore/QTimer>
#include <QtGui/QKeyEvent>
#include <QtGui/QPalette>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMenu>

#if QT_VERSION_MAJOR >= 6
#include <QtGui/QAction>
#else
#include <QtWidgets/QAction>
#endif

using namespace std;

class lineEditFilter;

// Constructor for the search dialog
SearchDialog::SearchDialog(QWidget *pParent, QPoint cursor_pos)
    : QDialog(pParent), m_CursorPosition(cursor_pos)
{
    // Populate UI elements from header generated from .ui file
    setupUi(this);

    // Install lineEdit Event Filter
    LineEditFilter *filter = new LineEditFilter(this);
    lineEdit->installEventFilter(filter);

    // Set the dialog style to be a popup and delete automatically on close
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::Popup);

    // Connect Singals & Slots
    connect(lineEdit, QOverload<const QString &>::of(&QLineEdit::textChanged), this, &SearchDialog::onTextChanged);
    connect(listWidget, QOverload<QListWidgetItem *>::of(&QListWidget::itemClicked), this, &SearchDialog::onItemClicked);
    connect(buttonGroup, QOverload<QAbstractButton *, bool>::of(&QButtonGroup::buttonToggled), this, &SearchDialog::onButtonToggled);
    connect(lineEdit, &QLineEdit::returnPressed, this, &SearchDialog::finalize);

#if QT_VERSION_MAJOR == 6
    // Fix default placeholder text color (Qt 6 issue)
    QPalette palette = lineEdit->palette();
    palette.setColor(QPalette::ColorRole::PlaceholderText, QColor("#c8c8c8"));
    lineEdit->setPalette(palette);
#endif

    // Attempt to get currently selected relation constraint
    m_SelectedConstraint = getCurrentSelectedRelation();
    if (m_SelectedConstraint == nullptr)
    {
        // Display warning and close dialog if no relation is selected
        string error_str_top = "Error: No Relation Constraint is selected.\n";
        string error_str_bottom = "Make sure the relation is selected in the scene browser.";
        FBMessageBox("Warning", (error_str_top + error_str_bottom).c_str(), "OK");

        QTimer::singleShot(0, this, SLOT(close()));
    }
}

// Find and return the selected FBConstraintRelation in the scene
FBConstraintRelation *SearchDialog::getCurrentSelectedRelation()
{
    for (int i = 0; i < FBSystem().Scene->Constraints.GetCount(); i++)
    {
        FBConstraint *constraint = FBSystem().Scene->Constraints[i];
        if (constraint->Is(FBConstraintRelation::TypeInfo) && constraint->Selected == true)
            return static_cast<FBConstraintRelation *>(constraint);
    }
    return nullptr;
}

// Executed when the dialog is shown (positioned, filled)
void SearchDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    // Reposition dialog so that the cursor appears aligned with the top of the lineEdit,
    // and the dialog opens with its left edge aligned with the cursor's x-position.
    QPoint open_position = m_CursorPosition - QPoint(0, this->lineEdit->geometry().y());
    this->move(open_position);

    // Populate list with all operator suggestions
    QStringList allSuggestions = SuggestionProvider::instance().getOperatorSuggestions();

    for (const QString &suggest_text : allSuggestions)
    {
        listWidget->addItem(suggest_text);
        QListWidgetItem *item = listWidget->item(listWidget->count() - 1);
        item->setSizeHint(QSize(0, 18)); // make a little bit taller
    }

    // Set current row on the top of list
    if (listWidget->count() > 0)
        listWidget->setCurrentRow(0);
}

// Slot: Update suggestion list
void SearchDialog::onTextChanged(const QString &text)
{
    listWidget->clear();

    QStringList allSuggestions;

    // Get suggestion string list
    if (radioButton_Operator->isChecked())
        allSuggestions = SuggestionProvider::instance().getOperatorSuggestions();
    else
        allSuggestions = SuggestionProvider::instance().getModelSuggestions();

    // Update dialog list with matching items
    for (const QString &suggest_text : allSuggestions)
    {
        // Not consider by case-sensitive
        if (suggest_text.contains(text, Qt::CaseInsensitive))
        {
            listWidget->addItem(suggest_text);
            QListWidgetItem *item = listWidget->item(listWidget->count() - 1);
            item->setSizeHint(QSize(0, 18)); // make a little bit taller
        }
    }

    // Set current row on the top of list
    if (listWidget->count() > 0)
        listWidget->setCurrentRow(0);
}

// Slot: Create relation objects when dialog item clicked
void SearchDialog::onItemClicked(QListWidgetItem *item)
{
    if (item)
    {
        listWidget->setCurrentItem(item);
        finalize();
    }
}

// Slot: Refresh the suggestion list when the Find Option is toggled
void SearchDialog::onButtonToggled(QAbstractButton *_button, bool _checked)
{
    onTextChanged(lineEdit->text());
}

// Slot: Finalize Dialog selection and create relation objects
void SearchDialog::finalize()
{
    QListWidgetItem *item = listWidget->currentItem();
    if (!item && listWidget->count() > 0)
    {
        item = listWidget->item(0); // fallback: use the top item
    }

    if (!item)
    {
        accept();
        return;
    }

    // FIXME: Currently using random positions; should align box with dialog's OpenGL position
    static mt19937 rng(random_device{}());
    uniform_int_distribution<int> dist(-25, 25);

    // Get item name selected
    QString selectedItemText = item->text();

    // Operator
    if (radioButton_Operator->isChecked())
    {
        // Parse "GroupName - FunctionName" format
        QStringList parts = selectedItemText.split(" - ");
        if (parts.size() == 2)
        {
            QString group_name = parts[0];
            QString function_name = parts[1];
            QByteArray utf8_bytes_GroupName = group_name.toUtf8();
            QByteArray utf8_bytes_FunctionName = function_name.toUtf8();

            if (m_SelectedConstraint)
            {
                // Create Relation Object
                FBBox *box_operator = m_SelectedConstraint->CreateFunctionBox(
                    utf8_bytes_GroupName.constData(),
                    utf8_bytes_FunctionName.constData());

                if (box_operator)
                {
                    // FIXME: Currently using random positions; should align box with dialog's OpenGL position
                    int x = 300 + dist(rng);
                    int y = 100 + dist(rng);
                    m_SelectedConstraint->SetBoxPosition(box_operator, x, y);
                }
            }
        }

        // Close Dialog
        accept();
    }

    // Model
    else
    {
        // Get Scene model with selected item name
        QByteArray utf8_bytes_ModelLabelName = selectedItemText.toUtf8();
        FBModel *selected_model = FBFindModelByLabelName(utf8_bytes_ModelLabelName.constData());
        if (!selected_model)
        {
            string error_str = "Error: \"" + selectedItemText.toStdString() + "\" is not found in the scene...";
            FBMessageBox("Warning", error_str.c_str(), "OK");

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
            QRect item_rect = listWidget->visualItemRect(item);

            // Show menu next to the current item
            QAction *action = menu->exec(listWidget->mapToGlobal(QPoint(item_rect.x() + item_rect.width(), item_rect.y())));

            if (action && m_SelectedConstraint)
            {
                FBBox *box_model = nullptr;

                if (action == action1)
                    box_model = m_SelectedConstraint->SetAsSource(selected_model);
                else if (action == action2)
                    box_model = m_SelectedConstraint->ConstrainObject(selected_model);

                if (box_model)
                {
                    // FIXME: Currently using random positions; should align box with dialog's OpenGL position
                    int x = 300 + dist(rng);
                    int y = 100 + dist(rng);
                    m_SelectedConstraint->SetBoxPosition(box_model, x, y);
                }

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

// Constructor for event filter installed to the QLineEdit
LineEditFilter::LineEditFilter(SearchDialog *parent)
    : QObject(parent), dialog(parent)
{
}

// Event filter logic
bool LineEditFilter::eventFilter(QObject *obj, QEvent *pEvent)
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(obj);
    if (lineEdit && dialog)
    {
        // Get child widget of parent dialog
        QRadioButton *radioButton_Operator = dialog->getOperatorRadioButton();
        QRadioButton *radioButton_Model = dialog->getModelRadioButton();
        QListWidget *listWidget = dialog->getListWidget();

        if (radioButton_Operator && radioButton_Model && listWidget)
        {
            // Handle key press
            if (pEvent->type() == QEvent::KeyPress)
            {
                QKeyEvent *keyEvent = static_cast<QKeyEvent *>(pEvent);
                int key = keyEvent->key();

                // Control 1 - Toggle between operator and model radio buttons
                if (key == Qt::Key_Tab)
                {
                    if (radioButton_Operator->isChecked())
                        radioButton_Model->setChecked(true);
                    else
                        radioButton_Operator->setChecked(true);

                    return true;
                }
                // Control 2 - Navigate dialog items using up/down key
                else if (key == Qt::Key_Up || key == Qt::Key_Down)
                {
                    int row = listWidget->currentRow();
                    int count = listWidget->count();

                    if (count == 0)
                        return true;

                    if (key == Qt::Key_Down)
                        row = (row + 1) % count;
                    else
                        row = (row - 1 + count) % count;

                    listWidget->setCurrentRow(row);

                    return true;
                }
                // Control 3 - Finalize selection and Create operator box or show model box menu
                else if (key == Qt::Key_Return || key == Qt::Key_Enter)
                {
                    dialog->finalize();
                    return true;
                }
            }
        }
    }

    return false; // Event not handled by this filter
}
