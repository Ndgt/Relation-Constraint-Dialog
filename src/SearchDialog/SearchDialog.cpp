#include "CustomLineEdit.h"
#include "RelationDialogManager.h"
#include "SearchDialog.h"
#include "SuggestionProvider.h"
#include "Utility.h"

#include <QtCore/QRect>
#include <QtCore/QStringList>
#include <QtCore/QtConfig>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>
#include <QtGui/QPalette>
#include <QtGui/QPainter>
#include <QtWidgets/QMenu>
#include <QtGui/QDesktopServices>

#if QT_VERSION_MAJOR >= 6
#include <QtGui/QAction>
#include <QtGui/QActionGroup>
#else
#include <QtWidgets/QAction>
#include <QtWidgets/QActionGroup>
#endif

const QString MOBU_HELP_FALLBACK_URL = "https://help.autodesk.com/view/MOBPRO/2027/ENU/";
const QString MOBU_HELP_LANGUAGE = "ENU";
const QString RELATION_REFERENCE_HELP_GUID = "GUID-C50152F9-5607-4779-A964-186B4E1A0601";
const QString GITHUB_REPOSITORY_URL = "https://github.com/Ndgt/Relation-Constraint-Dialog";

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

    Qt::WindowFlags flags;

    // Set the dialog style to be a popup and delete automatically on close
    setAttribute(Qt::WA_DeleteOnClose);
    flags |= Qt::Popup;

    // Set addtinal attributes and styles so that the overridden paintEvent will work
    // and the dialog will have rounded corners
    setAttribute(Qt::WA_TranslucentBackground);
    flags |= Qt::FramelessWindowHint;

    setWindowFlags(flags);

    // Initialize actions for the settings menu and connect their signals to the corresponding slots
    initializeActions();

    // Connect Singals & Slots
    connect(ui->lineEdit, &CustomLineEdit::keyReturnPressed, this, &SearchDialog::onLineEditKeyReturnPressed);
    connect(ui->lineEdit, &CustomLineEdit::keyTabPressed, this, &SearchDialog::onLineEditKeyTabPressed);
    connect(ui->lineEdit, &CustomLineEdit::keyUpDownPressed, this, &SearchDialog::onLineEditKeyUpDownPressed);
    connect(ui->lineEdit, &CustomLineEdit::textChanged, this, &SearchDialog::onTextChanged);
    connect(ui->listWidget, &QListWidget::itemClicked, this, &SearchDialog::onItemClicked);
    connect(ui->buttonSettings, &QPushButton::clicked, this, &SearchDialog::onSettingsButtonClicked);

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

    // Cache model suggestions for better performance
    // since model list is not expected to change during the dialog lifetime
    mCachedModelModelSuggestions = SuggestionProvider::getInstance().getModelSuggestions();
}

void SearchDialog::initializeActions()
{
    QActionGroup *operatorSettingsActionGroup = new QActionGroup(this);
    mSettingsActionOperatorOpe = new QAction("Show hit Operator first", this);
    mSettingsActionOperatorCat = new QAction("Show hit Category first", this);
    mSettingsActionOperatorDef = new QAction("Show in definition order", this);
    mSettingsActionOperatorOpe->setCheckable(true);
    mSettingsActionOperatorCat->setCheckable(true);
    operatorSettingsActionGroup->addAction(mSettingsActionOperatorOpe);
    operatorSettingsActionGroup->addAction(mSettingsActionOperatorCat);
    mSettingsActionOperatorOpe->setChecked(true); // default selection

    QActionGroup *modelSettingsActionGroup = new QActionGroup(this);
    mSettingsActionModelAll = new QAction("Search all models", this);
    mSettingsActionModelSkeleton = new QAction("Search only skeleton models", this);
    mSettingsActionModelAll->setCheckable(true);
    mSettingsActionModelSkeleton->setCheckable(true);
    modelSettingsActionGroup->addAction(mSettingsActionModelAll);
    modelSettingsActionGroup->addAction(mSettingsActionModelSkeleton);
    mSettingsActionModelAll->setChecked(true); // default selection

    QActionGroup *helpSettingsActionGroup = new QActionGroup(this);
    mSettingsActionHelpReference = new QAction("Relation Reference", this);
    mSettingsActionHelpGitHub = new QAction("GitHub Repository", this);
    helpSettingsActionGroup->addAction(mSettingsActionHelpReference);
    helpSettingsActionGroup->addAction(mSettingsActionHelpGitHub);
    helpSettingsActionGroup->setExclusive(false);

    connect(operatorSettingsActionGroup, &QActionGroup::triggered, this, &SearchDialog::onSettingsActionOperatorTriggered);
    connect(modelSettingsActionGroup, &QActionGroup::triggered, this, &SearchDialog::onSettingsActionModelTriggered);
    connect(helpSettingsActionGroup, &QActionGroup::triggered, this, &SearchDialog::onSettingsActionHelpTriggered);
}

void SearchDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(palette().window());
    painter.drawRoundedRect(rect(), ui->frame->property("BorderRadius").toUInt(), ui->frame->property("BorderRadius").toUInt());
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

            if (operatorTypeName == "My Macros")
            {
                // Check for macro recursivity
                if (checkMacroRecursivity(std::string(mSelectedConstraint->Name.AsString()), std::string(operatorName.toStdString())))
                {
                    QString errorStr = "[Error] Macro \"" + operatorName + "\" cannot be added due to recursivity.";
                    QByteArray errorStrBytes = errorStr.toUtf8();
                    FBMessageBox("Relation Constraint Dialog", errorStrBytes.constData(), "OK");

                    // Close Dialog
                    accept();

                    // Do not create the macro box
                    return;
                }
            }

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
        allSuggestions = mCachedModelModelSuggestions;

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

void SearchDialog::onSettingsButtonClicked(bool checked)
{
    Q_UNUSED(checked);

    QMenu *menu = new QMenu(this);
    QMenu *operatorOptionMenu = menu->addMenu("Operators");
    QMenu *modelOptionMenu = menu->addMenu("Models");
    menu->addSeparator();
    QMenu *onlineHelpMenu = menu->addMenu("Online Help");

    operatorOptionMenu->addAction(mSettingsActionOperatorOpe);
    operatorOptionMenu->addAction(mSettingsActionOperatorCat);
    modelOptionMenu->addAction(mSettingsActionModelAll);
    modelOptionMenu->addAction(mSettingsActionModelSkeleton);
    onlineHelpMenu->addAction(mSettingsActionHelpReference);
    onlineHelpMenu->addAction(mSettingsActionHelpGitHub);

    menu->exec(ui->buttonSettings->mapToGlobal(ui->buttonSettings->rect().topRight()));
}

void SearchDialog::onSettingsActionOperatorTriggered(QAction *action)
{
    if (action == mSettingsActionOperatorOpe)
    {
    }
    else if (action == mSettingsActionOperatorCat)
    {
    }
    else if (action == mSettingsActionOperatorDef)
    {
    }
}

void SearchDialog::onSettingsActionModelTriggered(QAction *action)
{
    if (action == mSettingsActionModelAll)
    {
    }
    else if (action == mSettingsActionModelSkeleton)
    {
    }
}

void SearchDialog::onSettingsActionHelpTriggered(QAction *action)
{
    if (!action)
        return;

    QUrl helpUrl;

    if (action == mSettingsActionHelpReference)
    {
        // Get product version
#if defined(PRODUCT_VERSION)
        int version = PRODUCT_VERSION;
#else
        // FBSystem::Version returns a value in the "xx000" format for version 20xx.
        int version = 2000 + (FBSystem::TheOne().Version.AsInt() / 1000);
#endif

        // Construct help URL with the version and language
        QUrl helpUrl;
        helpUrl.setScheme("https");
        helpUrl.setHost("help.autodesk.com");
        helpUrl.setPath(QString("/view/MOBPRO/%1/%2/").arg(version).arg(MOBU_HELP_LANGUAGE));

        // Set query parameter
        QUrlQuery query;
        query.addQueryItem("guid", RELATION_REFERENCE_HELP_GUID);
        helpUrl.setQuery(query);

        // Check if the help URL is valid, if not, fallback to a top-level help page
        if (!helpUrl.isValid())
            helpUrl = QUrl(MOBU_HELP_FALLBACK_URL);
    }
    else if (action == mSettingsActionHelpGitHub)
    {
        // Set the GitHub repository URL
        helpUrl = QUrl(GITHUB_REPOSITORY_URL);
    }

    QDesktopServices::openUrl(helpUrl);
}