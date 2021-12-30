!include "MUI.nsh"

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

!define MUI_WELCOMEPAGE_TITLE "Welcome to the ${CRE} (version ${CREVERSION}) Setup Wizard"

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


!insertmacro MUI_LANGUAGE "English"

Section "${CRE}" jx
  SectionIn RO

  ; Remove any installed version
  ReadRegStr $0 HKLM "Software\${CRE}" "InstallLocation"
  StrCmp $0 "" no_current
  ReadRegStr $1 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${CRE}" "UninstallString"
  StrCmp $1 "" no_current
        ; Uninstall needs to be run in silent mode, and without copying/forking itself (so ExecWait works)
        DetailPrint "Uninstalling current version..."
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

  ; Required directories
  CreateDirectory "share"
  CreateDirectory "share\i18n"
  CreateDirectory "etc"
  CreateDirectory "var"

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
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${CRE}" "DisplayName" "${CRE} (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${CRE}" "UninstallString" "$INSTDIR\Uninst.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${CRE}" "DisplayIcon" "$INSTDIR\jxclient.ico"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${CRE}" "Publisher" "The Crossfire Team"

  ; Write our install location so another installer can find it
  WriteRegStr HKLM "Software\${CRE}" "InstallLocation" "$INSTDIR"

SectionEnd

Section "Start Menu Shortcuts" menus
  ; Add Shortcuts in the Start menu
  CreateDirectory "$SMPROGRAMS\${CRE}"
  CreateShortCut "$SMPROGRAMS\${CRE}\${CRE}.lnk" "$INSTDIR\cre.exe" "" "$INSTDIR\cre.ico" 0
  CreateShortCut "$SMPROGRAMS\${CRE}\Uninstall ${CRE}.lnk" "$INSTDIR\uninst.exe" "" "$INSTDIR\uninst.exe" 0
SectionEnd

Section "Desktop Shortcut" desktop
  ; Add Shortcuts on the common desktop
  SetShellVarContext all
  CreateShortCut "$desktop\${CRE}.lnk" "$INSTDIR\cre.exe" "" "$INSTDIR\cre.ico" 0
SectionEnd

UninstallText "This will uninstall ${CRE} from your system"

Section "un.${CRE}" un_jx
  SectionIn RO

  ; Delete main files
  Delete "$INSTDIR\cre.exe"
  Delete "$INSTDIR\cre.qch"
  Delete "$INSTDIR\cre.qhc"
  Delete "$INSTDIR\cre.ico"

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

  ; Delete uninstaller and unistall registry entries
  Delete "$INSTDIR\Uninst.exe"
  DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\${CRE}"
  DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${CRE}"

  ; Finish cleaning the install directory
  rmdir "$INSTDIR"

SectionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${jx} "${CRE} (required)."
  !insertmacro MUI_DESCRIPTION_TEXT ${menus} "Create shortcuts in Start Menu."
  !insertmacro MUI_DESCRIPTION_TEXT ${desktop} "Create shortcut on the Desktop."
!insertmacro MUI_FUNCTION_DESCRIPTION_END

!insertmacro MUI_UNFUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${un_jx} "Remove ${CRE}."
!insertmacro MUI_UNFUNCTION_DESCRIPTION_END
