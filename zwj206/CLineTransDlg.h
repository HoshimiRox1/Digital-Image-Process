#pragma once
#include "afxdialogex.h"


// CLineTransDlg 对话框

class CLineTransDlg : public CDialog
{
	DECLARE_DYNAMIC(CLineTransDlg)

public:
	CLineTransDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CLineTransDlg();

	double m_param_a;
	double m_param_b;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LineTransDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
