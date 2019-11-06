package main

import (
	"log"
	"time"
)

func main() {
	chnls := make([]chan int, 0)
	chnls = append(chnls, make(chan int))
	chnls = append(chnls, make(chan int))
	ch := make(chan int)
	go run(chnls, ch)
	x := <-ch
	log.Printf("%d comunications per second", x)
	close(ch)
	close(chnls[0])
	close(chnls[1])
}

func run(chnls []chan int, ch chan int){
	go strt(chnls[0])
	start := time.Now()
	for true {
		elapsed := time.Since(start)
		if  elapsed>=time.Second{
			tmp := <-chnls[0]
			ch <-tmp
			break
		}
		go pipe(chnls[0], chnls[1])
		go pipe(chnls[1], chnls[0])
	}
}

func strt(src chan int){
	src <- 0
}

func pipe(src, dst chan int) {
	var x int
	x = <-src
	x += 1
	dst <- x
}
