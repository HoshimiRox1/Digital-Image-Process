// CLineTransDlg.cpp: 实现文件
//

#include "pch.h"
#include "zwj206.h"
#include "bmp.h"
#include "afxdialogex.h"
#include "CLineTransDlg.h"


// CLineTransDlg 对话框

IMPLEMENT_DYNAMIC(CLineTransDlg, CDialog)

CLineTransDlg::CLineTransDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_LineTransDlg, pParent)
{
	m_param_a = 0.0f;
	m_param_b = 0.0f;
}

CLineTransDlg::~CLineTransDlg()
{
}

void CLineTransDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_A, m_param_a);
	DDX_Text(pDX, IDC_EDIT_B, m_param_b);
}


BEGIN_MESSAGE_MAP(CLineTransDlg, CDialog)
END_MESSAGE_MAP()


// CLineTransDlg 消息处理程序
void LineTrans(float a, float b);
void CLineTransDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	UpdateData();
	LineTrans(m_param_a, m_param_b);
	GetParent()->Invalidate();
	CDialog::OnOK();
}

BOOL CLineTransDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}