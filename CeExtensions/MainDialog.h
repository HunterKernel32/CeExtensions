#pragma once


// MainDialog 对话框

class CListBoxEx : public CListBox
{

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};




class MainDialog : public CDialogEx
{
	DECLARE_DYNAMIC(MainDialog)

public:
	MainDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~MainDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	
	CListBoxEx listbox1;
	HMENU hMenu1;
	virtual BOOL DestroyWindow();
	virtual BOOL OnInitDialog();
	afx_msg void OnMenuButton1();
	afx_msg void OnMenuButton2();
	afx_msg void OnMenuButton3();
	afx_msg void OnMenuButton4();
};

typedef enum _THREADINFOCLASS {
	ThreadBasicInformation = 0,
	ThreadTimes = 1,
	ThreadPriority = 2,
	ThreadBasePriority = 3,
	ThreadAffinityMask = 4,
	ThreadImpersonationToken = 5,
	ThreadDescriptorTableEntry = 6,
	ThreadEnableAlignmentFaultFixup = 7,
	ThreadEventPair_Reusable = 8,
	ThreadQuerySetWin32StartAddress = 9,
	ThreadZeroTlsCell = 10,
	ThreadPerformanceCount = 11,
	ThreadAmILastThread = 12,
	ThreadIdealProcessor = 13,
	ThreadPriorityBoost = 14,
	ThreadSetTlsArrayAddress = 15,   // Obsolete
	ThreadIsIoPending = 16,
	ThreadHideFromDebugger = 17,
	ThreadBreakOnTermination = 18,
	ThreadSwitchLegacyState = 19,
	ThreadIsTerminated = 20,
	ThreadLastSystemCall = 21,
	ThreadIoPriority = 22,
	ThreadCycleTime = 23,
	ThreadPagePriority = 24,
	ThreadActualBasePriority = 25,
	ThreadTebInformation = 26,
	ThreadCSwitchMon = 27,   // Obsolete
	ThreadCSwitchPmu = 28,
	ThreadWow64Context = 29,
	ThreadGroupInformation = 30,
	ThreadUmsInformation = 31,   // UMS
	ThreadCounterProfiling = 32,
	ThreadIdealProcessorEx = 33,
	ThreadCpuAccountingInformation = 34,
	ThreadSuspendCount = 35,
	ThreadActualGroupAffinity = 41,
	ThreadDynamicCodePolicyInfo = 42,
	ThreadSubsystemInformation = 45,

	MaxThreadInfoClass = 51,
} THREADINFOCLASS;

typedef NTSTATUS(NTAPI *NTQUERYINFORMATIONTHREAD)(
	_In_ HANDLE ThreadHandle,
	_In_ THREADINFOCLASS ThreadInformationClass,
	_Out_writes_bytes_(ThreadInformationLength) PVOID ThreadInformation,
	_In_ ULONG ThreadInformationLength,
	_Out_opt_ PULONG ReturnLength);

extern ExportedFunctions CEP_Exported;
extern MainDialog* PMyDialog;