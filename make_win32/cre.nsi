!include "MUI2.nsh"

!define CRE  "Crossfire Resource Editor"
!ifndef CREVERSION
!define /date CREVERSION "%Y-%m-%d"
!endif
!ifndef OUTPUTFILE
!define OUTPUTFILE "CREInstaller-${CREVERSION}.exe"
!endif

; Application title
Name "${CRE}"

; Various settings for the .exe
VIAddVersionKey "ProductName" "${CRE} ${CREVERSION} installer"
VIAddVersionKey "Comments" "Website: http://crossfire.real-time.com"
VIAddVersionKey "FileDescription" "${CRE} ${CREVERSION} installer"
VIAddVersionKey "FileVersion" "${CREVERSION}"
VIAddVersionKey "ProductVersion" "${CREVERSION}"
VIAddVersionKey "LegalCopyright" "Crossfire is released under the GPL."
VIProductVersion "1.0.0.0"

!define MUI_ICON "cre.ico"
!define MUI_UNICON "cre.ico"

!define MUI_WELCOMEPAGE_TITLE $(I18N_WELCOME)

CRCCheck On
SetCompressor /SOLID lzma

OutFile "${OUTPUTFILE}"

LicenseText "You must agree to this license before installing."

; Find default installation directory if a version is already installed
InstallDir "$PROGRAMFILES\Crossfire Server"
InstallDirRegKey HKLM "Software\${CRE}" "InstallLocation"

!define MUI_ABORTWARNING

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\COPYING"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH


!define MUI_LANGDLL_ALWAYSSHOW
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "French"

Function .onInit

  !insertmacro MUI_LANGDLL_DISPLAY

FunctionEnd

Section "${CRE}" jx
  SectionIn RO

  ; Remove any installed version
  ReadRegStr $0 HKLM "Software\${CRE}" "InstallLocation"
  StrCmp $0 "" no_current
  ReadRegStr $1 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${CRE}" "UninstallString"
  StrCmp $1 "" no_current
        ; Uninstall needs to be run in silent mode, and without copying/forking itself (so ExecWait works)
        DetailPrint $(I18N_UNINSTALL_CURRENT)
        ExecWait '$1 /S _?=$0'
        ; This means we need to clean up ourselves after it
        Delete '$1'
        RmDir '$0'

no_current:

  SetOutPath "$INSTDIR"
  SetCompress Auto
  SetOverwrite IfNewer

  ; Install main files
  File "..\utils\cre\release\cre.exe"
  File "..\utils\cre\cre.qch"
  File "..\utils\cre\cre.qhc"
  File "cre.ico"

  ; Translations
  CreateDirectory "$INSTDIR\l10n"
  SetOutPath "$INSTDIR\l10n"
  File "..\utils\cre\l10n\*.qm"

  ; Required directories
  CreateDirectory "$INSTDIR\share"
  CreateDirectory "$INSTDIR\share\i18n"
  CreateDirectory "$INSTDIR\etc"
  CreateDirectory "$INSTDIR\var"

  ; Required files
  SetOutPath "$INSTDIR\share\i18n"
  File /r "..\lib\i18n\*"
  SetOutPath "$INSTDIR\etc"
  File "..\lib\config\exp_table"
  File "..\lib\config\settings"
  File "..\lib\config\stat_bonus"

  ; Write uninstaller
  WriteUninstaller "Uninst.exe"

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${CRE}" "DisplayName" $(I18N_UNINSTALL_NAME)
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${CRE}" "UninstallString" "$INSTDIR\Uninst.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${CRE}" "DisplayIcon" "$INSTDIR\jxclient.ico"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${CRE}" "Publisher" "The Crossfire Team"

  ; Write our install location so another installer can find it
  WriteRegStr HKLM "Software\${CRE}" "InstallLocation" "$INSTDIR"

SectionEnd

Section "$(I18N_SHORTCUTS)" menus
  ; Add Shortcuts in the Start menu
  CreateDirectory "$SMPROGRAMS\${CRE}"
  CreateShortCut "$SMPROGRAMS\${CRE}\${CRE}.lnk" "$INSTDIR\cre.exe" "" "$INSTDIR\cre.ico" 0
  CreateShortCut "$SMPROGRAMS\${CRE}\$(I18N_UNINSTALL).lnk" "$INSTDIR\uninst.exe" "" "$INSTDIR\uninst.exe" 0
SectionEnd

Section $(I18N_DESKTOP) desktop
  ; Add Shortcuts on the common desktop
  SetShellVarContext all
  CreateShortCut "$desktop\${CRE}.lnk" "$INSTDIR\cre.exe" "" "$INSTDIR\cre.ico" 0
SectionEnd

UninstallText $(I18N_UNINSTALL_TEXT)

Section "un.${CRE}" un_jx
  SectionIn RO

  ; Delete main files
  Delete "$INSTDIR\cre.exe"
  Delete "$INSTDIR\cre.qch"
  Delete "$INSTDIR\cre.qhc"
  Delete "$INSTDIR\cre.ico"

  ; Delete translations
  RmDir /r "$INSTDIR\l10n"

  ; Delete server files
  RmDir /r "$INSTDIR\share\i18n"
  Delete "$INSTDIR\etc\exp_table"
  Delete "$INSTDIR\etc\settings"
  Delete "$INSTDIR\etc\stat_bonus"

  ; Delete required directories, if empty
  RmDir "$INSTDIR\share"
  RmDir "$INSTDIR\etc"
  RmDir "$INSTDIR\var"

  ; Delete Start Menu shortcuts
  RmDir /r "$SMPROGRAMS\${CRE}"

  ; Delete desktop shortcut
  SetShellVarContext all
  Delete "$desktop\${CRE}.lnk"

  ; Delete uninstaller and uninstall registry entries
  Delete "$INSTDIR\Uninst.exe"
  DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\${CRE}"
  DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${CRE}"

  ; Finish cleaning the install directory
  rmdir "$INSTDIR"

SectionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${jx} $(I18N_DESC_CRE)
  !insertmacro MUI_DESCRIPTION_TEXT ${menus} $(I18N_DESC_SHORTCUTS)
  !insertmacro MUI_DESCRIPTION_TEXT ${desktop} $(I18N_DESC_DESKTOP)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

!insertmacro MUI_UNFUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${un_jx} $(I18N_UNINSTALL)
!insertmacro MUI_UNFUNCTION_DESCRIPTION_END

LangString I18N_WELCOME ${LANG_ENGLISH} "Welcome to the ${CRE} (version ${CREVERSION}) Setup Wizard"
LangString I18N_SHORTCUTS ${LANG_ENGLISH} "Start Menu Shortcuts"
LangString I18N_DESKTOP $(LANG_ENGLISH) "Desktop Shortcut"
LangString I18N_UNINSTALL $(LANG_ENGLISH) "Uninstall ${CRE}"
LangString I18N_UNINSTALL_TEXT $(LANG_ENGLISH) "This will uninstall ${CRE} from your system"
LangString I18N_DESC_CRE $(LANG_ENGLISH) "${CRE} (required)."
LangString I18N_DESC_SHORTCUTS $(LANG_ENGLISH) "Create shortcuts in Start Menu."
LangString I18N_DESC_DESKTOP $(LANG_ENGLISH) "Create shortcut on the Desktop."
LangString I18N_UNINSTALL_CURRENT $(LANG_ENGLISH) "Uninstalling current version..."
LangString I18N_UNINSTALL_NAME $(LANG_ENGLISH) "${CRE} (remove only)"

LangString I18N_WELCOME ${LANG_FRENCH} "Bienvenue dans l'installeur de ${CRE} (version ${CREVERSION})"
LangString I18N_SHORTCUTS ${LANG_FRENCH} "Raccourcis du menu Démarrer"
LangString I18N_DESKTOP $(LANG_FRENCH) "Raccourci sur le bureau"
LangString I18N_UNINSTALL $(LANG_FRENCH) "Désinstaller ${CRE}"
LangString I18N_UNINSTALL_TEXT $(LANG_FRENCH) "Ceci désinstallera ${CRE} de votre système"
LangString I18N_DESC_CRE $(LANG_FRENCH) "${CRE} (requis)."
LangString I18N_DESC_SHORTCUTS $(LANG_FRENCH) "Créer des raccourcis dans le menu Démarrer"
LangString I18N_DESC_DESKTOP $(LANG_FRENCH) "Créer un raccourci sur le bureau"
LangString I18N_UNINSTALL_CURRENT $(LANG_FRENCH) "Désinstallation de la version présente..."
LangString I18N_UNINSTALL_NAME $(LANG_FRENCH) "${CRE} (désinstallation uniquement)"
