package utfgrid

import (
	"bytes"
	"encoding/json"
	"testing"
)

func TestTile(t *testing.T) {
	utf := LoadUTFGrid("./testdata/utfgrid.json")

	key, err := utf.GetKey(35, 35)

	if err != nil {
		t.Error(err)
	}

	if len(key) == 0 {
		t.Error("err")
	}

	test := "{\"grid\":[\"sss\", \"333\"]}"

	type grid struct {
		Grid []string `json:"grid"`
	}

	var g grid
	err = json.NewDecoder(bytes.NewBuffer([]byte(test))).Decode(&g)
	if err != nil {
		t.Error(err)
	}
}
