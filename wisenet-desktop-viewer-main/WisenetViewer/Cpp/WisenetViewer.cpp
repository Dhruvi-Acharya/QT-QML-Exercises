#include "WisenetViewer.h"

WisenetViewer::WisenetViewer()
{
    m_MultiWindowHandler = new MultiWindowHandler();
    m_MultiWindowHandler->initializeNewWindow();
}

WisenetViewer::~WisenetViewer()
{
    delete m_MultiWindowHandler;
}
