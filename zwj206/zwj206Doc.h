#include <Windows.h>
#include <vector>

// zwj206Doc.h: Czwj206Doc 类的接口
//


#pragma once


class Czwj206Doc : public CDocument
{
protected: // 仅从序列化创建
	Czwj206Doc() noexcept;
	DECLARE_DYNCREATE(Czwj206Doc)

	// 特性
public:
	// 存储信息头和像素数据
	BITMAPINFOHEADER infoheader;
	std::vector<BYTE> pixeldata;

	// 操作
public:

	// 重写
public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
	// 声明打开新文件
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~Czwj206Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

	// 打开文档函数
public:

};
