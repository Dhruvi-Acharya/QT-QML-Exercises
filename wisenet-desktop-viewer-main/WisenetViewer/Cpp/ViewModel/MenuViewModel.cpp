#include "MenuViewModel.h"

MenuViewModel::MenuViewModel(QObject *parent) : QObject(parent)
{
    qDebug() << "MenuViewModel()";
}

MenuViewModel::~MenuViewModel()
{
    qDebug() << "~MenuViewModel()";
}
