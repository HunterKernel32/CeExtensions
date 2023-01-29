#include "pch.h"
#include "CeExtensions.h"
#include "MainDialog.h"
#include "afxdialogex.h"
#include "MinHook.h"

IMPLEMENT_DYNAMIC(MainDialog, CDialogEx)

MainDialog::MainDialog(CWnd* pParent)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

MainDialog::~MainDialog()
{
}

void MainDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, listbox1);
}


BEGIN_MESSAGE_MAP(MainDialog, CDialogEx)
END_MESSAGE_MAP()


//------------------------------------------------------

void InitializeHook();

BOOL MainDialog::DestroyWindow()
{
	
	MH_Uninitialize();
	return CDialogEx::DestroyWindow();
}


BOOL MainDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	PMyDialog->m_hWnd = this->m_hWnd; 
	PMyDialog->SetWindowPos(NULL, 0, 0, 800, 600, SWP_NOZORDER | SWP_NOMOVE);
	InitializeHook();

	return TRUE;  
}



//------------------------------------------Hook------------------------------------------
PVOID Old_OutputDebugStringA = NULL;
typedef VOID(WINAPI OUTPUTDEBUGSTRINGA)(_In_opt_ LPCSTR lpOutputString);
VOID WINAPI MyOutputDebugStringA(_In_opt_ LPCSTR lpOutputString)
{

	PMyDialog->listbox1.InsertString(-1, lpOutputString);
	int count = PMyDialog->listbox1.GetCount();
	PMyDialog->listbox1.SetCurSel(count - 1);
}


void InitializeHook()
{
	MH_Initialize();
	MH_CreateHookApi(
		L"KernelBase.dll", 
		"OutputDebugStringA", 
		MyOutputDebugStringA, 
		&Old_OutputDebugStringA);
	MH_EnableHook(MH_ALL_HOOKS);
}