(function () {
  let amis = amisRequire('amis/embed');
  let amisJSON = { 
    "type": "page",
    "title": "winTracer",
    "data": {
      "items":[
{"Name":"MessageBoxW","return":"1","err":"NULL","parameters":"uType => [ 0 ];    hWnd => [ 0x00000000 ];    lpCaption => [ I'm MessageBoxW's title ];    lpText => [ I'm MessageBoxW ];    ",},
{"Name":"HeapCreate","return":"0xCA1C0000","err":"NULL","parameters":"fIOptions => [ 1 ];    current_hHeap => [ 0xC8620000 ];    dwInitialSize => [ 40960 ];    dwMaximumSize => [ 409600 ];    ",},
{"Name":"HeapAlloc","return":"0xCA1C0720","err":"NULL","parameters":"dwFlags => [ 0 ];    hHeap => [ 0xCA1C0000 ];    dwBytes => [ 120 ];    ",},
{"Name":"HeapFree","return":"1","err":"NULL","parameters":"dwBytes => [ 0xCA1C0720 ];    dwFlags => [ 0 ];    hHeap => [ 0xCA1C0000 ];    ",},
{"Name":"HeapDestroy","return":"1","err":"NULL","parameters":"hHeap => [ 0xCA1C0000 ];    ",},
{"Name":"HeapCreate","return":"0xC9FC0000","err":"NULL","parameters":"current_hHeap => [ 0xC8620000 ];    dwInitialSize => [ 40960 ];    dwMaximumSize => [ 409600 ];    fIOptions => [ 1 ];    ",},
{"Name":"HeapAlloc","return":"0xC9FC0720","err":"NULL","parameters":"dwBytes => [ 400 ];    dwFlags => [ 0 ];    hHeap => [ 0xC9FC0000 ];    ",},
{"Name":"HeapFree","return":"1","err":"NULL","parameters":"hHeap => [ 0xC9FC0000 ];    dwBytes => [ 0xC9FC0720 ];    dwFlags => [ 0 ];    ",},
{"Name":"HeapFree","return":"0","err":"Double Free: Terminated","parameters":"hHeap => [ 0xC9FC0000 ];    dwBytes => [ 0xC9FC0720 ];    dwFlags => [ 0 ];    ",},
{"Name":"HeapDestroy","return":"1","err":"NULL","parameters":"hHeap => [ 0xC9FC0000 ];    ",},
{"Name":"CreateFile","return":"0x00000138","err":"NULL","parameters":"dwDesiredAccess => [ 3.221225472e+09 ];    lpFileName => [ a.txt ];    hTemplateFile => [ 0x00000000 ];    lpSecurityAttributes => [ 0 ];    dwCreationDisposition => [ 2 ];    dwFlagsAndAttributes => [ 0 ];    dwShareMode => [ 1 ];    ",},
{"Name":"WriteFile","return":"1","err":"NULL","parameters":"hFile => [ 0x00000138 ];    lpBuffer => [ 0x259EF938 ];    lpNumberOfBytesWritten => [ 0x00000000 ];    lpOverlapped => [ 0 ];    nNumberOfBytesToWrite => [ 37 ];    ",},
{"Name":"WriteFile","return":"1","err":"Try to change .exe file","parameters":"lpOverlapped => [ 0 ];    nNumberOfBytesToWrite => [ 5 ];    hFile => [ 0x00000138 ];    lpBuffer => [ 0x259EF938 ];    lpNumberOfBytesWritten => [ 0x00000000 ];    ",},
{"Name":"WriteFile","return":"1","err":"Try to change .dll file","parameters":"hFile => [ 0x00000138 ];    lpBuffer => [ 0x259EF938 ];    lpNumberOfBytesWritten => [ 0x00000000 ];    lpOverlapped => [ 0 ];    nNumberOfBytesToWrite => [ 6 ];    ",},
{"Name":"WriteFile","return":"1","err":"Try to change .ocx file","parameters":"lpOverlapped => [ 0 ];    nNumberOfBytesToWrite => [ 7 ];    hFile => [ 0x00000138 ];    lpBuffer => [ 0x259EF938 ];    lpNumberOfBytesWritten => [ 0x00000000 ];    ",},
{"Name":"CreateFile","return":"0x00000138","err":"NULL","parameters":"hTemplateFile => [ 0x00000000 ];    dwFlagsAndAttributes => [ 0 ];    dwCreationDisposition => [ 4 ];    dwDesiredAccess => [ 2.147483648e+09 ];    dwShareMode => [ 1 ];    lpFileName => [ a.txt ];    lpSecurityAttributes => [ 0 ];    ",},
{"Name":"ReadFile","return":"1","err":"NULL","parameters":"hFile => [ 0x00000138 ];    lpBuffer => [ 0xC8626FC0 ];    lpOverlapped => [ 0 ];    nNumberOfBytesToRead => [ 55 ];    ",},
{"Name":"RegCreateKeyEx","return":"0","err":"NULL","parameters":"lpSecurityAttributes => [ 0x00000000 ];    phkResult => [ 0x259EF658 ];    Reserved => [ 0x00000000 ];    dwOptions => [ 0x00000000 ];    lpClass => [ (null) ];    lpSubKey => [ aaaMykey ];    lpdwDisposition => [ 0x00000000 ];    samDesired => [ 0x000F003F ];    ",},
{"Name":"RegCreateKeyEx","return":"0","err":"Self-starting executable file item","parameters":"dwOptions => [ 0x00000001 ];    lpSecurityAttributes => [ 0x00000000 ];    lpSubKey => [ aaaMykey ];    lpdwDisposition => [ 0x00000000 ];    samDesired => [ 0x000F003F ];    Reserved => [ 0x00000000 ];    lpClass => [ (null) ];    phkResult => [ 0x259EF658 ];    ",},
{"Name":"RegSetValueEx","return":"0","err":"Registry Value Changed!","parameters":"Reserved => [ 0x00000000 ];    dwType => [ 0x00000036 ];    hKey => [ 0x00000150 ];    lpSubKey => [ https://github.com/Ganliber ];    ",},
{"Name":"RegOpenKeyEx","return":"","err":"NULL","parameters":"hKey => [ 0x80000001 ];    lpSubKey => [ aaaMykey ];    phkResult => [ 0x259EF978 ];    samDesired => [ 0x000F003F ];    ulOptions => [ 0x00000000 ];    ",},
{"Name":"RegDeleteValue","return":"2","err":"RegDeleteValue failed!","parameters":"lpSubKey => [ Ganliber ];    hKey => [ 0x0000013C ];    ",},
{"Name":"RegCloseKey","return":"0","err":"NULL","parameters":"hKey => [ 0x0000013C ];    ",},
{"Name":"socket","return":"316","err":"NULL","parameters":"af => [ 2 ];    protocol => [ 6 ];    type => [ 1 ];    ",},
{"Name":"send","return":"0","err":"NULL","parameters":"buf => [ 0x259EF8B8 ];    flags => [ 0x00000000 ];    len => [ 0x0000000D ];    s => [ 1.8446744073709552e+19 ];    ",},
{"Name":"closesocket","return":"-1","err":"10038","parameters":"s => [ 0xFFFFFFFF ];    ",},
{"Name":"closesocket","return":"0","err":"NULL","parameters":"s => [ 0x0000013C ];    ",},
{"Name":"socket","return":"316","err":"NULL","parameters":"protocol => [ 6 ];    type => [ 1 ];    af => [ 2 ];    ",},
{"Name":"connect","return":"0","err":"NULL","parameters":"ip => [ 127.0.0.1 ];    namelen => [ 0x00000010 ];    port => [ 1234 ];    s => [ 316 ];    sockaddr => [ 0x259EF7A8 ];    ",},
{"Name":"recv","return":"0","err":"NULL","parameters":"flags => [ 0x00000000 ];    len => [ 0x000000FF ];    s => [ 316 ];    buf => [ 0x259EF7E0 ];    ",},
{"Name":"closesocket","return":"0","err":"NULL","parameters":"s => [ 0x0000013C ];    ",},
{"Name":"CreateFile","return":"0x0000013C","err":"NULL","parameters":"lpFileName => [ a.exe ];    dwFlagsAndAttributes => [ 0 ];    dwShareMode => [ 1 ];    hTemplateFile => [ 0x00000000 ];    dwCreationDisposition => [ 2 ];    dwDesiredAccess => [ 3.221225472e+09 ];    lpSecurityAttributes => [ 0 ];    ",},
{"Name":"CreateFile","return":"0x0000013C","err":"NULL","parameters":"dwCreationDisposition => [ 4 ];    dwFlagsAndAttributes => [ 0 ];    lpSecurityAttributes => [ 0 ];    lpFileName => [ testCode.exe ];    dwDesiredAccess => [ 2.147483648e+09 ];    dwShareMode => [ 1 ];    hTemplateFile => [ 0x00000000 ];    ",},
{"Name":"RegOpenKeyEx","return":"","err":"RegOpenKeyEx failed!","parameters":"phkResult => [ 0x259EF978 ];    samDesired => [ 0x00020019 ];    ulOptions => [ 0x00000000 ];    hKey => [ 0x80000001 ];    lpSubKey => [ SOFTWAREMicrosoftWindowsCurrentVersionRun ];    ",},
{"Name":"RegCloseKey","return":"6","err":"RegCloseKey failed!","parameters":"hKey => [ 0x00000000 ];    ",},
      ]
    },
    "body": [{
      "type": "cards",
      "source": "$items",
      "card": {
        "header": {
          "avatarText": "${Name|substring:0:2|upperCase}",
          "avatarTextBackground": [
            "#FFB900",
            "#D83B01",
            "#B50E0E",
            "#E81123",
            "#B4009E",
            "#5C2D91",
            "#0078D7",
            "#00B4FF",
            "#008272"
          ]
        },
        "body": [
          {
            "label": "API",
            "name": "Name"
          },
          {
            "label": "Return",
            "name": "return"
          },
          {
            "name": "err",
            "label": "Error"
          }
        ],
        "actions": [
          {
            "type": "button",
            "level": "link",
            "icon": "fa fa-eye",
            "actionType": "dialog",
            "dialog": {
              "title": "查看详情",
              "body": {
                "type": "form",
                "body": [
                  {
                    "label": "API Name",
                    "name": "Name",
                    "type": "static"
                  },
                  {
                    "label": "API return",
                    "name": "return",
                    "type": "static"
                  },
                  {
                    "label": "Error",
                    "name": "err",
                    "type": "static"
                  },
                  {
                    "label": "Parameters",
                    "name": "parameters",
                    "type": "static"
                  },
                ]
              }
            }
          }
        ]
      }
    }],
    "toolbar": [
    {
      "type": "button",
      "label": "Refresh",
      "onEvent": {
        "click": {
          "actions": [
            {
              "componentId": "",
              "args": {
              },
              "actionType": "refresh"
            }
          ]
        }
      },
      "tooltip": "Refresh to view results",
      "icon": "fa fa-stack-overflow"
    },
    {
      "type": "button",
      "label": "API HOOK",
      "onEvent": {
        "click": {
          "actions": [
            {
              "componentId": "",
              "args": {
                "api": {
                  //"url": "http://127.0.0.1:9999/updateCards",
                  "url": "http://10.12.173.133:9999/updateCards",
                  "method": "get"
                },
                //"redirect": "http://127.0.0.1:9999/",
                "redirect": "http://10.12.173.133:9999/",
                "messages": {
                  "success": "成了:) 刷新一下咩",
                  "failed": "寄了:("
                },
              },
              "actionType": "ajax"
            }
          ]
        }
      },
      "id": "u:875d42f1f918",
      "tooltip": "win32 API HOOK",
      "tooltipPlacement": "bottom",
      "rightIcon": "",
      "size": "sm",
      "block": false,
      "level": "default",
      "icon": "fa fa-slideshare"
    },
    {
      "type": "input-file",
      "label": "",
      "name": "file",
      "btnLabel": "import file",
      "accept": ".exe",
      "receiver": {
        //"url": "http://127.0.0.1:9999/uploadFile",
        "url": "http://10.12.173.133:9999/uploadFile",
        "method": "POST",
        "dataType": "form-data"
      },
      "mode": "normal",
      "descriptionClassName": "b-a m-md p-md",
      "horizontal": {
        "left": 2,
        "right": 10
      },
      "className": "text-lg"
    },
    ],
    "aside": [
      {
        "type": "nav",
        "stacked": true,
        "links": [
          {
            "label": "没用的导航栏",
            "to": "?id=1"
          },
          {
            "label": "看似文件没有上传成功,实际上我已经偷偷收到了:D",
            "to": "?id=2"
          }
        ],
        "id": "u:b0297f5acdc7"
      },
      {
        "type": "service",
        "body": [
        ],
        "id": "u:1716d9638db5"
      }
    ],
  }    
  let amisScoped = amis.embed('#root', amisJSON);
})();