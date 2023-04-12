#pragma once
#include <QObject>
#include "TsParser.hpp"
#include "TsPod.hpp"
#include <xlsxdocument.h>

class MainViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
public:
    explicit MainViewModel(QObject *parent = nullptr);

    QString path(){ return m_path; }
    void setPath(QString path){
        m_path = path;
        emit pathChanged(path);
    }

    void loadTs();
    void writeXlsx();
    void writeLanguage(QXlsx::Document& doc, TsPOD& ts, int column);

    void loadXlsx();
    TsPOD readXlsx(QXlsx::Document& doc, std::string language, int columnNumber);
    void writeTs();

    std::string QStringToStdString(QString str);
    QString StdStringToQString(std::string str);

public slots:
    void tsToXlsx();
    void xlsxToTs();

signals:
    void pathChanged(QString path);

private:
    QString m_path;

    TsParser m_tsParser;

    TsPOD m_english;
    TsPOD m_korean;
    TsPOD m_chinese_china;
    TsPOD m_croatian;
    TsPOD m_czech;
    TsPOD m_danish;
    TsPOD m_dutch;
    TsPOD m_finnish;
    TsPOD m_french;
    TsPOD m_german;
    TsPOD m_greek;
    TsPOD m_hungarian;
    TsPOD m_italian;
    TsPOD m_japanese;
    TsPOD m_norwegian;
    TsPOD m_polish;
    TsPOD m_portuguese;
    TsPOD m_romanian;
    TsPOD m_russian;
    TsPOD m_serbian;
    TsPOD m_spanish;
    TsPOD m_chinese_taiwan;
    TsPOD m_turkish;
};
