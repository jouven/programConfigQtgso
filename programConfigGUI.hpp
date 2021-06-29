
#ifndef PROGRAMCONFIGQTGSO_PROGRAMCONFIGGUI_HPP
#define PROGRAMCONFIGQTGSO_PROGRAMCONFIGGUI_HPP

#include "crossPlatformMacros.hpp"
#include "programConfigQtso/programConfig.hpp"

#include "textQtso/text.hpp"

#include <QObject>
#include <QHash>

class QWidget;

class EXPIMP_PROGRAMCONFIGQTGSO programConfigGUI_c : public programConfig_c
{
    Q_OBJECT

    QWidget* mainWindow_pri = nullptr;

    QHash<QString, QByteArray> widgetGeometryUMap_pri;
    //first key is dialog name/id, nested qmap key is datetime and value is the directory path
    QHash<QString, QMap<int_fast64_t, QString>> fileDialogNameToDirectoryNameAndTimeMap_pri;

    //max items in the fileDialogNameToDirectoryNameAndTimeMap_pri inner container, older items will be trimmed first
    uint_fast64_t pathHistoryMaxItems_pri = 10;

    virtual void readJSON_f(const QJsonObject &json_par_con) override;
    virtual void writeJSON_f(QJsonObject &json_par) const override;
    //virtual void writeJSONDocumented_f(QJsonObject &json_par) const;

    //message should be translated prior
    void createQMessagebox_f(
            const QString& message_par_con
            , const messageType_ec messageType_par_con) const;

    //these GUI versions will create-show BLOCKING simple qmessagebox windows with the message, certain options related to the type of the message and an OK button
    //don't use this for advanced dialogs, these are intended for easy use on any situation since they block and don't need any complex destroying/deletion
    //essentialQtg messagebox require a parent these don't
    virtual void derivedMessageUser_f(const text_c& message_par_con, const messageType_ec messageType_par_con) const override;
    virtual void derivedMessageUser_f(const textCompilation_c& message_par_con, const messageType_ec messageType_par_con) const override;
public:
    explicit programConfigGUI_c(QObject* parent_par);
    explicit programConfigGUI_c(QWidget* mainWindow_par);

    QByteArray widgetGeometry_f(const QString& key_par_con) const;
    void setWidgetGeometry_f(const QString& key_par_con, const QByteArray& windowGeometry_par_con);

    std::vector<QString> directoryHistory_f(const QString& fileDialogStringId_par_con) const;
    bool addDirectoryHistory_f(const QString& fileDialogStringId_par_con, const QString& directory_par_con);
private Q_SLOTS:
    //virtual, so it can be replaced with nothing
    //by default when a signal is sent to the program it will spawn a QMessagebox telling the user which signal + the program is closing
    virtual void OSSignalRecieved_f(int signal_par);
};

#endif // PROGRAMCONFIGQTSO_PROGRAMCONFIGGUI_HPP
