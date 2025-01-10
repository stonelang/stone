package main

type File struct {
    Name    string `json:"name"`
    Content string `json:"content"`
}

func NewFile(name, content string) File {
    return File{Name: name, Content: content}
}
