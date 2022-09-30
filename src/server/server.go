package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"io"
	"os"
	"log"
	"net/http"
)


func uploadApiInformation(w http.ResponseWriter, req *http.Request) {
	body, err := ioutil.ReadAll(req.Body)

	if err != nil {
		fmt.Println("ioutil.ReadAll Error", err)
		return
	}
	defer req.Body.Close()

	json_data := []byte(body)
	//fmt.Println(body)

	var api_map interface{} //空接口
	json.Unmarshal(json_data, &api_map)
	data := api_map.(map[string]interface{})

	for k, v := range data {
		fmt.Println()
		switch k {
		case "MessageBoxA":
			fmt.Println(k, v)
		case "MessageBoxW":
			fmt.Println(k, v)
		case "HeapCreate":
			fmt.Println(k, v)
		case "HeapAlloc":
			fmt.Println(k, v)
		case "HeapFree":
			fmt.Println(k, v)
		case "HeapDestroy":
			fmt.Println(k, v)
		case "CreateFile":
			fmt.Println(k, v)
		case "CloseHandle":
			fmt.Println(k, v)
		case "WriteFile":
			fmt.Println(k, v)
		case "ReadFile":
			fmt.Println(k, v)
		case "RegCreateKeyEx":
			fmt.Println(k, v)
		case "RegDeleteValue":
			fmt.Println(k, v)
		case "RegOpenKeyEx":
			fmt.Println(k, v)
		case "RegCloseKey":
			fmt.Println(k, v)
		case "RegSetValueEx":
			fmt.Println(k,v)
		default:
			fmt.Println("Unkown:\n", k, v)
		}
	}
}

func uploadFile(w http.ResponseWriter, req *http.Request) {
	//req.ParseMultipartForm()
	fmt.Println("One case of uploading file is detected!")
	fmt.Println(req.Method)
	reader, err := req.MultipartReader()
	fmt.Println("post detected!")
	if err != nil {
		fmt.Println(err.Error())
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}
	for {
		part, err := reader.NextPart()
		if err == io.EOF {
			break
		}

		fmt.Printf("FileName=[%s], FormName=[%s]\n", part.FileName(), part.FormName())
		if part.FileName() == "" {  // this is FormData
			data, _ := ioutil.ReadAll(part)
			fmt.Printf("FormData=[%s]\n", string(data))
		} else {    // This is FileData
			dst, _ := os.Create("./tmpFile" + part.FileName())
			defer dst.Close()
			io.Copy(dst, part)
		}
		dt := Data{"./tmpFile" + part.FileName()}
		tmp := Response{0,"",dt}
		resp,_ := json.Marshal(tmp)
		w.Write(resp)
	}

	req.ParseMultipartForm(10000)
	mForm := req.MultipartForm

	for k, _ := range mForm.File {
		//file, fileHeader, err := req.FormFile(k)
		fmt.Print(k)
	}
}

func recvApiInfo()  {
	addr := "127.0.0.1:9999"

	http.HandleFunc("/uploadApiInformation", uploadApiInformation)
	http.HandleFunc("/uploadFile", uploadFile)
	fmt.Println("Listen ...",addr)
	if err := http.ListenAndServe(addr, nil); err != nil {
		log.Fatal("ListenAndServe: ",err)
	}	
}

