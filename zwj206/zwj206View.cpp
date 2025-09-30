
// zwj206View.cpp: Czwj206View 类的实现
//

#include "pch.h"
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
	Czwj206Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	// 存储信息头和调色板数据
	std::vector<BYTE> bmpInfoBuffer;

	bmpInfoBuffer.resize(sizeof(BITMAPINFOHEADER));
	memcpy(bmpInfoBuffer.data(), &(pDoc->infoheader), sizeof(BITMAPINFOHEADER));

	if (!pDoc->colorPalette.empty()) {
		size_t offset = bmpInfoBuffer.size();

		size_t paletteSize = pDoc->colorPalette.size() * sizeof(RGBQUAD);
		bmpInfoBuffer.resize(offset + paletteSize);

		memcpy(bmpInfoBuffer.data() + offset,
			pDoc->colorPalette.data(),
			paletteSize);
	}

	BITMAPINFO* pbmi = reinterpret_cast<BITMAPINFO*>(bmpInfoBuffer.data());

	int width = pDoc->infoheader.biWidth;
	int height = pDoc->infoheader.biHeight;

	if (!pDoc->pixeldata.empty())
	{
		StretchDIBits(
			pDC->GetSafeHdc(),
			0, 0,
			width,
			abs(height),
			0, 0,
			width,
			abs(height),
			pDoc->pixeldata.data(),
			pbmi,
			DIB_RGB_COLORS,
			SRCCOPY
		);
	}
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

void Czwj206View::OnTogrey()
{
	// TODO: 在此添加命令处理程序代码
	Czwj206Doc* pDoc = GetDocument();

	// 确保已加载24位BMP图像
	if (!pDoc || pDoc->pixeldata.empty() || pDoc->infoheader.biBitCount != 24) {
		AfxMessageBox(_T("请先加载24位真彩图像。"), MB_OK | MB_ICONINFORMATION);
		return;
	}

	// 准备循环变量
	std::vector<BYTE>& pixeldata = pDoc->pixeldata;
	int width = pDoc->infoheader.biWidth;
	int height = pDoc->infoheader.biHeight;

	// 向上取整到4的倍数
	int pitch = (width * 3 + 3) & (~3);

	// 遍历像素矩阵
	for (int y = 0; y < height; y++) {
		BYTE* pRow = pixeldata.data() + y * pitch;

		for (int x = 0; x < width; x++) {
			BYTE b = pRow[x * 3];
			BYTE g = pRow[x * 3 + 1];
			BYTE r = pRow[x * 3 + 2];

			// 灰度转换公式——说是亮度加权平均法
			// Gray = 0.299*R + 0.587*G + 0.114*B
			BYTE gray = static_cast<BYTE>(0.299 * r + 0.587 * g + 0.114 * b);

			// 灰度图中R=G=B=Gray
			pRow[x * 3] = gray;
			pRow[x * 3 + 1] = gray;
			pRow[x * 3 + 2] = gray;
		}
	}

	pDoc->UpdateAllViews(NULL);
}
