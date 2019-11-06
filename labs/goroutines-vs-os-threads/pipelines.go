package main

import (
	"time"
	"log"
)

func main() {
	for i := 41000000; i < 41000001; i++ {
		start := time.Now()

		chnls := make([]chan int, 0)
		chnls = append(chnls, make(chan int))
		go strt(chnls[0])

		for j := 0; j < i; j++ {
			chnls = append(chnls, make(chan int))
			go pipe(chnls[j], chnls[j+1])
		}
		for x := range chnls[i] {
			elapsed := time.Since(start)
		    log.Printf("%d pipes took: %s", x, elapsed)
	    }
	}
}

func pipe(src, dst chan int) {
	var x int
	x = <-src
	x += 1
	dst <- x
	close(dst)
}

func strt(src chan int){
	src <- 0
    close(src)
}
