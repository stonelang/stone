package main

type Module struct {
    Name        string `json:"name"`
    Description string `json:"description"`
}

func NewModule(name, description string) Module {
    return Module{Name: name, Description: description}
}
