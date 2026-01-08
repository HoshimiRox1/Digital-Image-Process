// CHistogramDlg.cpp: 实现文件
//

#include "pch.h"
#include "zwj206.h"
#include "afxdialogex.h"
#include "CHistogramDlg.h"


// CHistogramDlg 对话框

IMPLEMENT_DYNAMIC(CHistogramDlg, CDialog)

CHistogramDlg::CHistogramDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG1, pParent)
{

}

CHistogramDlg::~CHistogramDlg()
{
}

void CHistogramDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CHistogramDlg, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CHistogramDlg 消息处理程序
extern DWORD H[256];
void Histogram();
BOOL CHistogramDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	Histogram();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CHistogramDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	dc.Rectangle(20, 20, 20 + 256 + 1, 20 + 200);
	
	// 灰度直方图绘制
	int i;
	int max = 0;
	for (i = 0; i < 256; i++) {
		if (H[i] > max) {
			max = H[i];
		}
	}

	for (i = 0; i < 256; i++) {
		dc.MoveTo(20 + i, 220);
		dc.LineTo(20 + i, 220 - (int)(H[i] * 200 / max));
	}
	// 不为绘图消息调用 CDialog::OnPaint()
}
