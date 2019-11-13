// Copyright (c) 2019-present FlyWave, Inc. All Rights Reserved.
// See License.txt for license information.

package utfgrid

import (
	"encoding/json"
	"errors"
	"fmt"
	"math"
	"os"
)

type UTFGridConfig struct {
	Resolution int
	TileSize   int
}

func DefaultUTFGridConfig() *UTFGridConfig {
	return &UTFGridConfig{4, 256}
}

type UTFGridTile struct {
	Grid []string                          `json:"grid"`
	Keys []string                          `json:"keys"`
	Data map[string]map[string]interface{} `json:"data"`
}

type UTFGrid struct {
	Config UTFGridConfig
	Tile   *UTFGridTile
}

func LoadUTFGrid(path string) *UTFGrid {
	bs, err := os.Open(path)
	if err != nil {
		fmt.Println(err)
	}
	var cs *UTFGridTile
	json.NewDecoder(bs).Decode(&cs)
	return &UTFGrid{Config: *DefaultUTFGridConfig(), Tile: cs}
}

func NewUTFGrid(tile *UTFGridTile, conf UTFGridConfig) *UTFGrid {
	return &UTFGrid{Config: conf, Tile: tile}
}

func idx(i float64, res int) int {
	return int(math.Floor(i / float64(res)))
}

func (g *UTFGrid) getKey(x, y float64) (int, error) {
	if g.Tile != nil &&
		g.Tile.Grid != nil &&
		y >= 0 &&
		x >= 0 &&
		math.Floor(y) < float64(g.Config.TileSize) &&
		math.Floor(x) < float64(g.Config.TileSize) {
		row := idx(x, g.Config.Resolution)
		col := idx(y, g.Config.Resolution)
		return resolveCode([]rune(g.Tile.Grid[row])[col]), nil
	}
	return 0, errors.New("not fount")
}

func (g *UTFGrid) GetKey(x, y float64) (string, error) {
	key, err := g.getKey(x, y)
	if err != nil {
		return "", err
	}
	if g.Tile.Keys != nil &&
		key < len(g.Tile.Keys) {
		return g.Tile.Keys[key], nil
	}
	return "", errors.New("not fount")
}

func (g *UTFGrid) GetFeature(x, y float64) (map[string]interface{}, error) {
	key, err := g.GetKey(x, y)
	if err != nil {
		return nil, err
	}
	return g.Tile.Data[key], nil
}

func resolveCode(key rune) int {
	if key >= 93 {
		key--
	}
	if key >= 35 {
		key--
	}
	return int(key - 32)
}
