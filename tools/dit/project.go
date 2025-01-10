package main

type Project struct {
    Name        string    `json:"name"`
    Description string    `json:"description"`
    Version     string    `json:"version"`
    Packages    []Package `json:"packages"`
}

func NewProject(name, description, version string) Project {
    return Project{
        Name:        name,
        Description: description,
        Version:     version,
        Packages:    []Package{},
    }
}

func (p *Project) AddPackage(pkg Package) {
    p.Packages = append(p.Packages, pkg)
}
