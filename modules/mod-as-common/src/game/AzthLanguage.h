#ifndef AZTHLANGUAGE_H
#define AZTHLANGUAGE_H

#include "Config.h"
#include "Common.h"
#include <unordered_map>

class Player;

class AzthLangString {
public:
    friend class AzthLang;

    AzthLangString(std::string const def, std::string const it);
    AzthLangString() {};
    ~AzthLangString();
private:
    std::string def;
    std::string it;
};

class AzthLang {

public:
    std::unordered_map<uint32, AzthLangString*> strings;
    explicit AzthLang();
    ~AzthLang();

    static AzthLang* instance();

    void loadStrings();
    virtual const char* get(uint32 strId, Player const* pl) const;
    virtual std::string getf(uint32 strId, Player const* pl, ...) const;
    void add(uint32 strId, std::string const def, std::string const it);
};

#define sAzthLang AzthLang::instance()
#endif  /* AZTHLANGUAGE_H */
