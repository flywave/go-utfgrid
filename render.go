// Copyright (c) 2019-present FlyWave, Inc. All Rights Reserved.
// See License.txt for license information.

package utfgrid

/*
#include <stdlib.h>
#include "utfgrid_c_api.h"
#cgo CFLAGS: -I ./
#cgo CXXFLAGS: -I ./
*/
import "C"

import (
	"bytes"
	"encoding/json"
	"errors"
	"unsafe"
)

type KeyData struct {
	ID   string
	Data map[string]interface{}
	key  rune
}

type Render struct {
	m    *C.struct__utfgrid_t
	keys []KeyData
}

func New(width, height int) *Render {
	return &Render{
		m: C.utfgrid_new(C.int(width), C.int(height)),
	}
}

func (m *Render) LastError() error {
	if err := C.utfgrid_last_error(m.m); err != nil {
		return errors.New("Render: " + C.GoString(err))
	}
	return nil
}

func (m *Render) Free() {
	C.utfgrid_free(m.m)
	m.m = nil
}

func (m *Render) Reset() {
	C.utfgrid_reset(m.m)
}

func (m *Render) DrawLine(d KeyData, p1, p2 [2]float64, width float64) {
	d.key = rune(C.utfgrid_draw_line(m.m, C.double(p1[0]), C.double(p1[1]), C.double(p2[0]), C.double(p2[1]), C.double(width)))
	m.keys = append(m.keys, d)
}

func (m *Render) DrawPolygon(d KeyData, p [][2]float64) {
	fv := make([]float64, len(p)*2)
	for i := range p {
		fv[i*2] = p[i][0]
		fv[i*2+1] = p[i][1]
	}
	d.key = rune(C.utfgrid_draw_polygon(m.m, (*C.double)(unsafe.Pointer(&fv[0])), C.int(len(p)*2)))
	m.keys = append(m.keys, d)
}

func (m *Render) DrawEllipse(d KeyData, p1 [2]float64, rx float64, ry float64) {
	d.key = rune(C.utfgrid_draw_ellipse(m.m, C.double(p1[0]), C.double(p1[1]), C.double(rx), C.double(ry)))
	m.keys = append(m.keys, d)
}

func (m *Render) DrawCircle(d KeyData, p1 [2]float64, r float64) {
	m.DrawEllipse(d, p1, r, r)
}

func (m *Render) getBuffer() ([]byte, int) {
	var size int
	data := C.GoString(C.utfgrid_to_buf(m.m, (*C.int)(unsafe.Pointer(&size))))
	return []byte(data), size
}

func (m *Render) ToUTFGrid() UTFGridTile {
	var ut UTFGridTile
	for i := range m.keys {
		key := m.keys[i].ID
		ut.Keys = append(ut.Keys, key)
		ut.Data[key] = m.keys[i].Data
	}

	type grid struct {
		Grid []string `json:"grid"`
	}

	var g grid
	buf, _ := m.getBuffer()
	json.NewDecoder(bytes.NewBuffer([]byte(buf))).Decode(&g)
	ut.Grid = g.Grid
	return ut
}
