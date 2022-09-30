## 部署

> 首先你需要下载`release`文件

### 功能文件简介

```
<-- Project Structure -->
.
│  js.txt
│  README.md
│  winTracer.exe
│
├─hostfiles
│      testHost.exe
│
├─templates
│  │  index.html
│  │
│  └─sdk
│      │  amisjson.js  
│       ...
│      
└─tmpFile
        testDll.dll
        testLoader.exe
```

* `file : js.txt` : amisjson.js 的原始备份，重新上传文件后，js.txt 会覆盖 amisjson.js 即覆盖掉存储于`"data" -> item"`域下的`json`数据，而这些数据是截获API前端展示的数据源（该版本未引入数据库）
* `file : winTracer.exe` : 由Gin编写的后端可执行服务，无依赖，涉及到的相对路径等操作无误即可顺利执行，建议在中断执行，可以打印出`GET/POST`等请求的内容以及日志内容
* `folder : hostfiles` : 由我提供的一个测试文件，内含对二十种`win32API`调用的测试代码，包括一些异常/危险情况的截获，处理如堆的二次释放，创建自启动的注册表项，文件自我复制检测，对如`.exe`，`.dll`，`.ocx`文件的篡改等
* `folder : templates` : 前端文件夹，我自己基于`amis`的sdk创建了一个`amisjson.js`文件，该文件内容是原`index.html`中的js区域的一部分包含`json`核心数据段的内容，免于用数据库管理截获的API
* `folder : tmpFile` : 临时文件夹，client上传的文件会被保存到此处并被`Go`后端执行`testLoader.exe`将`testDll.dll`注入到上传的`Host`文件里（如hostfiles里所提供的示例`Host`代码文件），并通过向本地发`HTTP : POST`请求的方式将截获API的信息以`json`的形式发送到`Go`后端

### 执行步骤

* download release file

* depress file under any empty folder you want

* open terminal such as powershell

* key in

  ```powershell
  .\winTracer
  ```

* open browser like Edge or Chrome

* visit `127.0.0.1:9999`

* upload file such as `testHost.exe`( any .exe you want )

* API Hook

* F12 -> 禁止缓存

* refresh



### 切换 URL

目前适用的URL为`127.0.0.1:9999`

* `templates/sdk/amisjson.js` : toolbar -> url ( 三处 )
* `js.txt` : url ( 三处 ) 
* 源代码重新编译：testDll.dll -> `sendApiInformation(json &info)`中的URL
* Go文件中的`ginApis.go` -> `ginNet()` ->`r.Run( newURL )`