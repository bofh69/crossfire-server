#include <QtWidgets>

#include "RandomMapPanel.h"
#include "RandomMap.h"
#include "CREMapInformation.h"

CRERandomMapPanel::CRERandomMapPanel(QWidget* parent) : AssetSWrapperPanel(parent)
{
    myLayout->addWidget(new QLabel(tr("Source map:"), this), 0, 0);
    myLayout->addWidget(mySource = new QLabel(this), 0, 1);
    myLayout->addWidget(new QLabel(tr("Parameters:"), this), 1, 0, 1, 2);
    myLayout->addWidget(myInformation = new QTextEdit(this), 2, 0, 1, 2);
    myInformation->setReadOnly(true);
}

void CRERandomMapPanel::updateItem()
{
    StringBuffer* sb = write_map_parameters_to_string(myItem->parameters());
    char* text = stringbuffer_finish(sb);
    myInformation->setText(text);
    free(text);

    mySource->setText(tr("%1 [%2, %3]").arg(myItem->map()->displayName()).arg(myItem->x()).arg(myItem->y()));
}
