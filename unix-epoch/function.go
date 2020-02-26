package p

import (
	"fmt"
	"time"
	"net/http"
)

func UnixEpoch(w http.ResponseWriter, r *http.Request) {
    now := time.Now()
    secs := now.Unix()
    fmt.Fprint(w, secs)
}
