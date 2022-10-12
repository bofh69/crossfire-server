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

#include "CRESettings.h"
#include "CRESettingsDialog.h"

#include <QFileInfo>
#include <QDir>

CRESettings::CRESettings()
{
}

bool CRESettings::ensureOptions()
{
    bool need = false;
    if (mapCacheDirectory().isEmpty())
        need = true;
    else
    {
        QFileInfo info(mapCacheDirectory());
        if (!info.exists())
        {
            QDir dir(mapCacheDirectory());
            dir.mkpath(".");
            info.refresh();
        }

        if (!info.exists())
            need = true;
    }

    if (need)
    {
        CRESettingsDialog dlg(this);
        if (dlg.exec() == QDialog::Rejected)
            return false;

        setValue("mapCacheDirectory", dlg.mapCache());
        return ensureOptions();
    }

    return true;
}

QString CRESettings::mapCacheDirectory() const
{
    return value("mapCacheDirectory").toString();
}

void CRESettings::loadFilters(CREFilterDefinitionManager& filters)
{
    filters.copy(value("filters").value<CREFilterDefinitionManager>());
}

void CRESettings::saveFilters(const CREFilterDefinitionManager& filters)
{
    QVariant val;
    val.setValue(filters);
    setValue("filters", val);
}

void CRESettings::loadReports(CREReportDefinitionManager& reports)
{
    reports.copy(value("reports").value<CREReportDefinitionManager>());
}

void CRESettings::saveReports(const CREReportDefinitionManager& reports)
{
    QVariant val;
    val.setValue(reports);
    setValue("reports", val);
}

QString CRESettings::facesetToDisplay() const
{
    return value("facesetToDisplay", "base").toString();
}

void CRESettings::setFacesetToDisplay(const QString& faceset)
{
    setValue("facesetToDisplay", faceset);
}

bool CRESettings::facesetUseFallback() const
{
    return value("facesetUseFallback").toBool();
}

void CRESettings::setFacesetUseFallback(bool use)
{
    setValue("facesetUseFallback", use);
}

bool CRESettings::showChanges() const {
    return value("showChanges", true).toBool();
}

void CRESettings::setShowChanges(bool show) {
    setValue("showChanges", show);
}

int CRESettings::changesLength() const {
    return value("changesLength").toInt();
}

void CRESettings::setChangesLength(int length) {
    setValue("changesLength", length);
}

bool CRESettings::storeWindowsState() const {
    return value("storeWindowsState").toBool();
}

void CRESettings::setStoreWindowState(bool store) {
    setValue("storeWindowsState", store);
}

QByteArray CRESettings::mainWindowGeometry() {
    return value("mainWindowGeometry").toByteArray();
}

void CRESettings::setMainWindowGeometry(const QByteArray &data) {
    setValue("mainWindowGeometry", data);
    sync();
}

int CRESettings::subWindowCount() const {
    return value("subWindows/count").toInt();
}

void CRESettings::setSubWindowCount(int count) {
    remove("subWindows");
    setValue("subWindows/count", count);
}

int CRESettings::subWindowType(int subWindow) const {
    return value(QString("subWindows/win_%1/type").arg(subWindow)).toInt();
}

void CRESettings::setSubWindowType(int subWindow, int type) {
    setValue(QString("subWindows/win_%1/type").arg(subWindow), type);
}

QByteArray CRESettings::subWindowPosition(int subWindow) const {
    return value(QString("subWindows/win_%1/position").arg(subWindow)).toByteArray();
}

void CRESettings::setSubWindowPosition(int subWindow, const QByteArray &data) {
    setValue(QString("subWindows/win_%1/position").arg(subWindow), data);
}
