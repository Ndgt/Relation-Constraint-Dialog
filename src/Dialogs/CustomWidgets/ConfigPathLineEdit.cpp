#include "ConfigPathLineEdit.h"

#include <filesystem>

#include <QtCore/QtConfig>
#include <QtCore/QUrl>
#include <QtGui/QClipboard>
#include <QtGui/QGuiApplication>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QMenu>

#if QT_VERSION_MAJOR >= 6
#include <QtGui/QAction>
#else
#include <QtWidgets/QAction>
#endif

#include <fbsdk/fbsdk.h>

ConfigPathLineEdit::ConfigPathLineEdit(QWidget *parent) : QLineEdit(parent)
{
    if (contextMenuPolicy() != Qt::CustomContextMenu)
        setContextMenuPolicy(Qt::CustomContextMenu);

    if (!isReadOnly())
        setReadOnly(true);

    connect(this, &QLineEdit::customContextMenuRequested, this, &ConfigPathLineEdit::onCustomContextMenuRequested);
}

void ConfigPathLineEdit::setPathText(const QString &path)
{
    setText(path);
    mConfigPathSet = true;
}

void ConfigPathLineEdit::onCustomContextMenuRequested(const QPoint &pos)
{
    QMenu contextMenu(this);
    QAction *copyAction = contextMenu.addAction("Copy Path");
    QAction *openLocationAction = contextMenu.addAction("Open File Location");

    if (!mConfigPathSet)
    {
        copyAction->setEnabled(false);
        openLocationAction->setEnabled(false);
    }

    QAction *selectedAction = contextMenu.exec(mapToGlobal(pos));

    if (selectedAction == copyAction)
    {
        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setText(text());
    }
    else if (selectedAction == openLocationAction)
    {
        std::filesystem::path configFilePath(text().toStdString());

        if (!std::filesystem::exists(configFilePath))
        {
            FBMessageBox("Relation Constraint Dialog",
                         "[Error] The file path does not exist.",
                         "OK");
        }
        else
        {
            QString fileLocation = QString::fromStdString(configFilePath.parent_path().string());
            QDesktopServices::openUrl(QUrl::fromLocalFile(fileLocation));
        }
    }
}