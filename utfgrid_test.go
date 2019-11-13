// Copyright (c) 2019-present FlyWave, Inc. All Rights Reserved.
// See License.txt for license information.

package utfgrid

import "testing"

func TestTile(t *testing.T) {
	utf := LoadUTFGrid("./testdata/utfgrid.json")

	key, err := utf.GetKey(35, 35)

	if err != nil {
		t.Error(err)
	}

	if len(key) == 0 {
		t.Error("err")
	}
}
