#include "multiwindowhandler.h"


MultiWindowHandler::MultiWindowHandler(QObject *parent) : QObject(parent)
{

}

void MultiWindowHandler::instatiateNewWindow()
{
    QString key = QUuid::createUuid().toString();
    qDebug() << "qml: window key :" << key;

    MultiWindow* multiWindow = new MultiWindow(key, this);
    m_windows.insert(key, multiWindow);

    qDebug() << "qml: Create new window, count : " << m_windows.count();
}

void MultiWindowHandler::destroyWindow(QString key)
{
    if(m_windows.contains(key))
    {
        MultiWindow * multiWindow = m_windows[key];
        m_windows.remove(key);

        multiWindow->deleteLater();
    }
    qDebug() << "qml: delete a window, count : " << m_windows.count();
}
