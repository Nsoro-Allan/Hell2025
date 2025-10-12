#pragma once
#include "HellTypes.h"
#include "CreateInfo.h"

struct House {
    void SetFilename(const std::string& filename);
    void SetCreateInfoCollection(CreateInfoCollection& createInfoCollection);

    CreateInfoCollection& GetCreateInfoCollection()     { return m_createInfoCollection; }
    const std::string& GetFilename() const              { return m_filename; }

private:
    CreateInfoCollection m_createInfoCollection;
    std::string m_filename = UNDEFINED_STRING;
};