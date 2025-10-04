#include "Api.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "HellLogging.h"

bool JsonRegistry::has(RdString entity, const char* component) const {
    return _doc["entities"][entity.c_str()]["components"].HasMember(component);
}

JsonComponent JsonRegistry::get(RdString entity, const char* component) const {
    if (!_doc["entities"].HasMember(entity.c_str())) {
        throw std::runtime_error(entity + " did not exist");
    }

    if (!_doc["entities"][entity.c_str()]["components"].HasMember(component)) {
        throw std::runtime_error(entity + " did not have " + component);
    }

    const rapidjson::Value& value = _doc["entities"][entity.c_str()]["components"][component];
    return { value };
}

double JsonComponent::getDouble(std::string key, double defaultValue) {
    if (!has(key)) {
        return defaultValue;
    }

    return _value["members"][key.c_str()].GetDouble();
}

float JsonComponent::getFloat(std::string key, float defaultValue) {
    if (!has(key)) {
        return defaultValue;
    }

    return static_cast<float>(_value["members"][key.c_str()].GetDouble());
}

bool JsonComponent::getBoolean(std::string key, bool defaultValue) {
    if (!has(key)) {
        return defaultValue;
    }

    return _value["members"][key.c_str()].GetBool();
}

RdString JsonComponent::getString(std::string key, RdString defaultValue) {
    if (!has(key)) {
        return defaultValue;
    }

    return _value["members"][key.c_str()].GetString();
}

std::vector<RdString> JsonComponent::getStrings(std::string key, std::vector<RdString> defaultValue) {
    if (!has(key)) return defaultValue;

    const auto& v = _value["members"][key.c_str()];

    if (v.IsArray()) {
        std::vector<RdString> out;
        out.reserve(v.Size());
        for (const auto& it : v.GetArray()) {
            if (it.IsString()) out.emplace_back(it.GetString());
        }
        return out.empty() ? defaultValue : out;
    }

    if (v.IsString()) {
        return { RdString(v.GetString()) };
    }

    return defaultValue;
}

int JsonComponent::getInteger(std::string key, int defaultValue) {
    if (!has(key)) {
        return defaultValue;
    }

    return _value["members"][key.c_str()].GetInt();
}

unsigned JsonComponent::getUint(std::string key, unsigned defaultValue) {
    if (!has(key)) {
        return defaultValue;
    }

    return _value["members"][key.c_str()].GetUint();
}

RdMatrix JsonComponent::getMatrix(std::string key) {
    if (!_value["members"].HasMember(key.c_str())) {
        Logging::Error() << key << " does not exist";
        return RdMatrix(RdIdentityInit);
    }

    RdMatrix mtx{ RdIdentityInit };

    const auto& v = _value["members"][key.c_str()]["values"];
    assert(v.Size() == 16);

    mtx[0][0] = v[0].GetDouble();
    mtx[0][1] = v[1].GetDouble();
    mtx[0][2] = v[2].GetDouble();
    mtx[0][3] = v[3].GetDouble();

    mtx[1][0] = v[4].GetDouble();
    mtx[1][1] = v[5].GetDouble();
    mtx[1][2] = v[6].GetDouble();
    mtx[1][3] = v[7].GetDouble();

    mtx[2][0] = v[8].GetDouble();
    mtx[2][1] = v[9].GetDouble();
    mtx[2][2] = v[10].GetDouble();
    mtx[2][3] = v[11].GetDouble();

    mtx[3][0] = v[12].GetDouble();
    mtx[3][1] = v[13].GetDouble();
    mtx[3][2] = v[14].GetDouble();
    mtx[3][3] = v[15].GetDouble();

    return mtx;
}

RdQuaternion JsonComponent::getQuaternion(std::string key) {
    const auto& jvec = _value["members"][key.c_str()]["values"];
    RdQuaternion quat;

    quat.data()[0] = jvec[0].GetDouble();
    quat.data()[1] = jvec[1].GetDouble();
    quat.data()[2] = jvec[2].GetDouble();
    quat.data()[3] = jvec[3].GetDouble();

    return quat;
}

RdVector JsonComponent::getVector(std::string key) {
    const auto& jvec = _value["members"][key.c_str()]["values"];
    return {
        jvec[0].GetDouble(),
        jvec[1].GetDouble(),
        jvec[2].GetDouble()
    };
}

RdVectorF JsonComponent::getVectorF(std::string key) {
    const auto& jvec = _value["members"][key.c_str()]["values"];
    return {
        static_cast<float>(jvec[0].GetDouble()),
        static_cast<float>(jvec[1].GetDouble()),
        static_cast<float>(jvec[2].GetDouble())
    };
}

RdPoints JsonComponent::getPoints(std::string key) {
    const auto& jvec = _value["members"][key.c_str()]["values"];
    RdPoints points;

    for (auto i = 0U; i < jvec.Size(); i += 3) {
        RdPoint point{
            jvec[i + 0].GetDouble(),
            jvec[i + 1].GetDouble(),
            jvec[i + 2].GetDouble()
        };
        points.push_back(point);
    }

    return std::move(points);
}

RdUints JsonComponent::getUints(std::string key) {
    const auto& jvec = _value["members"][key.c_str()]["values"];
    RdUints uints;

    for (auto i = 0U; i < jvec.Size(); i++) {
        uints.push_back(jvec[i].GetUint());
    }

    return std::move(uints);
}

RdColor JsonComponent::getColor(std::string key) {
    const auto& jvec = _value["members"][key.c_str()]["values"];
    return {
        static_cast<float>(jvec[0].GetDouble()),
        static_cast<float>(jvec[1].GetDouble()),
        static_cast<float>(jvec[2].GetDouble())
    };
}

std::string JsonComponent::getEntity(std::string key) {
    const auto& jentity = _value["members"][key.c_str()]["value"];
    return std::to_string(jentity.GetUint());
}


RdString JsonComponent::getPath(RdString key) {
    return _value["members"][key.c_str()]["value"].GetString();
}


RdString JsonComponent::getData() {
    return _value["data"].GetString();
}
