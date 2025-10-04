#pragma once
#include "Types.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include <vector>

struct JsonComponent {
    JsonComponent(const rapidjson::Value& value) : _value(value) {}

    inline bool has(const RdString& key) { return _value["members"].HasMember(key.c_str()); }
    double getDouble(RdString key, double defaultValue = double());
    float getFloat(RdString key, float defaultValue = float());
    bool getBoolean(RdString key, bool defaultValue = bool());
    RdString getString(RdString key, RdString defaultValue = RdString()); 
    std::vector<RdString> getStrings(std::string key, std::vector<RdString> defaultValue = {});
    int getInteger(RdString key, int defaultValue = int());
    unsigned getUint(RdString key, unsigned defaultValue = unsigned());
    RdMatrix getMatrix(RdString key);
    RdQuaternion getQuaternion(RdString key);
    RdVector getVector(RdString key);
    RdVectorF getVectorF(RdString key);
    RdPoints getPoints(RdString key);
    RdUints getUints(RdString key);
    RdColor getColor(RdString key);
    RdString getEntity(RdString key);
    RdString getPath(RdString key);
    RdString getData();

private:
    const rapidjson::Value& _value;
};

struct JsonRegistry {
    JsonRegistry(const rapidjson::Document& doc) : _doc(doc) {}
    JsonComponent get(RdString entity, const char* component) const;
    bool has(RdString entity, const char* component) const;

    const rapidjson::Document& _doc;
};

using RdJsonRegistry = JsonRegistry;