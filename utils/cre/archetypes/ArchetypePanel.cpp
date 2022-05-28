#include <Qt>
#include "CREMapInformation.h"
#include "CREMapInformationManager.h"

extern "C" {
#include "global.h"
}

#include "ArchetypePanel.h"
#include "CREUtils.h"
#include "ResourcesManager.h"

#include "assets.h"
#include "AssetsManager.h"
#include "Archetypes.h"
#include "ArchetypeLoader.h"
#include "ResourcesManager.h"
#include "../assets/AssetModel.h"

ArchetypePanel::ArchetypePanel(CREMapInformationManager* store, ResourcesManager* resources, AssetModel *model, QWidget* parent) : AssetTWrapperPanel(parent)
{
    Q_ASSERT(store);
    myStore = store;
    myResources = resources;

    myDisplay = addTextEdit(QString(), nullptr, false);
    addAssetUseTree(tr("Used by"), model, "self");
}

void ArchetypePanel::updateItem()
{
    StringBuffer* dump = stringbuffer_new();
    object_dump(&myItem->clone, dump);
    char* final = stringbuffer_finish(dump);
    myInitialArch = final;
    free(final);

    // Hacks: replace initial "arch" by "Object", and remove the "more 0" line which messes things
    myInitialArch.replace(0, 4, "Object");
    auto more = myInitialArch.find("more 0\n");
    if (more != std::string::npos) {
        myInitialArch.erase(more, 7);
    }
    myDisplay->setText(myInitialArch.data());
}

void ArchetypePanel::commitData() {
    auto text = myDisplay->toPlainText().toStdString();
    if (text == myInitialArch) {
        return;
    }

    BufferReader *br = bufferreader_init_from_memory(nullptr, text.data(), text.length());

    auto origin = myResources->originOf(myItem);
    ArchetypeLoader loader(getManager()->archetypes(), nullptr);
    loader.load(br, origin);
    bufferreader_destroy(br);

    myResources->archetypeModified(myItem);
}
