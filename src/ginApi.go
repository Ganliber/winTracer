package main

import (
	"errors"
	"fmt"
	"io/ioutil"
	"net/http"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
	"github.com/gin-gonic/gin"
)

type Data struct {
	value	string	`json:value`
};

type Response struct {
	status	int		`json:status`
	msg		string	`json:msg`
	data	Data	`json:data`
};

//假设最多有1000个API
//var apiInfo [1000]map[string]interface{}
var apiList []ApiStructure

type ApiStructure struct {
	Name 	string
	errors 	string
	ret 	string
	parameters 	string
}

func ginNet() {
	//创建一个默认的路由引擎
	r := gin.Default()
	r.MaxMultipartMemory = 8 << 20 // 8 MiB

	r.LoadHTMLGlob("templates/index.html")
	r.Static("/sdk","templates/sdk")

	// GET:请求方式; /hello: 请求路径
	// 当客户端以GET方法请求/hello路径时,会执行后面的匿名函数
	r.GET("/", func (c *gin.Context)  {
		c.HTML(http.StatusOK, "index.html", gin.H{})
	})
	r.GET("/index", func (c *gin.Context)  {
		c.HTML(http.StatusOK, "index.html", gin.H{})
	})
	r.POST("/uploadApiInformation",ginUploadApiInformation)
	r.POST("/uploadFile", ginUploadFile)
	r.GET("/updateCards",updateCards)
	
	//启动 HTTP服务
	//r.Run("127.0.0.1:9999")
	r.Run("10.12.173.133:9999")
}

func updateCards(c *gin.Context){
		fmt.Println(len(apiList))
		length := len(apiList)
		/*
		* 四种属性: Name, params, return, err
		*/
		filePath := "./templates/sdk/amisjson.js"
		lineBytes, err := ioutil.ReadFile(filePath)
		fmt.Println("试图读取文件")
		var lines []string
		if err != nil {
			fmt.Println(err)
		} else {
			//将文件字节数组转换为字符串并按\n转化成切片，即每一行
			contents := string(lineBytes)
			lines = strings.Split(contents, "\n")
		}
		var newLines []string
		//setTailFlag := true
		index := 0
		for _, line := range lines {
			newLines = append(newLines, line)
			index ++
			if index != 7 {
				continue
			}
			fmt.Println("The detected line is: ", line)
			//输入内容
			for i:=1 ; i < length; i++{
				infoLine := fmt.Sprint(
					"{",
					"\"Name\":","\"",apiList[i].Name,"\",",
					"\"return\":","\"",apiList[i].ret,"\",",
					"\"err\":","\"",apiList[i].errors,"\",",
					"\"parameters\":","\"",apiList[i].parameters,"\",",
					"}",
					",",
				)
				fmt.Println(infoLine)
				newLines = append(newLines, infoLine)
			}
		}
		file, err := os.OpenFile("./templates/sdk/amisjson.js",os.O_WRONLY, 0666)
		defer file.Close()
		_, err = file.WriteString(strings.Join(newLines,"\n"))
		if err != nil {
			fmt.Println("Error")
		}

	
	//c.HTML(http.StatusOK, "index.html", gin.H{})
	value := map[string]string{"value":"Success!"}
	dt := map[string]interface{}{
		"status": 0,
		"msg": "",
		"data": value,
	}
	c.JSON(http.StatusOK, dt)
}

func ginUploadFile(c *gin.Context) {
	// FormFile方法会读取参数“upload”后面的文件名，返回值是一个File指针，和一个FileHeader指针，和一个err错误。
	fmt.Println("清除原始的数据")
	fileB := "./templates/sdk/amisjson.js"
	fileA := "./js.txt"
	fileCpErr := copyData(fileA, fileB)
	if (fileCpErr !=nil) {
		fmt.Println(fileCpErr)
	} else {
		fmt.Println("json配置文件内容初始化完成!")
	}

	fmt.Println("清空切片数据")
	apiList = apiList[:0]
	
	fmt.Println("成功捕获上传的文件")
	file, err := c.FormFile("file")
	if err != nil {
		c.String(http.StatusBadRequest, fmt.Sprintf("get form err: %s", err.Error()))
		return
	}

	basePath := "./tmpFile/"
	filename := basePath + filepath.Base(file.Filename)
	if err := c.SaveUploadedFile(file, filename); err != nil {
		c.String(http.StatusBadRequest, fmt.Sprintf("upload file err: %s", err.Error()))
		return
	}

	//c.String(http.StatusOK,fmt.Sprintf("文件 %s 上传成功 ", file.Filename))
 	//absPath,err := filepath.Abs(filename)
 	//dt := Data{absPath}
 	//tmp := Response{0,"",dt}
	absPath,err := filepath.Abs(filename)
	fmt.Println(absPath)
	value := map[string]string{"value":absPath}
	dt := map[string]interface{}{
		"status": 0,
		"msg": "",
		"data": value,
	}
	fmt.Println(dt)

	// /* 执行注入 */
	fmt.Println("开始注入")
	//ps := exec.Command("powershell")
	cd := exec.Command(".\\tmpFile\\testLoader.exe", absPath)
	err2 := cd.Run()
	if err2 == nil {
		fmt.Println("Injecting Succeed!")
	} else {
		fmt.Println("Injecting Failed!")
	}

 	c.JSON(http.StatusOK, dt)
}

func ginUploadApiInformation(c *gin.Context) {
	
	//fmt.Println(len(apiInfo))
	json := make(map[string]interface{}) //接收
	c.BindJSON(&json)
	//fmt.Println(json)

	for k, v := range json {
		fmt.Println(k)
		v_data := v.(map[string]interface{})

		params := "" //参数字符串
		name := ""
		erros := ""
		returnV := ""

		for k1, v1 := range v_data {
			if k1=="Name" {
				name += fmt.Sprintf("%v",v1)
			} else if k1=="err"{
				erros += fmt.Sprintf("%v",v1)
			} else if k1=="return" {
				returnV += fmt.Sprintf("%v",v1)
			} else {
				// _format := ""
				// if len(k1) < 8 {
				// 	_format += "\\t\\t\\t"
				// } else if len(k1) >= 8 && len(k1) < 16 {
				// 	_format += "\\t\\t"
				// } else {
				// 	_format += "\\t"
				// }
				// params += fmt.Sprintf("%v%v%v%v%v",k1,_format,"[",v1,"]\\n")
				params += fmt.Sprintf("%v%v%v%v",k1," => [ ",v1," ];    ")
			}
		}
		var api ApiStructure
		api.errors = erros
		api.parameters = params
		api.Name = name
		api.ret = returnV
		apiList = append(apiList, api)

		fmt.Println(name)
		fmt.Println(erros)
		fmt.Println(returnV)
		fmt.Println(params)
	}
}

//copyData 将源文件数据导入到目标文件中去（目标文件为空文件）
func copyData(sourcePath,targetPath string) error {
	data,err := ioutil.ReadFile(sourcePath)
	if err != nil {
		return errors.New("read file err")
	}
	err = ioutil.WriteFile(targetPath,data,0666)
	if err != nil {
		return errors.New("write file err")
	}
	return nil
}
