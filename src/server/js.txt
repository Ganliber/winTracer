(function () {
  let amis = amisRequire('amis/embed');
  let amisJSON = { 
    "type": "page",
    "title": "winTracer",
    "data": {
      "items":[
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
                  "url": "http://127.0.0.1:9999/updateCards",
                  //"url": "http://10.12.173.133:9999/updateCards",
                  "method": "get"
                },
                "redirect": "http://127.0.0.1:9999/",
                //"redirect": "http://10.12.173.133:9999/",
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
        "url": "http://127.0.0.1:9999/uploadFile",
        //"url": "http://10.12.173.133:9999/uploadFile",
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
            "label": "没用的导航栏:(",
            "to": "?id=1"
          },
          {
            "label": "依旧没用的导航栏:D",
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