#include "programConfigGUI.hpp"

#include "essentialQtso/enumClass.hpp"
#include "signalProxyQtso/signalProxyQtso.hpp"
#include "comuso/practicalTemplates.hpp"

#include <QMessageBox>
#include <QJsonArray>
#include <QWidget>

#include <csignal>
#include <unordered_set>
#include <iterator>

void programConfigGUI_c::readJSON_f(const QJsonObject& json_par_con)
{
    programConfig_c::readJSON_f(json_par_con);
    if (not json_par_con["widgetGeometry"].isUndefined())
    {
        QJsonObject jsonWindowGeometryTmp(json_par_con["widgetGeometry"].toObject());
        if (not jsonWindowGeometryTmp.isEmpty() )
        {
            widgetGeometryUMap_pri.reserve(jsonWindowGeometryTmp.size());
            for (auto ite = jsonWindowGeometryTmp.constBegin(); ite not_eq jsonWindowGeometryTmp.constEnd(); ++ite)
            {
                widgetGeometryUMap_pri.insert(ite.key(), qUncompress(QByteArray::fromBase64(ite.value().toString().toUtf8())));
            }
        }
    }

    if (not json_par_con["fileDialogsSettings"].isUndefined())
    {
        QJsonArray jsonArraySelectedDirectoryHistoryTmp(json_par_con["fileDialogsSettings"].toArray());
        if (not jsonArraySelectedDirectoryHistoryTmp.isEmpty())
        {
            for (const auto jsonArrayFileDialogItem_ite_con : jsonArraySelectedDirectoryHistoryTmp)
            {
                QJsonObject fileDialogSettingObjectTmp(jsonArrayFileDialogItem_ite_con.toObject());
                QJsonArray jsonArraySelectedDirectoryHistoryTmp(fileDialogSettingObjectTmp["selectedDirectoryHistory"].toArray());
                if (not jsonArraySelectedDirectoryHistoryTmp.isEmpty())
                {
                    //put them in a vector because since they were written in the array in order,
                    //first one had the bigger/recent time,
                    //when using addDirectoryHistory_f, the first one from the array will get
                    //the smaller/later time, a reverse is necessary
                    std::vector<QString> directoryVectorTmp;
                    for (const auto jsonArrayDirectoryHistoryItem_ite_con : jsonArraySelectedDirectoryHistoryTmp)
                    {
                        directoryVectorTmp.emplace_back(jsonArrayDirectoryHistoryItem_ite_con.toString());
                    }
                    std::reverse(directoryVectorTmp.begin(), directoryVectorTmp.end());
                    for (const QString& directoryItem_ite_con : directoryVectorTmp)
                    {
                        addDirectoryHistory_f(fileDialogSettingObjectTmp["fileDialogStringId"].toString(), directoryItem_ite_con);
                    }
                }
            }
        }
    }
}

void programConfigGUI_c::writeJSON_f(QJsonObject& json_par) const
{
    programConfig_c::writeJSON_f(json_par);
    //create a filedialogsSettings arraay
    //and the object will have the directory history for now
    QJsonArray jsonArrayFileDialongSettingsTmp;
    QHash<QString, QMap<int_fast64_t, QString>>::const_iterator iteTmp(fileDialogNameToDirectoryNameAndTimeMap_pri.constBegin());
    while (iteTmp not_eq fileDialogNameToDirectoryNameAndTimeMap_pri.constEnd())
    {
        std::vector<QString> directoryHistoryTmp(directoryHistory_f(iteTmp.key()));
        if (not directoryHistoryTmp.empty())
        {
            QJsonArray jsonArraySelectedDirectoryHistoryTmp;
            for (const QString& directoryItem_ite_con : directoryHistoryTmp)
            {
                jsonArraySelectedDirectoryHistoryTmp.append(QJsonValue(directoryItem_ite_con));
            }
            QJsonObject fileDialogSettingsTmp;
            fileDialogSettingsTmp["fileDialogStringId"] = iteTmp.key();
            fileDialogSettingsTmp["selectedDirectoryHistory"] = jsonArraySelectedDirectoryHistoryTmp;
            jsonArrayFileDialongSettingsTmp.append(fileDialogSettingsTmp);
        }
        ++iteTmp;
    }
    json_par["fileDialogsSettings"] = jsonArrayFileDialongSettingsTmp;

    if (not widgetGeometryUMap_pri.isEmpty())
    {
        //addLogMessage_f("Geometry saved", QString(), messageType_ec::debug, MACRO_FILEMETA);
        QJsonObject pairsTmp;
        QHash<QString, QByteArray>::const_iterator iteratorTmp = widgetGeometryUMap_pri.constBegin();
        while (iteratorTmp not_eq widgetGeometryUMap_pri.constEnd())
        {
            QString qStringTmp;
            qStringTmp.append(qCompress(iteratorTmp.value()).toBase64());
            pairsTmp[iteratorTmp.key()] = qStringTmp;
            ++iteratorTmp;
        }
        json_par["widgetGeometry"] = pairsTmp;
    }
}

void programConfigGUI_c::createQMessagebox_f(
        const QString& message_par_con
        , const messageType_ec messageType_par_con) const
{
    QString titleTmp;
    QMessageBox::Icon iconTmp(QMessageBox::NoIcon);
    while (true)
    {
       if (messageType_par_con == messageType_ec::debug)
       {
           titleTmp = translate_f("Debug");
           //no icon
           break;
       }

       if (messageType_par_con == messageType_ec::information)
       {
           titleTmp = translate_f("Information");
           iconTmp = QMessageBox::Information;
           break;
       }

       if (messageType_par_con == messageType_ec::warning)
       {
           titleTmp = translate_f("Warning");
           iconTmp = QMessageBox::Warning;
           break;
       }

       if (messageType_par_con == messageType_ec::error)
       {
           titleTmp = translate_f("Error");
           iconTmp = QMessageBox::Critical;
           break;
       }

       if (messageType_par_con == messageType_ec::question)
       {
           titleTmp = translate_f("Question");
           iconTmp = QMessageBox::Question;
           break;
       }

       break;
    }

    QMessageBox messageBoxTmp;
    messageBoxTmp.setWindowTitle(titleTmp);
    messageBoxTmp.setText(message_par_con);
    messageBoxTmp.setModal(Qt::WindowModal);
    messageBoxTmp.setIcon(iconTmp);
    messageBoxTmp.exec();
}

void programConfigGUI_c::derivedMessageUser_f(const text_c& message_par_con, const messageType_ec messageType_par_con) const
{
    createQMessagebox_f(translateAndReplace_f(message_par_con), messageType_par_con);
}

void programConfigGUI_c::derivedMessageUser_f(const textCompilation_c& message_par_con, const messageType_ec messageType_par_con) const
{
    createQMessagebox_f(translateAndReplace_f(message_par_con), messageType_par_con);
}

programConfigGUI_c::programConfigGUI_c(QObject* parent_par)
    : programConfig_c(parent_par)
{
}

programConfigGUI_c::programConfigGUI_c(QWidget *mainWindow_par)
    : programConfig_c(mainWindow_par)
    , mainWindow_pri(mainWindow_par)
{
    if (mainWindow_par not_eq nullptr)
    {
        QObject::connect(signalso::signalProxy_ptr_ext, &signalso::signalProxy_c::signalTriggered_signal, this, &programConfigGUI_c::OSSignalRecieved_f);
    }
}

QByteArray programConfigGUI_c::widgetGeometry_f(const QString& key_par_con) const
{
    return widgetGeometryUMap_pri.value(key_par_con);
}

void programConfigGUI_c::setWidgetGeometry_f(const QString& key_par_con, const QByteArray& windowGeometry_par_con)
{
    widgetGeometryUMap_pri.insert(key_par_con, windowGeometry_par_con);
}

std::vector<QString> programConfigGUI_c::directoryHistory_f(const QString& fileDialogStringId_par_con) const
{
    std::vector<QString> historyVector;
    if (fileDialogNameToDirectoryNameAndTimeMap_pri.count(fileDialogStringId_par_con) > 0)
    {
        QMap<int_fast64_t, QString> accessTimeToDirectoryMapTmp(fileDialogNameToDirectoryNameAndTimeMap_pri.value(fileDialogStringId_par_con));
        historyVector.reserve(accessTimeToDirectoryMapTmp.size());
        //(from qt doc) "If you only need to extract the values from a map (not the keys), you can also use foreach:"
        for (const QString& directory_ite_con : accessTimeToDirectoryMapTmp)
        {
            historyVector.emplace_back(directory_ite_con);
        }
        //since the default order is using "less" the old ones will be first, reverse
        std::reverse(historyVector.begin(), historyVector.end());
    }
    return historyVector;
}

template <typename T>
void trimMap_f(T& container, const uint_fast64_t size_par_con)
{
    if (static_cast<uint_fast64_t>(container.size()) > size_par_con)
    {
        uint_fast64_t lastItemKeyTmp(container.firstKey());
        container.remove(lastItemKeyTmp);
    }
}

bool programConfigGUI_c::addDirectoryHistory_f(const QString& fileDialogStringId_par_con, const QString& directory_par_con)
{
    bool resultTmp(false);
    //if the widget has already an stored setting
    if (fileDialogNameToDirectoryNameAndTimeMap_pri.count(fileDialogStringId_par_con) > 0)
    {
        QHash<QString, QMap<int_fast64_t, QString>>::iterator accessTimeToDirectoryMapTmp(fileDialogNameToDirectoryNameAndTimeMap_pri.find(fileDialogStringId_par_con));
        //detect duplicates, this function is "slow" but it's a 10 items QMap
        uint_fast64_t keyFromValueTmp(accessTimeToDirectoryMapTmp->key(directory_par_con, 0));
        while (keyFromValueTmp not_eq 0)
        {
            accessTimeToDirectoryMapTmp->remove(keyFromValueTmp);
            keyFromValueTmp = accessTimeToDirectoryMapTmp->key(directory_par_con, 0);
        }

        int_fast64_t nowTmp(std::chrono::steady_clock::now().time_since_epoch().count());
        accessTimeToDirectoryMapTmp->insert(nowTmp, directory_par_con);
        trimMap_f(*accessTimeToDirectoryMapTmp, pathHistoryMaxItems_pri);
        resultTmp = true;
    }
    else
    {
        QMap<int_fast64_t, QString> accessTimeToDirectoryMapTmp;
        int_fast64_t nowTmp(std::chrono::steady_clock::now().time_since_epoch().count());
        accessTimeToDirectoryMapTmp.insert(nowTmp, directory_par_con);
        fileDialogNameToDirectoryNameAndTimeMap_pri.insert(fileDialogStringId_par_con, accessTimeToDirectoryMapTmp);
        trimMap_f(accessTimeToDirectoryMapTmp, pathHistoryMaxItems_pri);
        resultTmp = true;
    }

    return resultTmp;
}

void programConfigGUI_c::OSSignalRecieved_f(int signal_par)
{
    QMessageBox* informationMessageBoxPtrTmp(new QMessageBox(mainWindow_pri));
    informationMessageBoxPtrTmp->setAttribute(Qt::WA_DeleteOnClose);
    informationMessageBoxPtrTmp->setWindowTitle(translate_f("Warning"));
    text_c textTmp("OS signal {0} received, proceeding to close the program", signal_par);
    informationMessageBoxPtrTmp->setText(translateAndReplace_f(textTmp));
    informationMessageBoxPtrTmp->setModal(Qt::WindowModal);

    QObject::connect(informationMessageBoxPtrTmp, &QMessageBox::finished, mainWindow_pri, &QWidget::close);
    informationMessageBoxPtrTmp->show();
}
