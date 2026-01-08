
// zwj206View.cpp: Czwj206View 类的实现
//

#include "pch.h"
#include "bmp.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "zwj206.h"   
#endif

#include "zwj206Doc.h"
#include "zwj206View.h"
#include "CHistogramDlg.h"
#include "CLineTransDlg.h"
#include "param.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Czwj206View

IMPLEMENT_DYNCREATE(Czwj206View, CScrollView)

BEGIN_MESSAGE_MAP(Czwj206View, CScrollView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &Czwj206View::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_TOGREY, &Czwj206View::OnTogrey)
	ON_UPDATE_COMMAND_UI(ID_TOGREY, &Czwj206View::OnUpdateTogrey)
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_HISTOGRAM, &Czwj206View::OnHistogram)
	ON_UPDATE_COMMAND_UI(ID_HISTOGRAM, &Czwj206View::OnUpdateHistogram)
	ON_COMMAND(ID_LINETRANS, &Czwj206View::OnLinetrans)
	ON_UPDATE_COMMAND_UI(ID_LINETRANS, &Czwj206View::OnUpdateLinetrans)
	ON_COMMAND(ID_EQUALIZE, &Czwj206View::OnEqualize)
	ON_UPDATE_COMMAND_UI(ID_EQUALIZE, &Czwj206View::OnUpdateEqualize)
	ON_COMMAND(ID_FT, &Czwj206View::OnFt)
	ON_UPDATE_COMMAND_UI(ID_FT, &Czwj206View::OnUpdateFt)
	ON_COMMAND(ID_IFT, &Czwj206View::OnIft)
	ON_UPDATE_COMMAND_UI(ID_IFT, &Czwj206View::OnUpdateIft)
	ON_COMMAND(ID_FFT, &Czwj206View::OnFft)
	ON_UPDATE_COMMAND_UI(ID_FFT, &Czwj206View::OnUpdateFft)
	ON_COMMAND(ID_IFFT, &Czwj206View::OnIfft)
	ON_UPDATE_COMMAND_UI(ID_IFFT, &Czwj206View::OnUpdateIfft)
	ON_COMMAND(ID_AVGSMOOTH, &Czwj206View::OnAvgsmooth)
	ON_UPDATE_COMMAND_UI(ID_AVGSMOOTH, &Czwj206View::OnUpdateAvgsmooth)
	ON_COMMAND(ID_MIDSMOOTH, &Czwj206View::OnMidsmooth)
	ON_UPDATE_COMMAND_UI(ID_MIDSMOOTH, &Czwj206View::OnUpdateMidsmooth)
	ON_COMMAND(ID_RAPLASSHARP, &Czwj206View::OnRaplassharp)
	ON_UPDATE_COMMAND_UI(ID_RAPLASSHARP, &Czwj206View::OnUpdateRaplassharp)
	ON_COMMAND(ID_GRADSHARP, &Czwj206View::OnGradsharp)
	ON_UPDATE_COMMAND_UI(ID_GRADSHARP, &Czwj206View::OnUpdateGradsharp)
	ON_COMMAND(ID_IDEALFILTER, &Czwj206View::OnIdealfilter)
	ON_UPDATE_COMMAND_UI(ID_IDEALFILTER, &Czwj206View::OnUpdateIdealfilter)
END_MESSAGE_MAP()

// Czwj206View 构造/析构

Czwj206View::Czwj206View() noexcept
{
	// TODO: 在此处添加构造代码

}

Czwj206View::~Czwj206View()
{
}

BOOL Czwj206View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CScrollView::PreCreateWindow(cs);
}

// Czwj206View 绘图
extern BITMAPINFO* lpBitsInfo;
extern BITMAPINFO* lpDIB_FT;
extern BITMAPINFO* lpDIB_IFT;

void Czwj206View::OnDraw(CDC* pDC)
{
    // 获取 Document 指针（保持不变）
    Czwj206Doc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    // 检查全局指针是否有效（保持不变）
    if (lpBitsInfo == nullptr)
        return;

    BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];

	StretchDIBits(pDC->GetSafeHdc(),
		0, 0, lpBitsInfo->bmiHeader.biWidth, lpBitsInfo->bmiHeader.biHeight,//图像的宽度高度，目标区域的矩形框
		0, 0, lpBitsInfo->bmiHeader.biWidth, lpBitsInfo->bmiHeader.biHeight,//原图的宽度和高度
		lpBits, lpBitsInfo,
		DIB_RGB_COLORS,
		SRCCOPY);

	if (lpDIB_FT) {
		lpBits = (BYTE*)&lpDIB_FT->bmiColors[lpDIB_FT->bmiHeader.biClrUsed];
		StretchDIBits(pDC->GetSafeHdc(),
			600, 0, lpDIB_FT->bmiHeader.biWidth, lpDIB_FT->bmiHeader.biHeight,
			0, 0, lpDIB_FT->bmiHeader.biWidth, lpDIB_FT->bmiHeader.biHeight,
			lpBits, lpDIB_FT,
			DIB_RGB_COLORS, SRCCOPY);
	}

	if (lpDIB_IFT) {
		lpBits = (BYTE*)&lpDIB_IFT->bmiColors[lpDIB_IFT->bmiHeader.biClrUsed];
		StretchDIBits(pDC->GetSafeHdc(),
			0, 600, lpDIB_IFT->bmiHeader.biWidth, lpDIB_IFT->bmiHeader.biHeight,
			0, 0, lpDIB_IFT->bmiHeader.biWidth, lpDIB_IFT->bmiHeader.biHeight,
			lpBits, lpDIB_IFT,
			DIB_RGB_COLORS, SRCCOPY);
	}
}

void Czwj206View::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: 计算此视图的合计大小
	sizeTotal.cx = sizeTotal.cy = 1200;
	SetScrollSizes(MM_TEXT, sizeTotal);
}


// Czwj206View 打印


void Czwj206View::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL Czwj206View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void Czwj206View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void Czwj206View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void Czwj206View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void Czwj206View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// Czwj206View 诊断

#ifdef _DEBUG
void Czwj206View::AssertValid() const
{
	CScrollView::AssertValid();
}

void Czwj206View::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

Czwj206Doc* Czwj206View::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Czwj206Doc)));
	return (Czwj206Doc*)m_pDocument;
}
#endif //_DEBUG


// Czwj206View 消息处理程序
void Gray();
void Czwj206View::OnTogrey()
{
	if (lpBitsInfo == nullptr)
		return;
	// TODO: 在此添加命令处理程序代码
	Gray();
	Invalidate();
}

void Czwj206View::OnUpdateTogrey(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(lpBitsInfo != nullptr && 24 == lpBitsInfo->bmiHeader.biBitCount);
}

void pixel(int i, int j, char*);
void Czwj206View::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// 同步滚动条的位置
	CPoint pt = GetScrollPosition();
	point.x += pt.x;
	point.y += pt.y;

	//
	char xy[100];
	memset(xy, 100, 100);
	sprintf(xy, "x:%d, y:%d    ", point.x, point.y);

	char rgb[100];
	memset(rgb, 0, 100);
	pixel(point.y, point.x, rgb);

	strcat(xy, rgb);
	((CFrameWnd*)GetParent())->SetMessageText(CString(xy));

	CScrollView::OnMouseMove(nFlags, point);
}

void Czwj206View::OnHistogram()
{
	// TODO: 在此添加命令处理程序代码
	CHistogramDlg dlg;
	dlg.DoModal();
}

void Czwj206View::OnUpdateHistogram(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(lpBitsInfo != nullptr && IsGray()); 
}

void Czwj206View::OnLinetrans()
{
	// TODO: 在此添加命令处理程序代码
	CLineTransDlg dlg;
	dlg.DoModal();
}

void Czwj206View::OnUpdateLinetrans(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(lpBitsInfo != nullptr && IsGray());
}

void Equalize();
void Czwj206View::OnEqualize()
{
	// TODO: 在此添加命令处理程序代码
	Equalize();
	Invalidate();
}

void Czwj206View::OnUpdateEqualize(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(lpBitsInfo != nullptr && IsGray());
}

void Fourier();
BOOL FD_Available();

void Czwj206View::OnFt()
{
	// TODO: 在此添加命令处理程序代码
	Fourier();
	Invalidate();
}

void Czwj206View::OnUpdateFt(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(lpBitsInfo != NULL && IsGray());
}

void IFourier();
void Czwj206View::OnIft()
{
	// TODO: 在此添加命令处理程序代码
	IFourier();
	Invalidate();
}

void Czwj206View::OnUpdateIft(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(lpBitsInfo != NULL && IsGray());
}

BOOL is_gFD_OK();
void FFourier();
void Czwj206View::OnFft()
{
	// TODO: 在此添加命令处理程序代码
	if (lpDIB_FT)
	{
		free(lpDIB_FT);
		lpDIB_FT = NULL;
	}
	if (lpDIB_IFT)
	{
		free(lpDIB_IFT);
		lpDIB_IFT = NULL;
	}

	FFourier();
	Invalidate();
}

void Czwj206View::OnUpdateFft(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(lpBitsInfo != NULL && IsGray());
}

void IFFourier();
void Czwj206View::OnIfft()
{
	// TODO: 在此添加命令处理程序代码
	if (lpDIB_IFT)
	{
		free(lpDIB_IFT);
		lpDIB_IFT = NULL;
	}
	IFFourier();
	Invalidate();
}

void Czwj206View::OnUpdateIfft(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(is_gFD_OK());
}

void AvgSmooth();
void Czwj206View::OnAvgsmooth()
{
	// TODO: 在此添加命令处理程序代码
	AvgSmooth();
	Invalidate();
}

void Czwj206View::OnUpdateAvgsmooth(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(lpBitsInfo != NULL && IsGray());
}

void MidSmooth();
void Czwj206View::OnMidsmooth()
{
	// TODO: 在此添加命令处理程序代码
	MidSmooth();
	Invalidate();
}

void Czwj206View::OnUpdateMidsmooth(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(lpBitsInfo != NULL && IsGray());
}

void RaplasSharp();
void Czwj206View::OnRaplassharp()
{
	// TODO: 在此添加命令处理程序代码
	RaplasSharp();
	Invalidate();
}

void Czwj206View::OnUpdateRaplassharp(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(lpBitsInfo != NULL && IsGray());
}

void GradSharp();
void Czwj206View::OnGradsharp()
{
	// TODO: 在此添加命令处理程序代码
	GradSharp();
	Invalidate();
}

void Czwj206View::OnUpdateGradsharp(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(lpBitsInfo != NULL && IsGray());
}

void FFT_Filter(int D);
void Czwj206View::OnIdealfilter()
{
	// TODO: 在此添加命令处理程序代码
	Param dlg;
	if (IDOK == dlg.DoModal())
		FFT_Filter(dlg.m_nDis); //建议使用对话框输入截止半径
	Invalidate();
}

void Czwj206View::OnUpdateIdealfilter(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(lpBitsInfo != NULL && IsGray());
}
