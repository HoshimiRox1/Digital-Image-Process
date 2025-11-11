
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

void Czwj206View::OnDraw(CDC* pDC)
{
    // 获取 Document 指针（保持不变）
    Czwj206Doc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    // 检查全局指针是否有效（保持不变）
    if (lpBitsInfo == nullptr)
        return;

    // 1. 获取图像的原始尺寸
    int w = lpBitsInfo->bmiHeader.biWidth;
    int h = lpBitsInfo->bmiHeader.biHeight;
    // 确保使用绝对高度，因为 BMP 图像头可能存储负值
    int absH = abs(h);

    // 2. 获取窗口尺寸和计算居中位置
    CRect rectClient;
    GetClientRect(&rectClient);

    // 3. 获取像素数据的起始地址
    // 保持与 Gray() 函数中的计算方式一致
    BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];

    // 4. 使用 StretchDIBits 绘制
    StretchDIBits(
        pDC->GetSafeHdc(),     // 设备句柄
        0,					   // 左上角x
        0,					   // 左上角y
        w,                     // **目标宽度：设置为图像原始宽度**
        absH,                  // **目标高度：设置为图像原始高度**
        0,                     // 源 X 坐标 (从图像左侧开始)
        0,                     // 源 Y 坐标 (从图像底部开始)
        w,                     // 源宽度 (图像原始宽度)
        absH,                  // 源高度 (图像原始高度)
        lpBits,                // 像素数据
        lpBitsInfo,            // BITMAPINFO 结构体
        DIB_RGB_COLORS,        // 颜色使用 RGB 模式
        SRCCOPY                // 复制源图像
    );
}

void Czwj206View::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: 计算此视图的合计大小
	sizeTotal.cx = sizeTotal.cy = 100;
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
