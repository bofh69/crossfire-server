/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2022 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#include "SoundFiles.h"
#include "../CRESettings.h"
#include "SoundFile.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "global.h"

SoundFiles::SoundFiles(AssetWrapper *parent) : AssetWrapper(parent, "SoundFiles") {
    setProperty(tipProperty, tr("Display all sound files."));
    refreshSounds();
}

void SoundFiles::refreshSounds() {
    CRESettings settings;
    QString path(settings.soundsDirectory());
    if (path.isEmpty()) {
        return;
    }
    QDir dir(path);
    if (!dir.exists()) {
        return;
    }

    markModified(BeforeLayoutChange);

    for (auto sound : mySounds) {
        delete sound;
    }
    mySounds.clear();

    std::map<std::string, std::string> soundConf;
    QFile file(dir.filePath("sounds.conf"));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.isEmpty() || line.startsWith('#')) {
                continue;
            }
            auto split = line.split(':');
            if (split.length() != 3) {
                continue;
            }
            soundConf[split[0].toStdString()] = split[2].toStdString();
        }
    } else {
        qDebug() << tr("Unable to open sound configuration file %1/sounds.conf").arg(path);
    }

    auto files = dir.entryList(QDir::Files, QDir::SortFlag::IgnoreCase | QDir::Name);
    for (auto file : files) {
        if (file.endsWith(".ogg") || file.endsWith(".wav")) {
            std::vector<std::string> events;
            for (const auto &sc : soundConf) {
                if (sc.second == file.toStdString()) {
                    events.push_back(sc.first);
                }
            }
            mySounds.push_back(new SoundFile(events, this, file));
            continue;
        }
        if (file.endsWith(".LICENSE")) {
            auto name = file.left(file.length() - 8);
            auto it = std::find_if(mySounds.cbegin(), mySounds.cend(), [&name] (const auto &sound) { return sound->displayName() == name; });
            if (it != mySounds.cend()) {
                auto br = bufferreader_init_from_file(nullptr, dir.filePath(file).toStdString().data(), "Failed to open LICENSE file %s: %s", llevError);
                if (br) {
                    LicenseManager::parseLicenseFile(br, "", (*it)->license());
                    bufferreader_destroy(br);
                }
            }
            continue;
        }
    }

    markModified(AfterLayoutChange);
}

AssetWrapper *SoundFiles::child(int index) {
    return mySounds[index];
}

int SoundFiles::childIndex(AssetWrapper *child) {
    for (size_t i = 0; i < mySounds.size(); ++i) {
        if (mySounds[i] == child) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

QString SoundFiles::licenseInformation() const {
    if (mySounds.empty()) {
        return tr("No sound file found");
    }
    size_t with = 0;
    for (const auto &sound : mySounds) {
        if (!sound->license().empty()) {
            ++with;
        }
    }
    size_t percent = 100 * with / mySounds.size();
    if (percent == 100 && with < mySounds.size()) {
        percent = 99;
    }
    return tr("%1 sound files with license information on %2 (%3%)").arg(with).arg(mySounds.size()).arg(percent);
}
