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

#ifndef CRE_REPORT_DEFINITION_H
#define CRE_REPORT_DEFINITION_H

#include <QObject>
#include <QMetaType>

class CREReportDefinition : public QObject
{
    Q_OBJECT

    public:
        CREReportDefinition();
        CREReportDefinition(const CREReportDefinition& other);
        virtual ~CREReportDefinition();

        const QString& name() const;
        void setName(const QString& name);

        const QString& header() const;
        void setHeader(const QString& header);

        const QString& itemSort() const;
        void setItemSort(const QString& sort);

        const QString& itemDisplay() const;
        void setItemDisplay(const QString& display);

        const QString& footer() const;
        void setFooter(const QString& footer);

    protected:
        QString myName;
        QString myHeader;
        QString myItemSort;
        QString myItemDisplay;
        QString myFooter;
};

Q_DECLARE_METATYPE(CREReportDefinition);

QDataStream &operator<<(QDataStream &out, const CREReportDefinition &report);
QDataStream &operator>>(QDataStream &in, CREReportDefinition &report);

#endif // CRE_REPORT_DEFINITION_H
