package main

import (
	"fmt"
	"os"
	"path/filepath"
)

// scanDir stands for the directory scanning implementation
func scanDir(dir string) error {
	os.Chdir(dir)
	m := make(map[string]int)
	err := filepath.Walk(".", func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}
		fileInfo, err := os.Lstat(path)
		if err != nil {
                 return err
         }
		 if fileInfo.Mode()&os.ModeSymlink == os.ModeSymlink {
				 m["sym"]+=1;
         }else if fileInfo.Mode()&os.ModeDir == os.ModeDir {
				 m["dir"]+=1;
         }else if fileInfo.Mode()&os.ModeDevice == os.ModeDevice {
				 m["dev"]+=1;
         }else if fileInfo.Mode()&os.ModeSocket == os.ModeSocket {
				 m["sok"]+=1;
         }else {
			 	 m["other"]+=1;
         }
		return nil
	})
	fmt.Println("Directory Scanner Tool")
	fmt.Println("+-------------------------+------+")
	fmt.Printf("| Path                    | %s |\n", dir)
	fmt.Println("+-------------------------+------+")
	fmt.Printf("| Directories             | %d |\n", m["dir"])
	fmt.Printf("| Symbolic Links          | %d |\n", m["sym"])
	fmt.Printf("| Devices                 | %d |\n", m["dev"])
	fmt.Printf("| Sockets                 | %d |\n", m["sok"])
	fmt.Printf("| Other files             | %d |\n", m["other"])
	fmt.Println("+-------------------------+------+")
	if err != nil {
		fmt.Printf("error walking the path %q: %v\n", dir, err)
		return err
	}
	return nil
}

func main() {

	if len(os.Args) < 2 {
		fmt.Println("Usage: ./dir-scan <directory>")
		os.Exit(1)
	}

	scanDir(os.Args[1])
}
