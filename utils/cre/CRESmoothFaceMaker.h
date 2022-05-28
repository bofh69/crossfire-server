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

#ifndef CRESMOOTHFACEMAKER_H
#define CRESMOOTHFACEMAKER_H

#include <QDialog>
#include <QObject>

class QLineEdit;
class FaceComboBox;
class Face;

class CRESmoothFaceMaker : public QDialog
{
    Q_OBJECT

    public:
        CRESmoothFaceMaker();
        virtual ~CRESmoothFaceMaker();

        void setSelectedFace(const Face* face);
        QString destination() const;

        void setAutoClose(bool autoClose = true);

    protected slots:
        void makeSmooth();
        void browse(bool);
        void destinationEdited(const QString&);

    private:
        bool myAutoClose;
        bool myOverwrite;
        QLineEdit* myDestination;
        FaceComboBox* myFace;
};

#endif /* CRESMOOTHFACEMAKER_H */
