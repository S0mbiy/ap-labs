// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 254.
//!+

// Chat is a server that lets clients chat with each other.
package main

import (
	"time"
	"bufio"
	"fmt"
	"log"
	"net"
	"strconv"
	"os"
	"strings"
)

//!+broadcaster
type client chan<- string // an outgoing message channel

type user struct {
name string
channel  client
connection net.Conn
}

type mssg struct {
msg string
channel  client
name string
}

var (
	entering = make(chan user)
	leaving  = make(chan user)
	messages = make(chan mssg) // all incoming client messages
)

func broadcaster() {
	clients := make(map[string]user) // all connected clients
	badhosts := make(map[string]bool)
	missingAdmin := true
	var admin user
	for {
		select {
		case msg := <-messages:
			s := strings.Split(msg.msg, " ")
			cmd := s[1]
			if cmd=="/users"{
				keys := make([]string, 0, len(clients))
				for key := range clients {
					keys = append(keys, key)
  				}
				msg.channel <- strings.Join(keys, ", ")
			}else if cmd=="/msg" {
				if len(s)>3{
					if to, found := clients[s[2]]; found {
						to.channel <- "@" + msg.name + ": " + strings.Join(s[3:], " ")
					}else{
						msg.channel <- "irc-server > User isn't connected"
					}
				}else{
					msg.channel <- "irc-server > Usage: /msg <user> <msg>"
				}
			}else if cmd=="/time"{
				loc, e := time.LoadLocation("America/Mexico_City")
				if e != nil {
					log.Fatal(e)
				}
				msg.channel <- "irc-server > Local Time: America/Mexico_City " + time.Now().In(loc).Format("15:04:05")
			} else if cmd=="/user"{
				if len(s)==3{
					if to, found := clients[s[2]]; found {
						msg.channel <- "irc-server > username: " + to.name + ", IP: " + strings.Split(to.connection.RemoteAddr().String(), ":")[0]
					}else{
						msg.channel <- "irc-server > User isn't connected"
					}
				}else{
					msg.channel <- "irc-server > Usage: /user <user>"
				}
			}else if cmd=="/kick"{
				if msg.name == admin.name{
					if len(s)==3{
						if to, found := clients[s[2]]; found {
							log.Printf("[%s] was kicked", to.name)
							to.channel <- "irc-server > You're kicked from this channel"
							to.channel <- "irc-server > Bad language is not allowed on this channel"
							to.connection.Close()
							for _, client := range clients {
								client.channel <- "irc-server > [" + to.name + "] was kicked from channel for bad language policy violation"
							}
						}else{
							msg.channel <- "irc-server > User isn't connected"
						}
					}else{
						msg.channel <- "irc-server > Usage: /kick <user>"
					}
				}else{
					msg.channel <- "irc-server > Only admin can kick user"
				}
			}else{
			// Broadcast incoming message to all
			// clients' outgoing message channels.
				for _, client := range clients {
					if msg.name!=client.name{
						client.channel <- msg.msg
					}
				}
			}

		case cli := <-entering:
			if _,found := clients[cli.name]; found {
				log.Printf("Client trying to use occupied username: [%s|%s]", cli.name, cli.connection.RemoteAddr().String())
				cli.channel <- "irc-server > This username is already in use, please try another one."
				badhosts[cli.connection.RemoteAddr().String()] = true
				cli.connection.Close()
			}else{
				log.Printf("New connected user [%s]", cli.name)
				cli.channel <- "irc-server > Your user [" + cli.name + "] is successfully logged"
				msg := "irc-server > " + cli.name + " has arrived"
				for _, client := range clients {
					client.channel <- msg
				}
				clients[cli.name] = cli
			}
			if missingAdmin{
				missingAdmin = false
				admin = cli
				admin.channel <- "irc-server > Congrats, you were the first user."
				admin.channel <- "irc-server > You're the new IRC Server ADMIN"
				log.Printf("[%s] was promoted as the channel ADMIN", admin.name)
			}

		case cli := <-leaving:
			if _, found := badhosts[cli.connection.RemoteAddr().String()]; !found {
				delete(clients, cli.name)
				log.Printf("[%s] left", cli.name)
				msg:= "irc-server > " + cli.name + " has left"
				for _, client := range clients {
					client.channel <- msg
				}
			}
			close(cli.channel)
			cli.connection.Close()
		}
	}
}

//!-broadcaster

//!+handleConn
func handleConn(conn net.Conn) {
	ch := make(chan string) // outgoing client messages
	go clientWriter(conn, ch)

	buffer := make([]byte, 1400)
	dataSize, err := conn.Read(buffer)
	if err != nil {
		log.Fatal(err)
	}
	who := string(buffer[:dataSize])

	ch <- "irc-server > Welcome to the Simple IRC Server"
	entering <- user{name: who, channel: ch, connection: conn}

	input := bufio.NewScanner(conn)
	for input.Scan() {
		messages <- mssg{who + ": " + input.Text(), ch, who}
	}
	// NOTE: ignoring potential errors from input.Err()

	leaving <- user{name: who, channel: ch, connection: conn}
}

func clientWriter(conn net.Conn, ch <-chan string) {
	for msg := range ch {
		fmt.Fprintln(conn, msg) // NOTE: ignoring network errors
	}
}

//!-handleConn

//!+main
func main() {
	portflag:= false
	ipflag:= false
	var ip, port, host string
	for _, s := range os.Args {
	    if s == "-port" {
	        portflag=true
			continue
	    }else if portflag{
			portflag= false
			i, err := strconv.Atoi(s)
			if err != nil || i>65535 || i<0{
				log.Fatal(err)
			}
			port = s
		}
		if s == "-host" {
	        ipflag=true
			continue
	    }else if ipflag{
			ipflag= false
			ip = s
		}
	}
	if ip!="" && port!=""{
		host = ip+":"+port
	}else{
		log.Fatal("Missing host or port")
		return
	}
	listener, err := net.Listen("tcp", host)
	if err != nil {
		log.Fatal(err)
	}
	log.Println("Simple IRC Server started at ", host)
	log.Println("Ready for receiving new clients")
	go broadcaster()
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err)
			continue
		}
		go handleConn(conn)
	}
}

//!-main
