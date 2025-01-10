package main

import (
    "fmt"
    "os"
)

func main() {
    if len(os.Args) < 2 {
        fmt.Println("Usage: dit <command> [options]")
        return
    }

    command := os.Args[1]
    switch command {
    case "init":
        handleInit()
    case "add":
        handleAdd(os.Args[2:])
    case "list":
        handleList(os.Args[2:])
    case "sync":
        handleSync()
    case "build":
        handleBuild()
    default:
        fmt.Printf("Unknown command: %s\n", command)
    }
}
