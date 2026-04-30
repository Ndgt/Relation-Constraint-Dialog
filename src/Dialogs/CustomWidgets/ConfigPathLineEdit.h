#pragma once

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>

/**
 * @class ConfigPathLineEdit
 * @brief Custom QLineEdit for displaying the config file path in the PreferencesDialog
 * @details This custom line edit is read-only and provides a context menu with options
 *          to copy the path or open the file location
 */
class ConfigPathLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for ConfigPathLineEdit
     * @param parent The parent widget
     */
    explicit ConfigPathLineEdit(QWidget *parent = nullptr);

    /**
     * @brief Set the text of the line edit with the specified path
     * @details This function also sets a flag to indicate that the config path has been set,
     *          which is used to enable or disable context menu actions
     * @param path The config file path to be displayed in the line edit
     */
    void setPathText(const QString &path);

private slots:
    /**
     * @brief Slot to handle the custom context menu request
     * @param pos The position where the context menu was requested
     * @details This slot creates a context menu with options to copy the path or open
     *          the file location. The options are enabled only if mConfigPathSet is set
     *          to true by the setPathText function.
     */
    void onCustomContextMenuRequested(const QPoint &pos);

private:
    bool mConfigPathSet = false; //!< Flag to indicate whether the config path text has been set
};