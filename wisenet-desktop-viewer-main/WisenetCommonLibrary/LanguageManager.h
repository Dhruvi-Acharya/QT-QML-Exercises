#pragma once

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QTranslator>
#include <QDebug>
#include <unordered_map>
#include "WisenetViewerDefine.h"

using namespace WisenetViewerDefine;

class LanguageManager: public QObject{
    Q_OBJECT
    Q_PROPERTY(int language READ language WRITE setLanguage NOTIFY languageChanged)

public:
    LanguageManager(QObject *parent = nullptr):
        QObject(parent), m_Language(-1){
    }

    static LanguageManager* getInstance(){
        static LanguageManager instance;
        return &instance;
    }

    virtual ~LanguageManager(){}

    /*
    void connectRemoveTranslator(const QObject *receiver, std::function <bool(QTranslator* t)> method)
    {
        connect(this, &LanguageManager::removeTranslator, receiver, method);
    }

    void connectInstallTranslator(const QObject *receiver, std::function <bool(QTranslator* t)> method)
    {
        connect(this, &LanguageManager::installTranslator, receiver, method);
    }*/

    int language() const{
        return m_Language;
    }

    void setLanguage(int lang){

        if(lang == m_Language)
            return;

        bool ret = false;
        qApp->removeTranslator(&m_Translator);

        // Resource (.qrc)에 파일을 넣어야 동작
        switch (lang)
        {
        case Language::Korean:
            ret = m_Translator.load(":/WisenetLanguage/lang_ko.qm");
            break;
        case Language::English_USA:
        case Language::English_UK:
            ret = m_Translator.load(":/WisenetLanguage/lang_en.qm");
            break;
        case Language::Chinese:
            ret = m_Translator.load(":/WisenetLanguage/lang_zh-cn.qm");
            break;
        case Language::Croatian:
            ret = m_Translator.load(":/WisenetLanguage/lang_hr.qm");
            break;
        case Language::Czech:
            ret = m_Translator.load(":/WisenetLanguage/lang_cs.qm");
            break;
        case Language::Danish:
            ret = m_Translator.load(":/WisenetLanguage/lang_da.qm");
            break;
        case Language::Dutch:
            ret = m_Translator.load(":/WisenetLanguage/lang_nl.qm");
            break;
        case Language::Finnish:
            ret = m_Translator.load(":/WisenetLanguage/lang_sv-fi.qm");
            break;
        case Language::French:
            ret = m_Translator.load(":/WisenetLanguage/lang_fr.qm");
            break;
        case Language::German:
            ret = m_Translator.load(":/WisenetLanguage/lang_de.qm");
            break;
        case Language::Greek:
            ret = m_Translator.load(":/WisenetLanguage/lang_el.qm");
            break;
        case Language::Hungarian:
            ret = m_Translator.load(":/WisenetLanguage/lang_hu.qm");
            break;
        case Language::Italian:
            ret = m_Translator.load(":/WisenetLanguage/lang_it.qm");
            break;
        case Language::Japanese:
            ret = m_Translator.load(":/WisenetLanguage/lang_ja.qm");
            break;
        case Language::Norwegian:
            ret = m_Translator.load(":/WisenetLanguage/lang_nb.qm");
            break;
        case Language::Polish:
            ret = m_Translator.load(":/WisenetLanguage/lang_pl.qm");
            break;
        case Language::Portuguese:
            ret = m_Translator.load(":/WisenetLanguage/lang_pt.qm");
            break;
        case Language::Romanian:
            ret = m_Translator.load(":/WisenetLanguage/lang_ro.qm");
            break;
        case Language::Russian:
            ret = m_Translator.load(":/WisenetLanguage/lang_ru.qm");
            break;
        case Language::Serbian:
            ret = m_Translator.load(":/WisenetLanguage/lang_sr.qm");
            break;
        case Language::Spanish:
            ret = m_Translator.load(":/WisenetLanguage/lang_es.qm");
            break;
        case Language::Taiwanese:
            ret = m_Translator.load(":/WisenetLanguage/lang_zh-tw.qm");
            break;
        case Language::Turkish:
            ret = m_Translator.load(":/WisenetLanguage/lang_tr.qm");
            break;
        default:
            ret = m_Translator.load(":/WisenetLanguage/lang_en.qm");
            break;
        }

        if(ret){
            qApp->installTranslator(&m_Translator);
        }else{
            qDebug() << "set will be default language.";
        }

        m_Language = lang;
        emit languageChanged();
    }
    Q_INVOKABLE int getLanguageEnum( QString langText )
    {
        auto ret = mLanguageEnumMap.find(langText);
        if( ret != mLanguageEnumMap.end() )
        {
            return ret->second;
        }
        return Language::English_USA;
    }
    Q_INVOKABLE QString getLanguageString( int langIndex )
    {
        for(auto& ret:mLanguageEnumMap)
        {
            if( ret.second==langIndex)
                return ret.first;
        }
        return "English-USA";
    }
    Q_INVOKABLE QString getCurrentLanguageString()
    {
        return getLanguageString(m_Language);
    }

    void setLanguageBySystemLocale() {
        QLocale systemLocale = QLocale::system();
        QLocale::Language language = systemLocale.language();

        if(language == QLocale::Korean) {
            setLanguage(getLanguageEnum("Korean"));
        }
        else if(language == QLocale::English) {
            if(systemLocale.country() == QLocale::UnitedKingdom)
                setLanguage(getLanguageEnum("English-UK"));
            else
                setLanguage(getLanguageEnum("English-USA"));
        }
        else if(language == QLocale::Chinese) {
            if(systemLocale.country() == QLocale::Taiwan)
                setLanguage(getLanguageEnum("Taiwanese"));
            else
                setLanguage(getLanguageEnum("Chinese"));
        }
        else if(language == QLocale::Croatian) {
            setLanguage(getLanguageEnum("Croatian"));
        }
        else if(language == QLocale::Czech) {
            setLanguage(getLanguageEnum("Czech"));
        }
        else if(language == QLocale::Danish) {
            setLanguage(getLanguageEnum("Danish"));
        }
        else if(language == QLocale::Dutch) {
            setLanguage(getLanguageEnum("Dutch"));
        }
        else if(language == QLocale::Finnish) {
            setLanguage(getLanguageEnum("Finnish"));
        }
        else if(language == QLocale::French) {
            setLanguage(getLanguageEnum("French"));
        }
        else if(language == QLocale::German) {
            setLanguage(getLanguageEnum("German"));
        }
        else if(language == QLocale::Greek) {
            setLanguage(getLanguageEnum("Greek"));
        }
        else if(language == QLocale::Hungarian) {
            setLanguage(getLanguageEnum("Hungarian"));
        }
        else if(language == QLocale::Italian) {
            setLanguage(getLanguageEnum("Italian"));
        }
        else if(language == QLocale::Japanese) {
            setLanguage(getLanguageEnum("Japanese"));
        }
        else if(language == QLocale::NorwegianBokmal)  {
            setLanguage(getLanguageEnum("Norwegian"));
        }
        else if(language == QLocale::Polish) {
            setLanguage(getLanguageEnum("Polish"));
        }
        else if(language == QLocale::Portuguese) {
            setLanguage(getLanguageEnum("Portuguese"));
        }
        else if(language == QLocale::Romanian) {
            setLanguage(getLanguageEnum("Romanian"));
        }
        else if(language == QLocale::Russian) {
            setLanguage(getLanguageEnum("Russian"));
        }
        else if(language == QLocale::Serbian) {
            setLanguage(getLanguageEnum("Serbian"));
        }
        else if(language == QLocale::Spanish) {
            setLanguage(getLanguageEnum("Spanish"));
        }
        else if(language == QLocale::Turkish) {
            setLanguage(getLanguageEnum("Turkish"));
        }
        else {
            setLanguage(getLanguageEnum("English-USA"));
        }
    }

signals:
    void languageChanged();

    //bool removeTranslator(QTranslator * messageFile);
    //bool installTranslator(QTranslator * messageFile);
public slots:
    void setLanguageString(QString& lang){
        qDebug() << "LanguageManager::setLanguageString:" << lang;
        setLanguage( WisenetLanguage::Convert(lang));
    }

private:
    QTranslator m_Translator;
    int m_Language;
    const std::unordered_map<QString, WisenetLanguage::Language> mLanguageEnumMap ={
        {"Korean", Language::Korean},
        {"English-USA", Language::English_USA},
        {"English-UK", Language::English_UK},
        {"Chinese", Language::Chinese},
        {"Croatian", Language::Croatian},
        {"Czech", Language::Czech},
        {"Danish", Language::Danish},
        {"Dutch", Language::Dutch},
        {"Finnish", Language::Finnish},
        {"French", Language::French},
        {"German", Language::German},
        {"Greek", Language::Greek},
        {"Hungarian", Language::Hungarian},
        {"Italian", Language::Italian},
        {"Japanese", Language::Japanese},
        {"Norwegian", Language::Norwegian},
        {"Polish", Language::Polish},
        {"Portuguese", Language::Portuguese},
        {"Romanian", Language::Romanian},
        {"Russian", Language::Russian},
        {"Serbian", Language::Serbian},
        {"Spanish", Language::Spanish},
        {"Taiwanese", Language::Taiwanese},
        {"Turkish", Language::Turkish}
    };
};
