#pragma once
#include "House.h"

void House::SetCreateInfoCollection(CreateInfoCollection& createInfoCollection) {
    m_createInfoCollection = createInfoCollection;
}

void House::SetFilename(const std::string& filename) {
    m_filename = filename;
}