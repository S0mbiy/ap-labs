// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 156.

// Package geometry defines simple types for plane geometry.
//!+point
package main

import (
	"fmt"
	"math"
	"os"
	"strconv"
	"math/rand"
	"time"
)


type Point struct{ x, y float64 }

func (p Point) X() float64 {
	return p.x
}
func (p Point) Y() float64 {
	return p.y
}

// traditional function
func makePoints(n int) Path {
	l := make([]Point, n)
	for i := 0; i < n; i++{
		rand.Seed(time.Now().UTC().UnixNano())
		tmp := Point{float64(rand.Intn(200)-100), float64(rand.Intn(200)-100)}
		if (i > 2){
			for j := 0; j < i-2; j++ {
				if (doIntersect(l[j],l[j+1], l[i-1], tmp)){
					rand.Seed(time.Now().UTC().UnixNano())
					j = -1
					tmp = Point{float64(rand.Intn(200)-100), float64(rand.Intn(200)-100)}
				}
			}
			l[i] = tmp
		}else{
			l[i] = tmp
		}
	}
	return Path{l}
}
func onSegment(p, q, r Point)bool {
    if (q.X() <= math.Max(p.X(), r.X()) && q.X() >= math.Min(p.X(), r.X()) &&
        q.Y() <= math.Max(p.Y(), r.Y()) && q.Y() >= math.Min(p.Y(), r.Y())){
			return true
	}
    return false
}
func orientation(p, q, r Point) int {
    val := (q.Y() - p.Y()) * (r.X() - q.X()) - (q.X() - p.X()) * (r.Y() - q.Y())
    if (val == 0){
		return 0 // colinear
	}else if(val > 0){
		return 1 // clock wise
	}else{
		return 2	// counterclock wise
	}

}
func doIntersect(p1, q1, p2, q2 Point) bool{
    o1 := orientation(p1, q1, p2)
    o2 := orientation(p1, q1, q2)
    o3 := orientation(p2, q2, p1)
    o4 := orientation(p2, q2, q1)
    // General case
    if (o1 != o2 && o3 != o4){
		return true
	}

    // Special Cases
    // p1, q1 and p2 are colinear and p2 lies on segment p1q1
    if (o1 == 0 && onSegment(p1, p2, q1)){
		return true
	}

    // p1, q1 and q2 are colinear and q2 lies on segment p1q1
    if (o2 == 0 && onSegment(p1, q2, q1)){
		return true
	}

    // p2, q2 and p1 are colinear and p1 lies on segment p2q2
    if (o3 == 0 && onSegment(p2, p1, q2)){
		return true
	}

    // p2, q2 and q1 are colinear and q1 lies on segment p2q2
    if (o4 == 0 && onSegment(p2, q1, q2)){
		return true
	}

    return false
}

// same thing, but as a method of the Point type
func (p Point) Distance(q Point) float64 {
	return math.Hypot(q.X()-p.X(), q.Y()-p.Y())
}


//!-point

//!+path

// A Path is a journey connecting the points with straight lines.
type Path struct{p []Point}

// Distance returns the distance traveled along the path.
func (path Path) Distance() float64 {
	sum := 0.0
	for i := range path.p {
		if i > 0 {
			sum += path.p[i-1].Distance(path.p[i])
		}
	}
	sum += path.p[len(path.p)-1].Distance(path.p[0])
	return sum
}

func main() {

	if len(os.Args) < 2 {
		fmt.Println("Usage: ./geometry <sides-number>")
		os.Exit(1)
	}
	arg,err := strconv.Atoi(os.Args[1])
	if err == nil {
		if (arg < 3){
			fmt.Println("Enter a number greater than 2")
			os.Exit(1)
		}
		p := makePoints(arg)
		fmt.Printf("- Generating a [%d] sides figure\n", arg)
		fmt.Printf("- Figure's vertices\n")
  		for e := 0; e < arg; e++ {
  			fmt.Printf("- (  %f,   %f)\n", p.p[e].X(), p.p[e].Y())
  		}
		fmt.Printf("- Figure's Perimeter\n")
		fmt.Printf("- ")
		for f := 0; f < arg-1; f++ {
			fmt.Printf(" %.2f +", p.p[f].Distance(p.p[f+1]))
		}
		fmt.Printf(" %.2f", p.p[arg-1].Distance(p.p[0]))
  		fmt.Printf(" = %.2f\n", p.Distance())
	}
}

//!-path
