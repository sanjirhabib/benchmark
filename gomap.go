package main
import "fmt"
import "strconv"

func main() {
    m := make(map[string]string)
	for i := 0; i < 5000000; i++ {
		m["id."+strconv.Itoa(i%500000)]="val."+strconv.Itoa(i)
	}
    fmt.Println(len(m))
    fmt.Println(m["id.10000"])
}
