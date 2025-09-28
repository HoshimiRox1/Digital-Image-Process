﻿
// zwj206View.h: Czwj206View 类的接口
//

#pragma once


class Czwj206View : public CScrollView
{
protected: // 仅从序列化创建
	Czwj206View() noexcept;
	DECLARE_DYNCREATE(Czwj206View)

// 特性
public:
	Czwj206Doc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~Czwj206View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTogrey();
};

#ifndef _DEBUG  // zwj206View.cpp 中的调试版本
inline Czwj206Doc* Czwj206View::GetDocument() const
   { return reinterpret_cast<Czwj206Doc*>(m_pDocument); }
#endif

