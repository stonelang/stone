package main

import (
    "fmt"
    "os"
)

func handleAdd(args []string) {
    if len(args) < 2 {
        fmt.Println("Usage: dit add <type> <name>")
        return
    }
    addType := args[0]
    name := args[1]

    switch addType {
    case "package":
        addPackage(name)
    case "module":
        if len(args) < 4 || args[2] != "--package" {
            fmt.Println("Usage: dit add module <name> --package <package>")
            return
        }
        packageName := args[3]
        addModule(name, packageName)
    default:
        fmt.Printf("Unknown add type: %s\n", addType)
    }
}

func addPackage(name string) {
    fmt.Printf("Adding package: %s\n", name)
    os.MkdirAll(fmt.Sprintf("%s", name), 0755)
    file, err := os.Create(fmt.Sprintf("%s/Package.dit", name))
    if err != nil {
        fmt.Println("Error creating Package.dit:", err)
        return
    }
    defer file.Close()

    file.WriteString(fmt.Sprintf("package %s {\n    description \"A new package\";\n    version 1.0;\n}", name))
    fmt.Println("Package added!")
}

func addModule(name, packageName string) {
    fmt.Printf("Adding module: %s to package: %s\n", name, packageName)
    os.MkdirAll(fmt.Sprintf("%s/%s", packageName, name), 0755)
    file, err := os.Create(fmt.Sprintf("%s/%s/%s.stone", packageName, name, name))
    if err != nil {
        fmt.Println("Error creating module file:", err)
        return
    }
    defer file.Close()

    file.WriteString(fmt.Sprintf("module %s;\n\n// Define your module here", name))
    fmt.Println("Module added!")
}
