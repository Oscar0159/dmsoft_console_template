# dmsoft console template
大漠插件腳本(console)模板

# 使用說明
1. 至[大漠綜合插件](http://www.dmwebsite.net/)下載後台系統
2. 註冊帳號並儲值金額
3. 產生註冊碼並填入腳本執行檔目錄底下的`license.txt`中
4. 下載大漠插件並註冊到系統(regsvr32)
5. 以管理員身分執行`dmsoft_console_template.exe`

# 編譯說明
## 環境
- Visual Studio IDE (需安裝C++開發環境及MFCLibrary)
- 使用x86編譯
- 大漠插件需註冊到系統(同上)

## 大漠版本更新
1. 下載新的大漠插件，並註冊到系統
2. 使用大漠類生成工具產生`obj.h`及`obj.cpp`
3. 將`obj.h`及`obj.cpp`替換掉專案底下的檔案
4. `obj.h`需`#include <afxdisp.h>`

