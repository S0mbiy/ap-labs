// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 227.

// Netcat is a simple read/write client for TCP servers.
package main

import (
	"io"
	"log"
	"net"
	"os"
)

//!+
func main() {
	var name, server string
	var serverflag, userflag bool
	for _, s := range os.Args {
	    if s == "-user" {
	        userflag=true
			continue
	    }else if userflag{
			userflag= false
			name = s
		}
		if s == "-server" {
	        serverflag=true
			continue
	    }else if serverflag{
			serverflag= false
			server = s
		}
	}
	if name == "" || server == ""{
		log.Fatal("Missing username and server host")
	}

	conn, err := net.Dial("tcp", server)
	if err != nil {
		log.Fatal(err)
	}
	done := make(chan struct{})
	go func() {
		io.Copy(os.Stdout, conn) // NOTE: ignoring errors
		log.Println("done")
		done <- struct{}{} // signal the main goroutine
	}()
	_, erro := conn.Write([]byte(name))
	if erro != nil {
		log.Fatal(erro)
	}
	mustCopy(conn, os.Stdin)
	conn.Close()
	<-done // wait for background goroutine to finish
}

//!-

func mustCopy(dst io.Writer, src io.Reader) {
	if _, err := io.Copy(dst, src); err != nil {
		log.Fatal(err)
	}
}
