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

#ifndef CLASS_CRE_SETTINGS_H
#define CLASS_CRE_SETTINGS_H

#include <QSettings>

#include "CREFilterDefinitionManager.h"
#include "CREReportDefinitionManager.h"

class CRESettings : protected QSettings
{
    public:
        CRESettings();

        bool ensureOptions();

        QString mapCacheDirectory() const;

        void loadFilters(CREFilterDefinitionManager& filters);
        void saveFilters(const CREFilterDefinitionManager& filters);

        void loadReports(CREReportDefinitionManager& reports);
        void saveReports(const CREReportDefinitionManager& reports);

        QString facesetToDisplay() const;
        void setFacesetToDisplay(const QString& faceset);
        bool facesetUseFallback() const;
        void setFacesetUseFallback(bool use);

        bool showChanges() const;
        void setShowChanges(bool show);

        int changesLength() const;
        void setChangesLength(int length);

        bool storeWindowsState() const;
        void setStoreWindowState(bool store);

        QByteArray mainWindowGeometry();
        void setMainWindowGeometry(const QByteArray &data);

        int subWindowCount() const;
        void setSubWindowCount(int count);

        int subWindowType(int subWindow) const;
        void setSubWindowType(int subWindow, int type);

        QByteArray subWindowPosition(int subWindow) const;
        void setSubWindowPosition(int subWindow, const QByteArray &data);

        QString soundsDirectory() const;
        void setSoundsDirectory(const QString &dir);
};

#endif // CLASS_CRE_SETTINGS_H
