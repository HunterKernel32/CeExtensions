#include "pch.h"
#include "CeExtensions.h"
#include "cepluginsdk.h"
#include "MainDialog.h"
#include "afxdialogex.h"
#include "MinHook.h"
#include <Psapi.h>
#include <TlHelp32.h>

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
	ON_COMMAND(ID_32771, &MainDialog::OnMenuButton1)
	ON_COMMAND(ID_32772, &MainDialog::OnMenuButton2)
	ON_COMMAND(ID_32773, &MainDialog::OnMenuButton3)
	ON_COMMAND(ID_32774, &MainDialog::OnMenuButton4)
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
	hMenu1 = LoadMenuA(AfxGetApp()->m_hInstance, MAKEINTRESOURCEA(IDR_MENU1));
	
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
}BEGIN_MESSAGE_MAP(CListBoxEx, CListBox)
ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


void CListBoxEx::OnRButtonUp(UINT nFlags, CPoint point)
{
	POINT pt = { 0 };
	GetCursorPos(&pt);
	HMENU subh = GetSubMenu(PMyDialog->hMenu1, 0);
	TrackPopupMenu(subh, TPM_CENTERALIGN, pt.x, pt.y, 0, PMyDialog->m_hWnd, NULL);

	CListBox::OnRButtonUp(nFlags, point);
}


//枚举模块
void MainDialog::OnMenuButton1()
{
	HANDLE hProcess = NULL;
	HMODULE ModuleList[100] = { 0 };
	CHAR ModulePath[MAX_PATH] = { 0 };

	if (GetProcessId(*CEP_Exported.OpenedProcessHandle))
	{
		hProcess = *CEP_Exported.OpenedProcessHandle;
		K32EnumProcessModulesEx(hProcess, ModuleList, sizeof(ModuleList), NULL, LIST_MODULES_ALL);
		for (int i = 0; ModuleList[i] != NULL; i++)
		{
			K32GetModuleFileNameExA(hProcess, ModuleList[i], ModulePath, sizeof(ModulePath));
			listbox1.InsertString(-1, ModulePath);
			PMyDialog->listbox1.SetCurSel(PMyDialog->listbox1.GetCount() - 1);
		}
	}
}


//枚举线程
void MainDialog::OnMenuButton2()
{
	static NTQUERYINFORMATIONTHREAD NtQueryInformationThread = (NTQUERYINFORMATIONTHREAD)
		(GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQueryInformationThread"));

	if (GetProcessId(*CEP_Exported.OpenedProcessHandle))
	{
		ULONG_PTR StartAddress = NULL;
		HMODULE ModuleList[100] = { 0 };
		MODULEINFO ModuleInfo = { 0 };
		CHAR ModuleName[MAX_PATH] = { 0 };
		CHAR OutPutBuffer[MAX_PATH] = { 0 };
		THREADENTRY32 ThreadEntry = { 0 };
		HANDLE hThread;
		BOOL Finded = FALSE;

		HANDLE hProcess = *CEP_Exported.OpenedProcessHandle;
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
		ThreadEntry.dwSize = sizeof(THREADENTRY32);

		Thread32First(hSnapshot, &ThreadEntry);
		do
		{
			if (ThreadEntry.th32OwnerProcessID == *CEP_Exported.OpenedProcessID)
			{
				hThread = OpenThread(PROCESS_ALL_ACCESS, FALSE, ThreadEntry.th32ThreadID);
				NtQueryInformationThread(hThread, ThreadQuerySetWin32StartAddress, &StartAddress, sizeof(PVOID), NULL);
				CloseHandle(hThread);

				K32EnumProcessModulesEx(hProcess, ModuleList, sizeof(ModuleList), NULL, LIST_MODULES_ALL);
				for (int i = 0; ModuleList[i] != NULL; i++)
				{
					K32GetModuleInformation(hProcess, ModuleList[i], &ModuleInfo, sizeof(ModuleInfo));

					if (StartAddress > (ULONG_PTR)ModuleInfo.lpBaseOfDll &&
						StartAddress < ((ULONG_PTR)ModuleList[i] + ModuleInfo.SizeOfImage))
					{
						K32GetModuleBaseNameA(hProcess, ModuleList[i], ModuleName, sizeof(ModuleName));

						sprintf_s(OutPutBuffer, "%06x:%s+%llx", ThreadEntry.th32ThreadID, 
							ModuleName, (StartAddress - (ULONG_PTR)ModuleList[i]));

						listbox1.InsertString(-1, OutPutBuffer);
						PMyDialog->listbox1.SetCurSel(PMyDialog->listbox1.GetCount() - 1);

						Finded = TRUE;
						break;
					}
				}
				if (Finded == FALSE)
				{
					sprintf_s(OutPutBuffer, "%06x:%llx", ThreadEntry.th32ThreadID, StartAddress);
					listbox1.InsertString(-1, OutPutBuffer);
					PMyDialog->listbox1.SetCurSel(PMyDialog->listbox1.GetCount() - 1);
				}
				Finded = FALSE;
			}

		} while (Thread32Next(hSnapshot, &ThreadEntry));

		CloseHandle(hSnapshot);
	}
}


//清屏
void MainDialog::OnMenuButton3()
{
	listbox1.ResetContent();
}


//复制
void MainDialog::OnMenuButton4()
{
	int index = listbox1.GetCurSel();
	if (index != LB_ERR)
	{
		CString string;
		listbox1.GetText(index, string);
		if (string.GetLength() > 0)
		{
			//打开剪切板
			OpenClipboard();  

			//清空剪切板
			EmptyClipboard(); 

			//申请堆内存
			size_t cbStr = (string.GetLength() + 1) * sizeof(TCHAR);
			HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE, cbStr);
			memcpy_s(GlobalLock(hData), cbStr, string.LockBuffer(), cbStr);
			GlobalUnlock(hData);
			string.UnlockBuffer();

			//设置剪切板内容及格式
			UINT uiFormat = (sizeof(TCHAR) == sizeof(WCHAR)) ? CF_UNICODETEXT : CF_TEXT;
			SetClipboardData(uiFormat, hData);

			//关闭剪切板并释放内存
			CloseClipboard();
			GlobalFree(hData);
		}
	}
	
}
