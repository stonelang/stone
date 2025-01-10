package main

import (
    "fmt"
    "os"
)

type Dit struct {
    ProjectFile string
    Project     Project
}

func NewDit(projectFile string) *Dit {
    return &Dit{ProjectFile: projectFile}
}

// Load project metadata from file
func (d *Dit) Load() error {
    file, err := os.Open(d.ProjectFile)
    if err != nil {
        return err
    }
    defer file.Close()

    decoder := json.NewDecoder(file)
    return decoder.Decode(&d.Project)
}

// Save project metadata to file
func (d *Dit) Save() error {
    file, err := os.Create(d.ProjectFile)
    if err != nil {
        return err
    }
    defer file.Close()

    encoder := json.NewEncoder(file)
    encoder.SetIndent("", "  ")
    return encoder.Encode(d.Project)
}

// Init command
func (d *Dit) Init(name, description, version string) error {
    d.Project = NewProject(name, description, version)
    return d.Save()
}

// Add package command
func (d *Dit) AddPackage(name, description, version string) error {
    pkg := NewPackage(name, description, version)
    d.Project.AddPackage(pkg)
    return d.Save()
}

// Add module command
func (d *Dit) AddModule(packageName, moduleName, moduleDescription string) error {
    for i, pkg := range d.Project.Packages {
        if pkg.Name == packageName {
            mod := NewModule(moduleName, moduleDescription)
            d.Project.Packages[i].AddModule(mod)
            return d.Save()
        }
    }
    return fmt.Errorf("package '%s' not found", packageName)
}

// Add file command
func (d *Dit) AddFile(moduleName, fileName, fileContent string) error {
    for _, pkg := range d.Project.Packages {
        for i, mod := range pkg.Modules {
            if mod.Name == moduleName {
                file := NewFile(fileName, fileContent)
                d.Project.Packages[i].Modules = append(pkg.Modules, file)
                return d.Save()
            }
        }
    }
    return fmt.Errorf("module '%s' not found", moduleName)
}

// Sync command
func (d *Dit) Sync() {
    fmt.Println("Synchronizing dependencies...")
    // Placeholder for actual sync logic
    fmt.Println("Dependencies synchronized successfully!")
}

// Build command
func (d *Dit) Build() {
    fmt.Println("Building project...")
    // Placeholder for actual build logic
    fmt.Println("Build complete!")
}
