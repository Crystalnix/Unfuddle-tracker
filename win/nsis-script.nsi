!include "MUI.nsh"
SetCompressor lzma

!define PRODUCT_NAME "Unfuddle Tracker"
!define PRODUCT_EXE "utracker.exe"
!define MUI_ABORTWWARNING
!define MUI_UNINSTALLER
!define MUI_UNCONFIRMPAGE
!define MUI_FINISHPAGE
!define MUI_FINISHPAGE_RUN "$INSTDIR\${PRODUCT_EXE}"
!define MUI_ICON "icon.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

!macro CheckAppRunning
  StrCpy $0 "${PRODUCT_EXE}"
  KillProc::FindProcesses
  StrCmp $1 "-1" wooops
  Goto AppRunning
 
  AppRunning:
    StrCmp $0 "0" AppNotRunning
    MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "${PRODUCT_NAME} allready opened. Close it?" IDYES KillApp
    Abort
  KillApp:
    StrCpy $0 "${PRODUCT_EXE}"
    KillProc::KillProcesses
    StrCmp $1 "-1" wooops
    sleep 1500
    Goto AppNotRunning
  wooops:
    Abort
  AppNotRunning:
!macroend

Name "${PRODUCT_NAME}"
Caption "Installing ${PRODUCT_NAME}"
InstallDir "$PROGRAMFILES\${PRODUCT_NAME}"
OutFile "${PRODUCT_NAME}.exe"

Section
  SetOutPath "$INSTDIR"
  File "${PRODUCT_EXE_PATH}\${PRODUCT_EXE}"
  File "$%QTDIR%\bin\mingwm10.dll"
  File "$%QTDIR%\bin\libgcc_s_dw2-1.dll"
  File "$%QTDIR%\bin\libeay32.dll"
  File "$%QTDIR%\bin\ssleay32.dll"
  File "$%QTDIR%\lib\QtCore4.dll"
  File "$%QTDIR%\lib\QtGui4.dll"
  File "$%QTDIR%\lib\QtNetwork4.dll"
  File "$%QTDIR%\lib\QtXml4.dll"
  SetOutPath "$INSTDIR\imageformats"
  File "$%QTDIR%\plugins\imageformats\qjpeg4.dll"
  File "$%QTDIR%\plugins\imageformats\qico4.dll"
  
  SetShellVarContext all
  CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_EXE}"
  CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_EXE}"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "DisplayName" "${PRODUCT_NAME} (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "UninstallString" "$INSTDIR\Uninstall.exe"
  
  WriteUninstaller "$INSTDIR\Uninstall.exe"
SectionEnd

Function .onInit
    !insertmacro CheckAppRunning
FunctionEnd

Function un.onInit
    !insertmacro CheckAppRunning
FunctionEnd

Section "Uninstall"
  SetShellVarContext all
  RMDir /r "$INSTDIR"
  RMDir /r "$SMPROGRAMS\${PRODUCT_NAME}"
  Delete "$DESKTOP\${PRODUCT_NAME}.lnk"
  
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
SectionEnd
