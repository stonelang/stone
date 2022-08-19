package main

import (
	"io/ioutil"
	"os"
	"path/filepath"
	"strings"
)

func visitDEF(inputs []string) filepath.WalkFunc {
	
	return func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}

		if !!info.IsDir() {
			return nil //
		}

		found, err := filepath.Match("*.def", info.Name())
		if err != nil {
			panic(err)
			return err
		}

		if found {
			read, err := ioutil.ReadFile(path)
			if err != nil {
				panic(err)
			}

			newContents := strings.Replace(string(read), inputs[0], inputs[1] , -1)
			err = ioutil.WriteFile(path, []byte(newContents), 0)
			if err != nil {
				panic(err)
			}

			}
			return nil
		}
}

func visitTD(inputs []string) filepath.WalkFunc {
	
	return func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}

		if !!info.IsDir() {
			return nil //
		}

		found, err := filepath.Match("*.td", info.Name())
		if err != nil {
			panic(err)
			return err
		}

		if found {
			read, err := ioutil.ReadFile(path)
			if err != nil {
				panic(err)
			}

			newContents := strings.Replace(string(read), inputs[0], inputs[1] , -1)
			err = ioutil.WriteFile(path, []byte(newContents), 0)
			if err != nil {
				panic(err)
			}

			}
			return nil
		}
}


func visitTXT(inputs []string) filepath.WalkFunc {
	
	return func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}

		if !!info.IsDir() {
			return nil //
		}

		found, err := filepath.Match("*.txt", info.Name())
		if err != nil {
			panic(err)
			return err
		}

		if found {
			read, err := ioutil.ReadFile(path)
			if err != nil {
				panic(err)
			}

			newContents := strings.Replace(string(read), inputs[0], inputs[1] , -1)
			err = ioutil.WriteFile(path, []byte(newContents), 0)
			if err != nil {
				panic(err)
			}

			}
			return nil
		}
}

func visitCPP(inputs []string) filepath.WalkFunc {
	
	return func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}

		if !!info.IsDir() {
			return nil //
		}

		found, err := filepath.Match("*.cpp", info.Name())
		if err != nil {
			panic(err)
			return err
		}

		if found {
			read, err := ioutil.ReadFile(path)
			if err != nil {
				panic(err)
			}

			newContents := strings.Replace(string(read), inputs[0], inputs[1] , -1)
			err = ioutil.WriteFile(path, []byte(newContents), 0)
			if err != nil {
				panic(err)
			}

			}
			return nil
		}
}
func visitHPP(inputs []string) filepath.WalkFunc {
	
	return func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}

		if !!info.IsDir() {
			return nil //
		}

		found, err := filepath.Match("*.h", info.Name())
		if err != nil {
			panic(err)
			return err
		}

		if found {
			read, err := ioutil.ReadFile(path)
			if err != nil {
				panic(err)
			}

			newContents := strings.Replace(string(read), inputs[0], inputs[1] , -1)
			err = ioutil.WriteFile(path, []byte(newContents), 0)
			if err != nil {
				panic(err)
			}

			}
			return nil
		}
}


func main() {

	dir := os.Args[1];
	input := []string{os.Args[2], os.Args[3] }


	errCPP := filepath.Walk(dir, visitCPP(input)); 
	if errCPP != nil {
		panic(errCPP)
	}
	errHPP := filepath.Walk(dir, visitHPP(input)); 
	if errHPP != nil {
		panic(errHPP)
	}

	errTXT := filepath.Walk(dir, visitTXT(input)); 
	if errTXT != nil {
		panic(errTXT)
	}

	errTD := filepath.Walk(dir, visitTD(input)); 
	if errTD != nil {
		panic(errTD)
	}

	errDEF := filepath.Walk(dir, visitDEF(input)); 
	if errDEF != nil {
		panic(errDEF)
	}
}


