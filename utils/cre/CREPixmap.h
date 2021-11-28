#ifndef CREPIXMAP_H
#define CREPIXMAP_H

#include <QtWidgets>
extern "C" {
#include "global.h"
#include "image.h"
}

class CREPixmap
{
    public:
        static void init();

        static face_sets *faceset;
        static void setFaceset(const QString& prefix);
        static void setUseFacesetFallback(bool use);
        static void clearFaceCache();

        static QIcon getIcon(uint16_t faceNumber);
        static QIcon getIcon(const Face *face);

        static QIcon getTreasureIcon();
        static QIcon getTreasureOneIcon();
        static QIcon getTreasureYesIcon();
        static QIcon getTreasureNoIcon();

    protected:
        static QIcon getIcon(const face_sets* faceset, uint16_t faceNumber);

        static QHash<uint16_t, QIcon> allFaces;
        static QIcon* myTreasureIcon;
        static QIcon* myTreasureOneIcon;
        static QIcon* myTreasureYesIcon;
        static QIcon* myTreasureNoIcon;
        static bool myUseFaceFallback;
};

#endif // CREPIXMAP_H
