// CefOverlayBrowser.exe — CEF subprocess helper for CefOverlay.
//
// Every renderer / GPU / utility subprocess CEF spawns is launched as a copy
// of THIS executable (via CefSettings.browser_subprocess_path) with --type=...
// arguments. Keeping it a separate tiny exe means Task Manager shows only our
// own names (CefOverlay.exe + CefOverlayBrowser.exe) — no chrome/msedge
// branding, which WebView2 could not do.
#include <Windows.h>
#include <aclapi.h>
#include "include/cef_app.h"
#include "include/cef_command_line.h"

#pragma comment(lib, "advapi32.lib")

static void HardenProcessSecurity() {
  HANDLE hProcess = GetCurrentProcess();
  PSID pEveryoneSid = nullptr;
  SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
  if (!AllocateAndInitializeSid(&SIDAuthWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &pEveryoneSid)) return;

  EXPLICIT_ACCESS_W ea = {};
  ea.grfAccessPermissions = PROCESS_TERMINATE | PROCESS_SUSPEND_RESUME;
  ea.grfAccessMode = DENY_ACCESS;
  ea.grfInheritance = NO_INHERITANCE;
  ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
  ea.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
  ea.Trustee.ptstrName = (LPWSTR)pEveryoneSid;

  PACL pOldAcl = nullptr;
  PSECURITY_DESCRIPTOR pSD = nullptr;
  if (GetSecurityInfo(hProcess, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION,
                      nullptr, nullptr, &pOldAcl, nullptr, &pSD) == ERROR_SUCCESS) {
    PACL pNewAcl = nullptr;
    if (SetEntriesInAclW(1, &ea, pOldAcl, &pNewAcl) == ERROR_SUCCESS) {
      SetSecurityInfo(hProcess, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION,
                      nullptr, nullptr, pNewAcl, nullptr);
      LocalFree(pNewAcl);
    }
    LocalFree(pSD);
  }
  FreeSid(pEveryoneSid);
}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int) {
  HardenProcessSecurity();
  CefMainArgs main_args(hInst);
  CefRefPtr<CefApp> app;   // no handler overrides needed in the helper
  int exit_code = CefExecuteProcess(main_args, app.get(), nullptr);
  if (exit_code >= 0) return exit_code;
  return 0;
}
