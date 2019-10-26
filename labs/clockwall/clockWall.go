// Netcat2 is a read-only TCP client with channels
package main

import (
	"log"
	"net"
	"os"
	"strings"
)

func main() {
	var ny, tok, lon string
	for _, s := range os.Args[1:] {
		list := strings.Split(s, "=")
	    if list[0] == "NewYork" {
			ny = list[1]
	    }else if list[0] == "Tokyo" {
			tok = list[1]
		}else if list[0] == "London" {
			lon = list[1]
		}else{
			log.Fatal("Invalid location")
		}
	}
	var conny, contok, conlon net.Conn
	var err error
	if ny!=""{
		conny, err = net.Dial("tcp", ny)
		if err != nil {
			log.Fatal(err)
		}
	}
	if tok!=""{
		contok, err = net.Dial("tcp", tok)
		if err != nil {
			log.Fatal(err)
		}
	}
	if lon!=""{
		conlon, err = net.Dial("tcp", lon)
		if err != nil {
			log.Fatal(err)
		}
	}
	done := make(chan int)
	go func(conny, contok, conlon net.Conn) {
		for{
			buffer := make([]byte, 1400)
			if ny!=""{
				dataSize, err := conny.Read(buffer)
				if err != nil {
					log.Fatal(err)
					break
				}
				data := buffer[:dataSize]
                log.Print(string(data))
			}
			if tok!=""{
				dataSize, err := contok.Read(buffer)
				if err != nil {
					log.Fatal(err)
					break
				}
				data := buffer[:dataSize]
                log.Print(string(data))
			}
			if lon!=""{
				dataSize, err := conlon.Read(buffer)
				if err != nil {
					log.Fatal(err)
					break
				}
				data := buffer[:dataSize]
                log.Print(string(data))
			}
		}
		log.Println("done")
		done <- 2 // signal the main goroutine
	}(conny, contok, conlon)

	x := 1
	x = <-done // wait for background goroutine to finish
	log.Println("Channel Closed with value: ", x)
	close(done)
}
