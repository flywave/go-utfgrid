// Copyright (c) 2019-present FlyWave, Inc. All Rights Reserved.
// See License.txt for license information.

package utfgrid

/*
#include <stdlib.h>
#include "geometry.h"
#cgo CFLAGS: -I ./
#cgo CXXFLAGS: -I ./
*/
import "C"
import "unsafe"

type GeoType int

var (
	UNKNOWN         = GeoType(C.GEO_UNKNOWN)
	POINT           = GeoType(C.GEO_POINT)
	MULTIPOINT      = GeoType(C.GEO_MULTIPOINT)
	LINESTRING      = GeoType(C.GEO_LINESTRING)
	MULTILINESTRING = GeoType(C.GEO_MULTILINESTRING)
	POLYGON         = GeoType(C.GEO_POLYGON)
	MULTIPOLYGON    = GeoType(C.GEO_MULTIPOLYGON)
	COLLECTION      = GeoType(C.GEO_COLLECTION)
)

type Geometry struct {
	m *C.struct__geometry_t
	t GeoType
}

func NewGeometry(tp GeoType) *Geometry {
	return &Geometry{
		m: C.geometry_new(C.int(tp)),
		t: tp,
	}
}

func (m *Geometry) Free() {
	C.geometry_free(m.m)
	m.m = nil
}

func (m *Geometry) Type() GeoType {
	return GeoType(C.geometry_type(m.m))
}

func (m *Geometry) addPoint(x, y float64) {
	C.add_point(m.m, C.double(x), C.double(y))
}

func (m *Geometry) addLine(x1, y1, x2, y2 float64) {
	C.add_line(m.m, C.double(x1), C.double(y1), C.double(x2), C.double(y2))
}

func (m *Geometry) addPolygon(exter []float64, inner [][]float64) {
	C.add_polygon(m.m, (*C.double)(unsafe.Pointer(&exter[0])), C.int(len(exter)/2))
	for i := range inner {
		ring := inner[i]
		C.add_inner_linear_ring(m.m, (*C.double)(unsafe.Pointer(&ring[0])), C.int(len(ring)/2))
	}
}

func (m *Geometry) SetPoint(p1 []float64) {
	m.addPoint(p1[0], p1[1])
}

func (m *Geometry) SetMultiPoint(p1 [][]float64) {
	for i := range p1 {
		m.addPoint(p1[i][0], p1[i][1])
	}
}

func (m *Geometry) SetLine(l [][]float64) {
	m.addLine(l[0][0], l[0][1], l[1][0], l[1][1])
}

func (m *Geometry) SetMultiLine(l [][][]float64) {
	for i := range l {
		m.addLine(l[i][0][0], l[i][0][1], l[i][1][0], l[i][1][1])
	}
}

func (m *Geometry) SetPolygon(pl [][][]float64) {
	exter := make([]float64, len(pl[0])*2)
	for i := range pl[0] {
		exter[i*2] = pl[0][i][0]
		exter[i*2+1] = pl[0][i][1]
	}

	rings := make([][]float64, len(pl)-1)
	for i := 1; i < len(pl); i++ {
		rings[i-1] = make([]float64, len(pl[i])*2)
		for j := range pl[i] {
			rings[i-1][j*2] = pl[i][i][0]
			rings[i-1][j*2+1] = pl[i][i][1]
		}
	}
	m.addPolygon(exter, rings)
}

func (m *Geometry) SetMultiPolygon(pl [][][][]float64) {
	for i := range pl {
		m.SetPolygon(pl[i])
	}
}

func (m *Geometry) SetCollection(geos []Geometry) {
	for i := range geos {
		C.add_geometry(m.m, geos[i].m)
	}
}

func NewLine(l [][]float64) *Geometry {
	geo := NewGeometry(LINESTRING)
	geo.SetLine(l)
	return geo
}

func NewMultiLine(p [][][]float64) *Geometry {
	geo := NewGeometry(MULTILINESTRING)
	geo.SetMultiLine(p)
	return geo
}

func NewPolygon(p [][][]float64) *Geometry {
	geo := NewGeometry(POLYGON)
	geo.SetPolygon(p)
	return geo
}

func NewMultiPolygon(p [][][][]float64) *Geometry {
	geo := NewGeometry(MULTIPOLYGON)
	geo.SetMultiPolygon(p)
	return geo
}

func NewPoint(p []float64) *Geometry {
	geo := NewGeometry(POINT)
	geo.SetPoint(p)
	return geo
}

func NewMultiPoint(p [][]float64) *Geometry {
	geo := NewGeometry(MULTIPOINT)
	geo.SetMultiPoint(p)
	return geo
}

func NewCollection(geos []Geometry) *Geometry {
	geo := NewGeometry(COLLECTION)
	geo.SetCollection(geos)
	return geo
}
