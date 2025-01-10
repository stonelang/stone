package main

type Package struct {
    Name        string   `json:"name"`
    Description string   `json:"description"`
    Version     string   `json:"version"`
    Modules     []Module `json:"modules"`
}

func NewPackage(name, description, version string) Package {
    return Package{
        Name:        name,
        Description: description,
        Version:     version,
        Modules:     []Module{},
    }
}

func (p *Package) AddModule(module Module) {
    p.Modules = append(p.Modules, module)
}
