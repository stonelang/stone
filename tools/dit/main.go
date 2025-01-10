package main

import (
    "fmt"
    "os"
)

func main() {
    dp := NewDit("Project.dit")

    if len(os.Args) < 2 {
        fmt.Println("Usage: dit <command> [options]")
        return
    }

    switch os.Args[1] {
    case "init":
        if len(os.Args) < 3 {
            fmt.Println("Usage: dit init <project_name>")
            return
        }
        dp.Init(os.Args[2], "A new project", "1.0")
    case "add-package":
        if len(os.Args) < 3 {
            fmt.Println("Usage: dit add package <package_name>")
            return
        }
        dp.AddPackage(os.Args[2], "Description for package", "1.0")
    case "add-module":
        if len(os.Args) < 5 || os.Args[3] != "--package" {
            fmt.Println("Usage: dit add module <module_name> --package <package_name>")
            return
        }
        dp.AddModule(os.Args[4], os.Args[2], "Description for module")
    case "add-file":
        if len(os.Args) < 5 || os.Args[3] != "--module" {
            fmt.Println("Usage: dit add file <file_name> --module <module_name>")
            return
        }
        dp.AddFile(os.Args[4], os.Args[2], "File content here")
    case "sync":
        dp.Sync()
    case "build":
        dp.Build()
    default:
        fmt.Printf("Unknown command: %s\n", os.Args[1])
    }
}
