lessThan(QT_MAJOR_VERSION, 5): error("This program requires Qt5. Make sure you use the 'qmake' command from version 5 and not 4, and/or try 'qmake -qt=5'.")

TEMPLATE = app

INCLUDEPATH += ../../include

SOURCES += cre.cpp \
CREMainWindow.cpp \
CREPixmap.cpp \
 CREUtils.cpp \
 CREResourcesWindow.cpp \
 CREMapInformation.cpp \
 CREMapInformationManager.cpp \
 CRESettings.cpp \
 CRESettingsDialog.cpp \
 CREFilterDialog.cpp \
 CREReportDialog.cpp \
 CREReportDisplay.cpp \
 CREFilterDefinition.cpp \
 CREFilterDefinitionManager.cpp \
 CREReportDefinition.cpp \
 CREMapPanel.cpp \
 CREExperienceWindow.cpp \
 CREReportDefinitionManager.cpp \
 CREScriptEngine.cpp \
 CREQuestPanel.cpp \
 MessageManager.cpp \
 CREMessagePanel.cpp \
 MessageFile.cpp \
 CREStringListPanel.cpp \
 CREPrePostPanel.cpp \
 CREReplyPanel.cpp \
 CREQuestItemModel.cpp \
 CREMultilineItemDelegate.cpp \
 QuestConditionScript.cpp \
 CREHPBarMaker.cpp \
 CRESmoothFaceMaker.cpp \
 ResourcesManager.cpp \
 CRECombatSimulator.cpp \
 random_maps/RandomMap.cpp \
 random_maps/RandomMapPanel.cpp \
 FaceMakerDialog.cpp \
 EditMonstersDialog.cpp \
 ArchetypesModel.cpp \
 CREMessageItemModel.cpp \
 CREPrePostConditionDelegate.cpp \
 CREPrePostList.cpp \
 CREPlayerRepliesPanel.cpp \
 CREPlayerRepliesDelegate.cpp \
 CREStringListDelegate.cpp \
 FaceComboBox.cpp \
 LicenseManager.cpp \
 animations/AnimationWrapper.cpp \
 animations/AnimationControl.cpp \
 animations/AnimationWidget.cpp \
 animations/AnimationPanel.cpp \
 archetypes/ArchetypeComboBox.cpp \
 archetypes/ArchetypePanel.cpp \
 archetypes/ArchetypeWrapper.cpp \
 archetypes/ArchetypesWrapper.cpp \
 archetypes/ObjectWrapper.cpp \
 artifacts/ArtifactListPanel.cpp \
 artifacts/ArtifactListWrapper.cpp \
 artifacts/ArtifactPanel.cpp \
 artifacts/ArtifactWrapper.cpp \
 artifacts/ArtifactsWrapper.cpp \
 assets/AssetModel.cpp \
 assets/AssetUseTree.cpp \
 assets/AssetWrapperPanel.cpp \
 attack_messages/AttackMessagePanel.cpp \
 attack_messages/AttackMessagesWrapper.cpp \
 faces/FacePanel.cpp \
 faces/FaceWrapper.cpp \
 faces/FacesetsPanel.cpp \
 general_messages/GeneralMessagePanel.cpp \
 general_messages/GeneralMessageWrapper.cpp \
 quests/QuestWrapper.cpp \
 recipes/RecipeListWrapper.cpp \
 recipes/RecipePanel.cpp \
 recipes/RecipeWrapper.cpp \
 recipes/RecipesWrapper.cpp \
 regions/RegionPanel.cpp \
 regions/RegionWrapper.cpp \
 regions/RegionsWrapper.cpp \
 scripts/ScriptFile.cpp \
 scripts/ScriptFileManager.cpp \
 scripts/ScriptFilePanel.cpp \
 treasures/TreasureListComboBox.cpp \
 treasures/TreasureListPanel.cpp \
 treasures/TreasureListWrapper.cpp \
 treasures/TreasurePanel.cpp \
 treasures/TreasureWrapper.cpp \
 AllAssets.cpp \
 ChangesDock.cpp \
 HelpBrowser.cpp \
 HelpManager.cpp \
 MimeUtils.cpp \
 MonsterResistances.cpp
HEADERS += CREMainWindow.h \
CREPixmap.h \
 CREUtils.h \
 CREResourcesWindow.h \
 CREMapInformation.h \
 CREMapInformationManager.h \
 CRESettings.h \
 CRESettingsDialog.h \
 CREFilterDialog.h \
 CREReportDialog.h \
 CREReportDisplay.h \
 CREFilterDefinition.h \
 CREFilterDefinitionManager.h \
 CREReportDefinition.h \
 CREMapPanel.h \
 CREExperienceWindow.h \
 CREReportDefinitionManager.h \
 CREScriptEngine.h \
 CREQuestPanel.h \
 MessageManager.h \
 CREMessagePanel.h \
 MessageFile.h \
 CREStringListPanel.h \
 CREPrePostPanel.h \
 CREReplyPanel.h \
 CREQuestItemModel.h \
 CREMultilineItemDelegate.h \
 QuestConditionScript.h \
 CREHPBarMaker.h \
 CRESmoothFaceMaker.h \
 ResourcesManager.h \
 CRECombatSimulator.h \
 random_maps/RandomMap.h \
 random_maps/RandomMapPanel.h \
 random_maps/RandomMaps.h \
 FaceMakerDialog.h \
 EditMonstersDialog.h \
 ArchetypesModel.h \
 CREMessageItemModel.h \
 CREPrePostConditionDelegate.h \
 CREPrePostList.h \
 CREPlayerRepliesPanel.h \
 CREPlayerRepliesDelegate.h \
 CREStringListDelegate.h \
 FaceComboBox.h \
 LicenseManager.h \
 animations/AnimationWrapper.h \
 animations/AnimationControl.h \
 animations/AnimationWidget.h \
 animations/AnimationPanel.h \
 animations/AnimationsWrapper.h \
 archetypes/ArchetypeComboBox.h \
 archetypes/ArchetypePanel.h \
 archetypes/ArchetypeWrapper.h \
 archetypes/ArchetypesWrapper.h \
 archetypes/ObjectWrapper.h \
 artifacts/ArtifactListPanel.h \
 artifacts/ArtifactListWrapper.h \
 artifacts/ArtifactPanel.h \
 artifacts/ArtifactWrapper.h \
 artifacts/ArtifactsWrapper.h \
 assets/AssetModel.h \
 assets/AssetUseTree.h \
 assets/AssetWrapper.h \
 assets/AssetWrapperPanel.h \
 attack_messages/AttackMessagePanel.h \
 attack_messages/AttackMessagesWrapper.h \
 faces/FacePanel.h \
 faces/FaceWrapper.h \
 faces/FacesWrapper.h \
 faces/FacesetsPanel.h \
 faces/FacesetsWrapper.h \
 general_messages/GeneralMessagePanel.h \
 general_messages/GeneralMessageWrapper.h \
 general_messages/GeneralMessagesWrapper.h \
 quests/QuestWrapper.h \
 quests/QuestsWrapper.h \
 recipes/RecipeListWrapper.h \
 recipes/RecipePanel.h \
 recipes/RecipeWrapper.h \
 recipes/RecipesWrapper.h \
 regions/RegionPanel.cpp \
 regions/RegionWrapper.h \
 regions/RegionsWrapper.h \
 scripts/ScriptFile.cpp \
 scripts/ScriptFileManager.h \
 scripts/ScriptFilePanel.h \
 treasures/TreasureListComboBox.h \
 treasures/TreasureListPanel.h \
 treasures/TreasureListWrapper.h \
 treasures/TreasurePanel.h \
 treasures/TreasureWrapper.h \
 treasures/TreasuresWrapper.h \
 AllAssets.h \
 ChangesDock.h \
 HelpBrowser.h \
 HelpManager.h \
 MonsterResistances.h

LIBS += ../../server/libserver.a ../../common/libcross.a ../../socket/libsocket.a ../../random_maps/librandom_map.a ../../types/libtypes.a -lcurl
QMAKE_CXXFLAGS += -Wcast-qual
# -Wold-style-cast

linux-*|unix {
LIBS += -lcrypt -ldl
CONFIG += precompile_header
PRECOMPILED_DIR = .pch
PRECOMPILED_HEADER = cre_pch.h
CONFIG += debug

CONFIG(debug, debug|release) {
    BDIR = build/debug
}
CONFIG(release, debug|release) {
    BDIR = build/release
}

OBJECTS_DIR = $$BDIR/obj
MOC_DIR = $$BDIR/moc
RCC_DIR = $$BDIR/qrc
UI_DIR = $$BDIR/u

}

win* {
CONFIG += release
}

RESOURCES += cre.qrc

CONFIG += thread
CONFIG += c++14

QT += concurrent script widgets help
