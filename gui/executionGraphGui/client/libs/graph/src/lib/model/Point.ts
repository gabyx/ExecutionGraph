// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sun Jul 29 2018
//  @author Simon Spoerri, simon (døt) spoerri (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

export interface Point {
  x: number;
  y: number;
};

export type Position = Point;

export function diff(a: Point, b: Point): Point {
  return {
    x: a.x - b.x,
    y: a.y - b.y
  };
}

export function norm2(v: Point): number {
  return v.x * v.x + v.y * v.y;
}

export function norm(v: Point): number {
  return Math.sqrt(norm2(v));
}

export function normalize(v: Point): Point {
  const length = norm(v);
  return {
    x: v.x / length,
    y: v.y / length
  };
}