/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2021-2022 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#include "LicenseManager.h"

#include "global.h"
#include "bufferreader.h"
#include <memory>

LicenseManager::LicenseManager() {
}

LicenseManager::~LicenseManager() {
}

void LicenseManager::readLicense(BufferReader *reader, const char *filename) {
    char *c;
    std::unique_ptr<char, void(*)(void*)> dup(strdup(filename), free);

    c = strrchr(dup.get(), '.');
    if (!c) {
        LOG(llevError, "LicenseManager: invalid filename %s\n", filename);
        return;
    }
    (*c) = '\0';
    c = strrchr(dup.get(), '.');
    if (!c) {
        LOG(llevError, "LicenseManager: invalid filename %s\n", filename);
        return;
    }
    (*c) = '\0';
    c++;

    std::string faceset(c);

    c = strrchr(dup.get(), '/');
    if (!c) {
        LOG(llevError, "LicenseManager: invalid filename %s\n", filename);
        return;
    }
    c++;

    auto &licenses = m_licenses[c];
    auto &items = licenses[faceset];
    parseLicenseFile(reader, filename, items);
}

void LicenseManager::parseLicenseFile(BufferReader *reader, const char *filename, std::vector<LicenseItem> &items) {
    char *line, *c;
    while ((line = bufferreader_next_line(reader)) != nullptr) {
        if (line[0] == '\0')
            continue;
        c = strchr(line, ' ');
        if (!c) {
            LOG(llevError, "LicenseManager: malformed line '%s' in %s:%zu\n", line, filename, bufferreader_current_line(reader));
            continue;
        }

        (*c) = '\0';
        c++;

        items.push_back(std::make_pair(line, c));
    }
}

LicenseManager::LicenseItems LicenseManager::getForFace(const std::string &face) {
    for (auto wall : { false, true }) {
        auto search = m_licenses.find(licenseNameFromFaceName(face, wall));
        if (search != m_licenses.end()) {
            return search->second;
        }
    }
    return LicenseItems();
}

std::string LicenseManager::licenseNameFromFaceName(const std::string &face, bool tryWall) {
    auto dot = face.find('.');
    if (dot == std::string::npos) {
        return std::string();
    }
    auto result = face.substr(0, dot);
    if (tryWall && result.length() > 2 && result[result.length() - 2] == '_') {
        auto letter = result[result.length() - 1];
        if ((letter >= '0' && letter <= '9') || (letter >= 'A' && letter <= 'F')) {
            return result.substr(0, result.length() - 2);
        }
    }
    return result;
}
