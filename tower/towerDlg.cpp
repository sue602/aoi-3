
// towerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "tower.h"
#include "towerDlg.h"
#include "afxdialogex.h"

#include <io.h>    
#include <fcntl.h>  
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CtowerDlg �Ի���



CtowerDlg::CtowerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CtowerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtowerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CtowerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CtowerDlg ��Ϣ��������

BOOL CtowerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵������ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ����Ӷ���ĳ�ʼ������

	AllocConsole();
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long)handle, _O_TEXT);
	FILE * hf = _fdopen(hCrt, "w");
	*stdout = *hf;

	m_aoi_ctx = create_aoi(1024 * 10, 1000, 1000, 2);
	m_countor = 1;

	GetWindowRect(&m_rt);

	for (int i = 0; i < 500; i++)
	{
		CPoint pt(rand() % m_rt.right, rand() % m_rt.bottom);
		CreateEntity(pt);
	}

	for (int i = 0; i < 10; i++)
	{
		TriggerCtx* ctx = new TriggerCtx();
		ctx->pos = CPoint(rand() % m_rt.right, rand() % m_rt.bottom);
		ctx->dest = CPoint(rand() % m_rt.right, rand() % m_rt.bottom);
		ctx->id = CreateTrigger(ctx->pos, rand() % 10 + 10);
		m_trigger_list.push_back(ctx);
	}


	SetTimer(1, 50, NULL);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CtowerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի���������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void foreach_entity_callback(int uid, int x, int z, void* ud) {
	CtowerDlg* pDlg = (CtowerDlg*)ud;

	CClientDC dc(pDlg);

	std::map<int, bool>::iterator it = pDlg->m_status.find(uid);
	if (it != pDlg->m_status.end())
	{
		bool val = it->second;

		CBrush brush0(RGB(0, 255, 0));
		CBrush brush1(RGB(255, 0, 0));

		if (val)
			dc.SelectObject(&brush0);
		else
			dc.SelectObject(&brush1);

		dc.Ellipse(x - 5, z - 5, x + 5, z + 5);
	}
	else{
		CBrush brush0(RGB(255, 0, 0));
		dc.SelectObject(&brush0);
		dc.Ellipse(x - 5, z - 5, x + 5, z + 5);
	}
}

void foreach_trigger_callback(int uid, int x, int z, int range, void* ud) {
	CClientDC* dc = (CClientDC*)ud;
	dc->Ellipse(x - 5, z - 5, x + 5, z + 5);

	dc->MoveTo(x - range * 2, z - range * 2);
	dc->LineTo(x + range * 2, z - range * 2);

	dc->MoveTo(x - range * 2, z + range * 2);
	dc->LineTo(x + range * 2, z + range * 2);

	dc->MoveTo(x - range * 2, z - range * 2);
	dc->LineTo(x - range * 2, z + range * 2);

	dc->MoveTo(x + range * 2, z - range * 2);
	dc->LineTo(x + range * 2, z + range * 2);
}

void CtowerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}

	CClientDC dc(this);

	CBrush brush0(RGB(255, 0, 0));
	dc.SelectObject(&brush0);

	foreach_aoi_entity(m_aoi_ctx, foreach_entity_callback, this);

	CBrush brush1(RGB(0, 0, 0));
	dc.SelectObject(&brush1);
	foreach_aoi_trigger(m_aoi_ctx, foreach_trigger_callback, &dc);
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CtowerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void OnEntityCallback(int self, int other, int op, void* ud) {
	if (op == 1)
	{
		printf("entity:%d enter:%d\n", self, other);
	}
	else {
		printf("entity:%d leave:%d\n", self, other);
	}

}

void OnTriggerCallback(int self, int other, int op, void* ud) {
	CtowerDlg* pDlg = (CtowerDlg*)ud;
	if (op == 1)
	{
		printf("trigger:%d enter:%d\n", self, other);
		pDlg->m_status[other] = true;
	}
	else {
		printf("trigger:%d leave:%d\n", self, other);
		pDlg->m_status[other] = false;
	}
}

int CtowerDlg::CreateEntity(CPoint& point)
{
	int uid = m_countor++;
	int id = create_entity(m_aoi_ctx, uid, point.x, point.y, OnEntityCallback, (void*)this);
	m_entity_list.push_back(id);
	return id;
}

int CtowerDlg::CreateTrigger(CPoint& point, int range)
{
	int uid = m_countor++;
	int id = create_trigger(m_aoi_ctx, uid, point.x, point.y, range, OnTriggerCallback, (void*)this);
	return id;
}


void CtowerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ�������Ϣ������������/�����Ĭ��ֵ

	CDialogEx::OnTimer(nIDEvent);
	UpdateTrigger();
}

void CtowerDlg::UpdateTrigger()
{
	for (int i = 0; i < m_trigger_list.size(); i++)
	{
		TriggerCtx* ctx = m_trigger_list[i];
		float dt = sqrt((ctx->dest.x - ctx->pos.x) * (ctx->dest.x - ctx->pos.x) + (ctx->dest.y - ctx->pos.y) * (ctx->dest.y - ctx->pos.y));
		if (dt <= 5)
		{
			ctx->dest = CPoint(rand() % m_rt.right, rand() % m_rt.bottom);
		}
		else {
			float vt = 50;
			float ratio = (vt * 0.1f) / dt;
			ctx->pos.x = ctx->pos.x + (ctx->dest.x - ctx->pos.x) * ratio;
			ctx->pos.y = ctx->pos.y + (ctx->dest.y - ctx->pos.y) * ratio;

			move_trigger(m_aoi_ctx, ctx->id, ctx->pos.x, ctx->pos.y);

			Invalidate();
		}
	}
}