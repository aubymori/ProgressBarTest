#define _WIN32_WINNT 0x0502

#include <windows.h>
#include <uxtheme.h>
#include <commctrl.h>
#include "resource.h"

#define PBM_SETSTATE (WM_USER+16)
#define PBS_SMOOTHREVERSE 0x10

#define PBST_NORMAL 1
#define PBST_ERROR 2
#define PBST_PAUSED 3

void SetWindowStyle(HWND hwnd, DWORD dwStyle, BOOL fSet)
{
	DWORD dwStyleOld = GetWindowLongW(hwnd, GWL_STYLE);
	if (fSet)
		dwStyleOld |= dwStyle;
	else
		dwStyleOld &= ~dwStyle;
	SetWindowLongW(hwnd, GWL_STYLE, dwStyleOld);
}

void SetDlgItemStyle(HWND hDlg, int nIDDlgItem, DWORD dwStyle, BOOL fSet)
{
	SetWindowStyle(GetDlgItem(hDlg, nIDDlgItem), dwStyle, fSet);
}

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		SendDlgItemMessageW(hwnd, IDC_NORMAL, BM_SETCHECK, BST_CHECKED, NULL);
		SendDlgItemMessageW(hwnd, IDC_SLIDER, TBM_SETRANGE, FALSE, MAKELONG(1, 100));
		break;
	case WM_CLOSE:
		EndDialog(hwnd, 0);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			case IDC_TOGGLETHEME:
				EndDialog(hwnd, IDC_TOGGLETHEME);
				break;
			case IDC_MARQUEE:
			{
				BOOL fChecked = BST_CHECKED == IsDlgButtonChecked(hwnd, IDC_MARQUEE);
				EnableWindow(GetDlgItem(hwnd, IDC_SLIDER), !fChecked);

				SetDlgItemStyle(hwnd, IDC_HPROGRESS, PBS_MARQUEE, fChecked);
				SetDlgItemStyle(hwnd, IDC_VPROGRESS, PBS_MARQUEE, fChecked);

				SendDlgItemMessageW(hwnd, IDC_HPROGRESS, PBM_SETMARQUEE, TRUE, 30);
				SendDlgItemMessageW(hwnd, IDC_VPROGRESS, PBM_SETMARQUEE, TRUE, 30);
				break;
			}
			case IDC_SMOOTH:
			{
				BOOL fChecked = BST_CHECKED == IsDlgButtonChecked(hwnd, IDC_SMOOTH);
				SetDlgItemStyle(hwnd, IDC_HPROGRESS, PBS_SMOOTH, fChecked);
				SetDlgItemStyle(hwnd, IDC_VPROGRESS, PBS_SMOOTH, fChecked);
				break;
			}
			case IDC_SMOOTHREVERSE:
			{
				BOOL fChecked = BST_CHECKED == IsDlgButtonChecked(hwnd, IDC_SMOOTHREVERSE);
				SetDlgItemStyle(hwnd, IDC_HPROGRESS, PBS_SMOOTHREVERSE, fChecked);
				SetDlgItemStyle(hwnd, IDC_VPROGRESS, PBS_SMOOTHREVERSE, fChecked);
				break;
			}
			case IDC_NORMAL:
			case IDC_PAUSED:
			case IDC_ERROR:
			{
				int iState = PBST_NORMAL;
				switch (LOWORD(wParam))
				{
				case IDC_PAUSED:
					iState = PBST_PAUSED;
					break;
				case IDC_ERROR:
					iState = PBST_ERROR;
					break;
				}
				SendDlgItemMessageW(hwnd, IDC_HPROGRESS, PBM_SETSTATE, iState, NULL);
				SendDlgItemMessageW(hwnd, IDC_VPROGRESS, PBM_SETSTATE, iState, NULL);
				break;
			}
		}
		return TRUE;
	case WM_HSCROLL:
	{
		UINT uPos = SendMessageW((HWND)lParam, TBM_GETPOS, 0, 0);
		SendDlgItemMessageW(hwnd, IDC_HPROGRESS, PBM_SETPOS, uPos, NULL);
		SendDlgItemMessageW(hwnd, IDC_VPROGRESS, PBM_SETPOS, uPos, NULL);
		return TRUE;
	}
	default:
		return FALSE;
	}
	return FALSE;
}

INT_PTR ShowDialog()
{
	INT_PTR nResult = DialogBoxParamW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDD_MAIN), NULL, DlgProc, NULL);
	if (nResult == IDC_TOGGLETHEME)
	{
		SetThemeAppProperties(GetThemeAppProperties() ? NULL : STAP_ALLOW_NONCLIENT | STAP_ALLOW_CONTROLS | STAP_ALLOW_WEBCONTENT);
		return ShowDialog();
	}
	return nResult;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	ShowDialog();
	return 0;
}