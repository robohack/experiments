
package main

import (
	"encoding/json"
	"fmt"
)

type Response1 struct {
    Page   int
    Fruits []string
}
type Response2 struct {
    Page   int      `json:"sheet"`
    Fruits []string `json:"food"`
}
	
func demoStructs() {
	res1D := &Response1 {
		Page:   1,
		Fruits: []string{"apple", "peach", "pear"}}
	res1B, _ := json.Marshal(res1D)

	fmt.Println(string(res1B))
	fmt.Println("")

	fancy, err := json.MarshalIndent(res1D, "", "\t")
	if err != nil {
		panic(err)
	}
	fmt.Println(string(fancy))
	fmt.Println("")

	res2D := &Response2 {
		Page:   1,
		Fruits: []string{"apple", "peach", "pear"}}
	res2B, _ := json.Marshal(res2D)

	fmt.Println(string(res2B))
	fmt.Println("")

	fancy2, err := json.MarshalIndent(res2D, "", "\t")
	if err != nil {
		panic(err)
	}
	fmt.Println(string(fancy2))
	fmt.Println("")

	str := `{"sheet": 1, "food": ["apple", "peach"]}`
	res := Response2{}
	json.Unmarshal([]byte(str), &res)
	fmt.Println(res)
	fmt.Println(res.Fruits[0])
	fmt.Println("")
}

func main() {
	demoStructs()
}	
