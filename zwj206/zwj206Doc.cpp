
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
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// CT2A 是一个 MFC 宏，用于将宽字符路径（LPCTSTR）转换为窄字符（char*）
	CT2A strPath(lpszPathName);
	const char* filePath = strPath;

	Errorstate result = LoadBmpFile(filePath, infoheader, pixeldata);

	switch (result) {
	case Errorstate::success:
		UpdateAllViews(NULL);
		return TRUE;
	case Errorstate::file_error:
		AfxMessageBox(_T("文件打开失败！"), MB_OK | MB_ICONERROR);
		break;
	case Errorstate::type_error:
		AfxMessageBox(_T("文件类型错误！"), MB_OK | MB_ICONERROR);
		break;
	case Errorstate::bit_error:
		AfxMessageBox(_T("文件位数错误！"), MB_OK | MB_ICONERROR);
		break;
	}

	// 调用你的独立函数来加载文件
	//if (LoadBmpFile(filePath, infoheader, pixeldata))
	//{
	//	// 如果加载成功，通知所有视图更新
	//	UpdateAllViews(NULL);
	//	return TRUE;
	//}
	//else
	//{
	//	// 加载失败，弹出错误提示
	//	AfxMessageBox(_T("Failed to load BMP file. Please ensure it is a valid 24-bit BMP."), MB_OK | MB_ICONERROR);
	//	return FALSE;
	//}

	return FALSE;
}
