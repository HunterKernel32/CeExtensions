#include "pch.h"
#include "framework.h"
#include "cepluginsdk.h"
#include "MainDialog.h"
#include "CeExtensions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



BEGIN_MESSAGE_MAP(CCeExtensionsApp, CWinApp)
END_MESSAGE_MAP()



CCeExtensionsApp::CCeExtensionsApp()
{
	
}


CCeExtensionsApp theApp;


BOOL CCeExtensionsApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}


MainDialog* PMyDialog = NULL;
DWORD WINAPI ShowDialog(LPARAM lpData)
{
	PMyDialog = new MainDialog;
	PMyDialog->DoModal();

	delete PMyDialog;

	return TRUE;
}



BOOL __stdcall CEPlugin_GetVersion(PPluginVersion pv, int sizeofpluginversion)
{
	pv->version = CESDK_VERSION;
	pv->pluginname = "v1.0.2";
	return TRUE;
}


ExportedFunctions CEP_Exported;
BOOL __stdcall CEPlugin_InitializePlugin(PExportedFunctions ef, int pluginid)
{
	static int selfid = pluginid;
	CEP_Exported = *ef;

	if (CEP_Exported.sizeofExportedFunctions != sizeof(ExportedFunctions))
	{
		OutputDebugStringA("Exported.sizeofExportedFunctions != sizeof(ExportedFunctions)");
		return FALSE;
	}

	::CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ShowDialog, NULL, 0, NULL);

	return TRUE;
}


BOOL __stdcall CEPlugin_DisablePlugin(void)
{
	if (PMyDialog->m_hWnd)
	{
		PMyDialog->EndDialog(IDOK);
	}
	return TRUE;
}