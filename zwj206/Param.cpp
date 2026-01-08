// Param.cpp: 实现文件
//

#include "pch.h"
#include "zwj206.h"
#include "bmp.h"
#include "afxdialogex.h"
#include "Param.h"


// Param 对话框

IMPLEMENT_DYNAMIC(Param, CDialog)

Param::Param(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_Param, pParent)
{
	m_nDis = 0;
}

Param::~Param()
{
}

void Param::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_nDis);
	DDV_MinMaxInt(pDX, m_nDis, -360, 360);
}


BEGIN_MESSAGE_MAP(Param, CDialog)
END_MESSAGE_MAP()


// Param 消息处理程序
