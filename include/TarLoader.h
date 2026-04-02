#ifndef TARLOADER_H
#define TARLOADER_H

#include "AssetLoader.h"
#include "Utils.h"


class TarLoader : public AssetLoader {
public:
    TarLoader(AssetLoader *loader);
    virtual ~TarLoader();

    virtual bool willLoad(const std::string &filename) override {
        return Utils::endsWith(filename.c_str(), ".tar");
    };

    virtual void load(BufferReader *reader, const std::string &filename) override;

private:
    BufferReader *m_reader;
    AssetLoader *m_loader;
};

#endif /* TARLOADER_H */

