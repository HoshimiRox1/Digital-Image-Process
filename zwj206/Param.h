#pragma once
#include "afxdialogex.h"


// Param 对话框

class Param : public CDialog
{
	DECLARE_DYNAMIC(Param)

public:
	Param(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~Param();
	
	int m_nDis;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_Param };
#endif
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
