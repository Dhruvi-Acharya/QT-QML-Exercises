#include "TsParser.hpp"

TsPOD TsParser::parse(std::string &&content)
{
    rapidxml::xml_document<> doc;
    doc.parse<0>(const_cast<char *>(content.c_str()));

    TsPOD ret;
    auto node = doc.first_node();
    ret.version = node->first_attribute()->value();
    ret.language = node->first_attribute()->next_attribute()->value();
    while (node != nullptr) { // context
        auto bro = node->first_node();
        while (bro != nullptr) { // name/message
            auto child = bro->first_node();
            Context c;
            if (child->name() == std::string("name")) {
                c.name = child->value();
                child = child->next_sibling();
            }
            while (child != nullptr) { // location/source/translation
                auto att = child->first_node();
                Translation t;
                while (att != nullptr) {
                    if (att->name() == std::string("location")) {
                        Location loc;
                        if (check_attribute_type(att->first_attribute("type"))) {
                            loc.path = "";
                            loc.line = 0;
                        } else {
                            loc.path = att->first_attribute()->value();
                            const auto v = att->first_attribute()->next_attribute()->value();
                            loc.line = static_cast<uint16_t>(std::stoi(v));
                        }
                        t.locations.emplace_back(std::move(loc));
                    } else {
                        if (att->name() != std::string("")) {
                            if (att->name() == std::string("source")) {
                                if (check_attribute_type(att->first_attribute("type"))) {
                                    t.source = "";
                                } else {
                                    t.source = att->value();
                                }
                            } else {
                                if (check_attribute_type(att->first_attribute("type"))) {
                                    t.tr = "";
                                } else {
                                    t.tr = att->value();
                                }
                            }
                        }
                    }
                    att = att->next_sibling();
                }
                c.translations.emplace_back(std::move(t));
                child = child->next_sibling();
            }
            ret.emplace_back(std::move(c));
            bro = bro->next_sibling();
        }
        node = node->next_sibling();
    }

    //delete_empty_translations(&ret);
    //delete_empty_context(&ret);
    ret.max_locations = find_max_locations(ret);
    return ret;
}

void TsParser::delete_empty_context(TsPOD *ts) const
{
    size_t i = 0;
    while (i < ts->size()) {
        if (ts->at(i).translations.empty()) {
            ts->erase(ts->cbegin() + static_cast<int32_t>(i));
            continue;
        }
        ++i;
    }
}

void TsParser::delete_empty_translations(TsPOD *ts) const
{
    for (auto &c : *ts) {
        size_t i = 0;
        while (i < c.translations.size()) {
            if (c.translations[i].tr.empty()) {
                c.translations.erase(c.translations.begin() +
                                     static_cast<int32_t>(i));
                continue;
            }
            ++i;
        }
    }
}

uint16_t TsParser::find_max_locations(const TsPOD &ts)
{
    uint16_t ret = 0;
    for (const auto &c : ts) {
        if (c.translations.front().locations.size() > ret) {
            ret = static_cast<uint16_t>(c.translations.front().locations.size());
        }
    }
    return ret;
}

bool TsParser::check_attribute_type(rapidxml::xml_attribute<char> *att) {
    return att != nullptr &&
        (att->value() == std::string("vanished") ||
         att->value() == std::string("obsolete"));
}
