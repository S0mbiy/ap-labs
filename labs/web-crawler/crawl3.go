// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 241.

// Crawl2 crawls web links starting with the command-line arguments.
//
// This version uses a buffered channel as a counting semaphore
// to limit the number of concurrent calls to links.Extract.
//
// Crawl3 adds support for depth limiting.
//
package main

import (
	"fmt"
	"log"
	"os"
	"flag"
	"regexp"
	"github.com/todostreaming/gopl.io/ch5/links"
)

//!+sema
// tokens is a counting semaphore used to
// enforce a limit of 20 concurrent requests.
var tokens = make(chan struct{}, 20)

func crawl(url string) []string {
	fmt.Println(url)
	tokens <- struct{}{} // acquire a token
	list, err := links.Extract(url)
	<-tokens // release the token

	if err != nil {
		log.Print(err)
	}
	return list
}

//!-sema


type work struct {
level int
list []string
}

//!+
func main() {
	var depth = flag.Int("depth", 1, "integer greater than 0")
	flag.Parse()
	worklist := make(chan work)
	var n int // number of pending sends to worklist

	// Start with the command-line arguments.
	n++

	go func() { worklist <- work{level: 1, list: os.Args[1:]}}()

	// Crawl the web concurrently.
	seen := make(map[string]bool)
	var validLink = regexp.MustCompile(`^(http:\/\/www\.|https:\/\/www\.|http:\/\/|https:\/\/)?[a-z0-9]+([\-\.]{1}[a-z0-9]+)*\.[a-z]{2,5}(:[0-9]{1,5})?(\/.*)?$`)
	for ; n > 0; n-- {
		list := <-worklist
		if list.level <= *depth {
			for _, link := range list.list {
				if validLink.MatchString(link){
					if !seen[link]{
						seen[link] = true
						n++
						go func(link string) {
							worklist <- work{level: list.level+1, list: crawl(link)}
						}(link)
					}
				}
			}
		}
	}
}

//!-
