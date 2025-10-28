
// zwj206Doc.cpp: Czwj206Doc 类的实现
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

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Czwj206Doc

IMPLEMENT_DYNCREATE(Czwj206Doc, CDocument)

BEGIN_MESSAGE_MAP(Czwj206Doc, CDocument)
END_MESSAGE_MAP()


// Czwj206Doc 构造/析构

Czwj206Doc::Czwj206Doc() noexcept
{
	// TODO: 在此添加一次性构造代码

}

Czwj206Doc::~Czwj206Doc()
{
}

BOOL Czwj206Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// Czwj206Doc 序列化

void Czwj206Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void Czwj206Doc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void Czwj206Doc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void Czwj206Doc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl* pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// Czwj206Doc 诊断

#ifdef _DEBUG
void Czwj206Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void Czwj206Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// Czwj206Doc 命令

BOOL Czwj206Doc::OnOpenDocument(LPCTSTR lpszPathName)
{
	// 1. 释放上次加载的内存（C 风格编程必须手动释放）
	if (lpBitsInfo != nullptr)
	{
		free(lpBitsInfo);
		lpBitsInfo = nullptr;
	}

	// 2. 调用老师的 C 风格加载函数
	// 注意：MFC 使用 Unicode 字符串 (LPCTSTR)，但 fopen 需要 char*
	// 需要进行类型转换，这里使用一个简单的宏或函数：
	USES_CONVERSION;

	// 如果你的项目是 ANSI/多字节字符集，可以直接用 (const char*)lpszPathName
	// 如果是 Unicode，则需要转换：
	if (!LoadBmpFile(T2A(lpszPathName)))
	{
		// 加载失败，提示用户
		AfxMessageBox(_T("加载 BMP 文件失败！"));
		return FALSE;
	}

	// 3. 成功加载后，lpBitsInfo 现在指向了图像数据。
	return TRUE;
}
