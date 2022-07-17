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

#ifndef ASSET_ORIGIN_AND_CREATION_DIALOG_H
#define ASSET_ORIGIN_AND_CREATION_DIALOG_H

#include <QtWidgets>

/**
 * Helper dialog to input origin of an asset, and maybe create a new one.
 */
class AssetOriginAndCreationDialog : public QDialog {
public:
    enum Type { Quest = 0, Treasure = 1 };
    enum Mode { CreateAsset, DefineOrigin };

    /**
     * Standard constructor.
     * @param type asset type, to adjust messages.
     * @param mode dialog mode to operate in.
     * @param code asset code, used in DefineOrigin mode only.
     * @param origins list of existing files for the asset.
     * @param assets existing assets.
     */
    AssetOriginAndCreationDialog(Type type, Mode mode, const QString &code, const std::vector<std::string> &origins, const std::vector<std::string> &assets);

    QString code() const { return myCode->text(); }
    QString file() const { return myNewFile->isEnabled() ? myNewFile->text() : myExistingFile->currentText(); }

private:
    Type myType;                /**< Asset type. */
    QString myRootDirectory;    /**< Base path for this asset type. */
    QLineEdit *myCode;          /**< Quest code. */
    QComboBox *myExistingFile;  /**< Selected current file. */
    QLineEdit *myNewFile;       /**< New file path. */
    std::vector<std::string> myAssets;  /**< Existing assets, to avoid duplication. */

    /**
     * Check if data is complete. If yes then close the dialog, else explain to the
     * user what is wrong.
     */
    void validate();
};

#endif /* ASSET_ORIGIN_AND_CREATION_DIALOG_H */
