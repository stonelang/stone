package main

import (
    "fmt"
    "os"
)

func handleInit() {
    fmt.Println("Initializing project...")
    os.MkdirAll("Project", 0755)
    file, err := os.Create("Project/Project.dit")
    if err != nil {
        fmt.Println("Error creating Project.dit:", err)
        return
    }
    defer file.Close()

    file.WriteString("project MyProject {\n    description \"A new project\";\n    version 1.0;\n}")
    fmt.Println("Project initialized!")
}
