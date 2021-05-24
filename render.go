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

type RenderOptions struct {
	Width  float64
	Radius float64
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

func (m *Render) DrawLine(d KeyData, l [][]float64, width float64) {
	geo := NewLine(l)
	defer geo.Free()
	m.drawGeometry(d, geo, &RenderOptions{Width: width})
}

func (m *Render) DrawMultiLine(d KeyData, p [][][]float64, width float64) {
	geo := NewMultiLine(p)
	defer geo.Free()
	m.drawGeometry(d, geo, &RenderOptions{Width: width})
}

func (m *Render) DrawPolygon(d KeyData, p [][][]float64) {
	geo := NewPolygon(p)
	defer geo.Free()
	m.drawGeometry(d, geo, nil)
}

func (m *Render) DrawMultiPolygon(d KeyData, p [][][][]float64) {
	geo := NewMultiPolygon(p)
	defer geo.Free()
	m.drawGeometry(d, geo, nil)
}

func (m *Render) DrawPoint(d KeyData, p []float64, r float64) {
	geo := NewPoint(p)
	defer geo.Free()
	m.drawGeometry(d, geo, &RenderOptions{Radius: r})
}

func (m *Render) DrawMultiPoint(d KeyData, p [][]float64, r float64) {
	geo := NewMultiPoint(p)
	defer geo.Free()
	m.drawGeometry(d, geo, &RenderOptions{Radius: r})
}

func (m *Render) drawGeometry(d KeyData, geo *Geometry, opt *RenderOptions) {
	d.key = rune(C.utfgrid_draw_geometry(m.m, geo.m, (*C.double)(unsafe.Pointer(&opt.Width)), (*C.double)(unsafe.Pointer(&opt.Radius))))
	m.keys = append(m.keys, d)
}

func (m *Render) getBuffer() ([]byte, int) {
	var size int
	data := C.GoString(C.utfgrid_to_buf(m.m, (*C.int)(unsafe.Pointer(&size))))
	return []byte(data), size
}

func (m *Render) ToUTFGrid() UTFGridTile {
	ut := UTFGridTile{Data: make(map[string]map[string]interface{})}
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
