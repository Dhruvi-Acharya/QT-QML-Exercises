#include "MainViewModel.h"
#include <sstream>
#include <QDebug>
#include <QFile>
#include <QDir>
#include "Reader.hpp"
#include "Writer.hpp"
#include "TsBuilder.hpp"

MainViewModel::MainViewModel(QObject *parent)
    : QObject{parent}
{

}

void MainViewModel::tsToXlsx()
{
    qDebug() << "MainViewModel::tsToXlsx()";
    loadTs();
    writeXlsx();
}

void MainViewModel::loadTs()
{
    qDebug() << "MainViewModel::loadTs()";

    QString englishPath = m_path + "\\lang_en.ts"; // English, UnitedStates
    QString koreanPath = m_path + "\\lang_ko.ts"; // Korean, SouthKorea
    QString chinese_chinaPath = m_path + "\\lang_zh-cn.ts"; // Chinese, China
    QString croatianPath = m_path + "\\lang_hr.ts"; // Croatian, Croatia
    QString czechPath = m_path + "\\lang_cs.ts"; // Czech, CzechRepublic
    QString danishPath = m_path + "\\lang_da.ts"; // Danish, Denmark
    QString dutchPath = m_path + "\\lang_nl.ts"; // Dutch, Netherlands
    QString finnishPath = m_path + "\\lang_sv-fi.ts"; // Finnish, Finland
    QString frenchPath = m_path + "\\lang_fr.ts"; // French, France
    QString germanPath = m_path + "\\lang_de.ts"; // German, Germany
    QString greekPath = m_path + "\\lang_el.ts"; // Greek, Greece
    QString hungarianPath = m_path + "\\lang_hu.ts"; // Hungarian, Hungary
    QString italianPath = m_path + "\\lang_it.ts"; // Italian, Italy
    QString japanesePath = m_path + "\\lang_ja.ts"; // Japanese, Japan
    QString norwegianPath = m_path + "\\lang_nb.ts"; // NorwegianBokmal, Norway
    QString polishPath = m_path + "\\lang_pl.ts"; // Polish, Poland
    QString portuguesePath = m_path + "\\lang_pt.ts"; // Portuguese, Portugal
    QString romanianPath = m_path + "\\lang_ro.ts"; // Romanian, Romania
    QString russianPath = m_path + "\\lang_ru.ts"; // Russian, Russia
    QString serbianPath = m_path + "\\lang_sr.ts"; // Serbian, Serbia
    QString spanishPath = m_path + "\\lang_es.ts"; // Spanish, Spain
    QString chinese_taiwanPath = m_path + "\\lang_zh-tw.ts"; // Chinese, Taiwan
    QString turkishPath = m_path + "\\lang_tr.ts"; // Turkish, Turkey

    if(QFile::exists(englishPath))
        m_english = m_tsParser.parse(Reader().read(QStringToStdString(englishPath)));
    if(QFile::exists(koreanPath))
        m_korean = m_tsParser.parse(Reader().read(QStringToStdString(koreanPath)));
    if(QFile::exists(chinese_chinaPath))
        m_chinese_china = m_tsParser.parse(Reader().read(QStringToStdString(chinese_chinaPath)));
    if(QFile::exists(croatianPath))
        m_croatian = m_tsParser.parse(Reader().read(QStringToStdString(croatianPath)));
    if(QFile::exists(czechPath))
        m_czech = m_tsParser.parse(Reader().read(QStringToStdString(czechPath)));
    if(QFile::exists(danishPath))
        m_danish = m_tsParser.parse(Reader().read(QStringToStdString(danishPath)));
    if(QFile::exists(dutchPath))
        m_dutch = m_tsParser.parse(Reader().read(QStringToStdString(dutchPath)));
    if(QFile::exists(finnishPath))
        m_finnish = m_tsParser.parse(Reader().read(QStringToStdString(finnishPath)));
    if(QFile::exists(frenchPath))
        m_french = m_tsParser.parse(Reader().read(QStringToStdString(frenchPath)));
    if(QFile::exists(germanPath))
        m_german = m_tsParser.parse(Reader().read(QStringToStdString(germanPath)));
    if(QFile::exists(greekPath))
        m_greek = m_tsParser.parse(Reader().read(QStringToStdString(greekPath)));
    if(QFile::exists(hungarianPath))
        m_hungarian = m_tsParser.parse(Reader().read(QStringToStdString(hungarianPath)));
    if(QFile::exists(italianPath))
        m_italian = m_tsParser.parse(Reader().read(QStringToStdString(italianPath)));
    if(QFile::exists(japanesePath))
        m_japanese = m_tsParser.parse(Reader().read(QStringToStdString(japanesePath)));
    if(QFile::exists(norwegianPath))
        m_norwegian = m_tsParser.parse(Reader().read(QStringToStdString(norwegianPath)));
    if(QFile::exists(polishPath))
        m_polish = m_tsParser.parse(Reader().read(QStringToStdString(polishPath)));
    if(QFile::exists(portuguesePath))
        m_portuguese = m_tsParser.parse(Reader().read(QStringToStdString(portuguesePath)));
    if(QFile::exists(romanianPath))
        m_romanian = m_tsParser.parse(Reader().read(QStringToStdString(romanianPath)));
    if(QFile::exists(russianPath))
        m_russian = m_tsParser.parse(Reader().read(QStringToStdString(russianPath)));
    if(QFile::exists(serbianPath))
        m_serbian = m_tsParser.parse(Reader().read(QStringToStdString(serbianPath)));
    if(QFile::exists(spanishPath))
        m_spanish = m_tsParser.parse(Reader().read(QStringToStdString(spanishPath)));
    if(QFile::exists(chinese_taiwanPath))
        m_chinese_taiwan = m_tsParser.parse(Reader().read(QStringToStdString(chinese_taiwanPath)));
    if(QFile::exists(turkishPath))
        m_turkish = m_tsParser.parse(Reader().read(QStringToStdString(turkishPath)));
}

void MainViewModel::writeXlsx()
{
    qDebug() << "MainViewModel::writeXlsx()";
    QXlsx::Document doc;
    int col = 1;
    int row = 1;

    doc.write(row, col++, "line");
    doc.write(row, col++, "key");
    doc.write(row, col++, "Korean");
    doc.write(row, col++, "English");
    doc.write(row, col++, "Chinese");
    doc.write(row, col++, "Croatian");
    doc.write(row, col++, "Czech");
    doc.write(row, col++, "Danish");
    doc.write(row, col++, "Dutch");
    doc.write(row, col++, "Finnish");
    doc.write(row, col++, "French");
    doc.write(row, col++, "German");
    doc.write(row, col++, "Greek");
    doc.write(row, col++, "Hungarian");
    doc.write(row, col++, "Italian");
    doc.write(row, col++, "Japanese");
    doc.write(row, col++, "Norwegian");
    doc.write(row, col++, "Polish");
    doc.write(row, col++, "Portuguese");
    doc.write(row, col++, "Romanian");
    doc.write(row, col++, "Russian");
    doc.write(row, col++, "Serbian");
    doc.write(row, col++, "Spanish");
    doc.write(row, col++, "Taiwanese");
    doc.write(row, col++, "Turkish");


    TsPOD ts = m_korean;
    for (const auto &c : ts)
    {
        for (const auto &d : c.translations)
        {
            ++row;
            col = 1;

            for (unsigned long long i = 0; i < ts.max_locations; ++i)
            {
                if (d.locations.size() != 0 && i <= d.locations.size() - 1)
                {
                    doc.write(row, col++, QString::number(d.locations[i].line));
                }
            }

            col = 2;

            doc.write(row, col++, QString::fromStdString(d.source));
            doc.write(row, col++, QString::fromStdString(d.tr));
        }
    }

    writeLanguage(doc, m_english, col++);
    writeLanguage(doc, m_chinese_china, col++);
    writeLanguage(doc, m_croatian, col++);
    writeLanguage(doc, m_czech, col++);
    writeLanguage(doc, m_danish, col++);
    writeLanguage(doc, m_dutch, col++);
    writeLanguage(doc, m_finnish, col++);
    writeLanguage(doc, m_french, col++);
    writeLanguage(doc, m_german, col++);
    writeLanguage(doc, m_greek, col++);
    writeLanguage(doc, m_hungarian, col++);
    writeLanguage(doc, m_italian, col++);
    writeLanguage(doc, m_japanese, col++);
    writeLanguage(doc, m_norwegian, col++);
    writeLanguage(doc, m_polish, col++);
    writeLanguage(doc, m_portuguese, col++);
    writeLanguage(doc, m_romanian, col++);
    writeLanguage(doc, m_russian, col++);
    writeLanguage(doc, m_serbian, col++);
    writeLanguage(doc, m_spanish, col++);
    writeLanguage(doc, m_chinese_taiwan, col++);
    writeLanguage(doc, m_turkish, col++);

    doc.saveAs(m_path + "\\language.xlsx");
}

void MainViewModel::writeLanguage(QXlsx::Document& doc, TsPOD& ts, int column)
{
    for (const auto &c : ts)
    {
        int row = 1;
        column;

        for (const auto &d : c.translations)
        {
            ++row;
            doc.write(row, column, QString::fromStdString(d.tr));
        }
    }
}

void MainViewModel::xlsxToTs()
{
    qDebug() << "MainViewModel::xlsxToTs()";
    loadXlsx();
    writeTs();
}

void MainViewModel::loadXlsx()
{
    qDebug() << "MainViewModel::loadXlsx()";

    QXlsx::Document doc(m_path + "\\language.xlsx");

    int column = 3;

    m_korean = readXlsx(doc, "ko_KR", column++);
    m_english = readXlsx(doc, "en_US", column++);
    m_chinese_china = readXlsx(doc, "zh_CN", column++);
    m_croatian = readXlsx(doc, "hr_HR", column++);
    m_czech = readXlsx(doc, "cs_CZ", column++);
    m_danish = readXlsx(doc, "da_DK", column++);
    m_dutch = readXlsx(doc, "nl_NL", column++);
    m_finnish = readXlsx(doc, "sv_FI", column++);
    m_french = readXlsx(doc, "fr_FR", column++);
    m_german = readXlsx(doc, "de_DE", column++);
    m_greek = readXlsx(doc, "el_GR", column++);
    m_hungarian = readXlsx(doc, "hu_HU", column++);
    m_italian = readXlsx(doc, "it_IT", column++);
    m_japanese = readXlsx(doc, "ja_JP", column++);
    m_norwegian = readXlsx(doc, "nb_NO", column++);
    m_polish = readXlsx(doc, "pl_PL", column++);
    m_portuguese = readXlsx(doc, "pt_BR", column++);
    m_romanian = readXlsx(doc, "ro_RO", column++);
    m_russian = readXlsx(doc, "ru_RU", column++);
    m_serbian = readXlsx(doc, "sr_RS", column++);
    m_spanish = readXlsx(doc, "es_ES", column++);
    m_chinese_taiwan = readXlsx(doc, "zh_TW", column++);
    m_turkish = readXlsx(doc, "tr_TR", column++);
}

TsPOD MainViewModel::readXlsx(QXlsx::Document& doc, std::string language, int columnNumber)
{
    TsPOD tsPOD;
    tsPOD.language = language;
    tsPOD.version = "2.1";

    Context context;
    context.name = "WisenetLinguist";

    int row = 2;
    std::string cell = "";

    while (!doc.read(row, 1).toString().toStdString().empty()) {
        Translation translation;

        // line
        cell = doc.read(row, 1).toString().toStdString();
        Location l;
        l.path = "WisenetLinguist.qml";
        l.line = static_cast<unsigned>(std::stoi(cell));
        translation.locations.emplace_back(std::move(l));

        // key
        cell = doc.read(row, 2).toString().toStdString();
        translation.source = cell;

        // translation
        cell = doc.read(row, columnNumber).toString().toStdString();
        translation.tr = cell;

        context.translations.emplace_back(std::move(translation));

        row++;
    }

    tsPOD.emplace_back(std::move(context));

    return tsPOD;
}

void MainViewModel::writeTs()
{
    qDebug() << "MainViewModel::writeTs()";

    QDir dir(m_path);

    if(!dir.exists("output"))
        dir.mkdir("output");

    Writer().write(TsBuilder().build(std::move(m_korean)), QStringToStdString(m_path + "\\output\\lang_ko.ts"));
    Writer().write(TsBuilder().build(std::move(m_english)), QStringToStdString(m_path + "\\output\\lang_en.ts"));
    Writer().write(TsBuilder().build(std::move(m_chinese_china)), QStringToStdString(m_path + "\\output\\lang_zh-cn.ts"));
    Writer().write(TsBuilder().build(std::move(m_croatian)), QStringToStdString(m_path + "\\output\\lang_hr.ts"));
    Writer().write(TsBuilder().build(std::move(m_czech)), QStringToStdString(m_path + "\\output\\lang_cs.ts"));
    Writer().write(TsBuilder().build(std::move(m_danish)), QStringToStdString(m_path + "\\output\\lang_da.ts"));
    Writer().write(TsBuilder().build(std::move(m_dutch)), QStringToStdString(m_path + "\\output\\lang_nl.ts"));
    Writer().write(TsBuilder().build(std::move(m_finnish)), QStringToStdString(m_path + "\\output\\lang_sv-fi.ts"));
    Writer().write(TsBuilder().build(std::move(m_french)), QStringToStdString(m_path + "\\output\\lang_fr.ts"));
    Writer().write(TsBuilder().build(std::move(m_german)), QStringToStdString(m_path + "\\output\\lang_de.ts"));
    Writer().write(TsBuilder().build(std::move(m_greek)), QStringToStdString(m_path + "\\output\\lang_el.ts"));
    Writer().write(TsBuilder().build(std::move(m_hungarian)), QStringToStdString(m_path + "\\output\\lang_hu.ts"));
    Writer().write(TsBuilder().build(std::move(m_italian)), QStringToStdString(m_path + "\\output\\lang_it.ts"));
    Writer().write(TsBuilder().build(std::move(m_japanese)), QStringToStdString(m_path + "\\output\\lang_ja.ts"));
    Writer().write(TsBuilder().build(std::move(m_norwegian)), QStringToStdString(m_path + "\\output\\lang_nb.ts"));
    Writer().write(TsBuilder().build(std::move(m_polish)), QStringToStdString(m_path + "\\output\\lang_pl.ts"));
    Writer().write(TsBuilder().build(std::move(m_portuguese)), QStringToStdString(m_path + "\\output\\lang_pt.ts"));
    Writer().write(TsBuilder().build(std::move(m_romanian)), QStringToStdString(m_path + "\\output\\lang_ro.ts"));
    Writer().write(TsBuilder().build(std::move(m_russian)), QStringToStdString(m_path + "\\output\\lang_ru.ts"));
    Writer().write(TsBuilder().build(std::move(m_serbian)), QStringToStdString(m_path + "\\output\\lang_sr.ts"));
    Writer().write(TsBuilder().build(std::move(m_spanish)), QStringToStdString(m_path + "\\output\\lang_es.ts"));
    Writer().write(TsBuilder().build(std::move(m_chinese_taiwan)), QStringToStdString(m_path + "\\output\\lang_zh-tw.ts"));
    Writer().write(TsBuilder().build(std::move(m_turkish)), QStringToStdString(m_path + "\\output\\lang_tr.ts"));
}

std::string MainViewModel::QStringToStdString(QString str)
{
    std::string ret = str.toStdString();

    return ret;
}

QString MainViewModel::StdStringToQString(std::string str)
{
    QString ret = QString::fromStdString(str);

    return ret;
}
