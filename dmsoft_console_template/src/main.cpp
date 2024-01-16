#include <atomic>
#include <csignal>
#include <iostream>
#include <thread>

#include "main.h"
#include "obj.h"

using namespace std;

#define WINDOW_TITLE "記事本"
#define WINDOW_CLASS ""
#define BIND_DISPLAY "dx2"
#define BIND_MOUSE "dx.mouse.position.lock.api"
#define BIND_KEYPAD "windows"
#define BIND_MODE 0

dmsoft *g_dm = NULL;
atomic<bool> g_script_run(false);

void signal_handler(int signal) {
  cout << "結束腳本中..." << endl;
  g_script_run = false;
}

void main_script(void *pParam) {
  // 主腳本
  dmsoft *dm = (dmsoft *)pParam;
  long dm_ret = 0;

  // 腳本開始
  while (g_script_run) {
    dm->delay(1000);
  }
}

void sub_script(void *pParam) {
  // 副腳本
  dmsoft *dm = (dmsoft *)pParam;
  long dm_ret = 0;

  // 腳本開始
  while (g_script_run) {
    long hwnd = dm->FindWindowEx(0, _T(WINDOW_CLASS), _T(WINDOW_TITLE));
    if (hwnd == 0) {
      cout << "遊戲窗口消失" << endl;
      g_script_run = false;
    }

    dm->delay(1000);
  }
}

void main_thread() {
  // 主線程
  dmsoft *dm = new dmsoft;
  long hwnd = 0;
  long dm_ret = 0;

  // 設置大漠插件目錄
  dm->SetPath(_T("."));

  // 查找遊戲窗口
  cout << "查找遊戲窗口...";
  hwnd = dm->FindWindowEx(0, _T(WINDOW_CLASS), _T(WINDOW_TITLE));
  if (hwnd == 0) {
    cout << "失敗" << endl;
    return;
  }
  cout << "成功(" << hwnd << ")" << endl;

  // 綁定窗口
  cout << "綁定窗口...";
  dm_ret = dm->BindWindow(hwnd, _T(BIND_DISPLAY), _T(BIND_MOUSE),
                          _T(BIND_KEYPAD), BIND_MODE);
  if (dm_ret != 1) {
    cout << "失敗(" << dm_ret << ")" << endl;
    return;
  }
  cout << "成功" << endl;

  // "----------------------------------------"
  cout << "----------------------------------------" << endl;

  // 腳本開始
  g_script_run = true;
  cout << "腳本開始..." << endl;
  main_script((void *)dm);

  // "----------------------------------------"
  cout << "----------------------------------------" << endl;

  // 解除綁定窗口
  cout << "解綁窗口...";
  dm_ret = dm->UnBindWindow();
  if (dm_ret != 1) {
    cout << "失敗(" << dm_ret << ")" << endl;
    return;
  }
  cout << "成功" << endl;

  // 釋放大漠插件
  delete dm;
  dm = NULL;
}

void sub_thread() {
  // 副線程
  dmsoft *dm = new dmsoft;

  // 設置大漠插件目錄
  dm->SetPath(_T("."));

  // 等待主線程綁定完窗口
  while (!g_script_run) {
    dm->delay(1000);
  }

  // 腳本開始
  sub_script((void *)dm);

  // 釋放大漠插件
  delete dm;
  dm = NULL;
}

int main() {
  // 設置訊號處理函數
  signal(SIGINT, signal_handler);

  // 初始化COM(mta)
  cout << "初始化COM...";
  if (CoInitializeEx(NULL, 0) != S_OK) {
    cout << "失敗" << endl;
    return -1;
  }
  cout << "成功" << endl;

  // 初始化MFC
  cout << "初始化MFC...";
  if (AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0) !=
      TRUE) {
    cout << "失敗" << endl;
    return -1;
  }
  cout << "成功" << endl;

  // 初始化大漠插件
  cout << "初始化大漠插件...";
  g_dm = new dmsoft;
  if (g_dm == NULL || g_dm->Ver().GetLength() == 0) {
    cout << "失敗" << endl;
    return -1;
  }
  cout << "成功" << endl;

  // 設置大漠插件目錄
  cout << "設置大漠插件目錄...";
  if (g_dm->SetPath(_T(".")) == 0) {
    cout << "失敗" << endl;
    return -1;
  }
  wcout << "成功(" << g_dm->GetPath().GetString() << ")" << endl;

  // 讀取註冊碼
  cout << "讀取註冊碼...";
  CString reg_code;
  if (g_dm->IsFileExist(_T("license.txt")) == 0) {
    g_dm->WriteFile(_T("license.txt"), _T("xxxxx"));
    cout << "失敗，創建\"license.txt\"文件，如需註冊大漠插件，請填入註冊碼"
         << endl;
  } else {
    reg_code = g_dm->ReadFile(_T("license.txt"));
    wcout << "成功(" << reg_code.GetString() << ")" << endl;
  }

  // 註冊大漠插件
  cout << "註冊大漠插件...";
  long dm_ret = g_dm->Reg(reg_code.GetString(), _T(""));
  if (dm_ret != 1) {
    cout << "失敗(" << dm_ret << ")" << endl;
    return -1;
  }
  cout << "成功" << endl;

  // 主線程
  thread t1(main_thread);

  // 副線程
  thread t2(sub_thread);

  // 等待線程結束
  t1.join();
  t2.join();

  // 釋放大漠插件
  delete g_dm;
  g_dm = NULL;

  return 0;
}