// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sun Jan 13 2019
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================
import { NodeId } from 'apps/eg/src/app/model';
import { Point, Position } from '../../model/Point';
import { Observable } from 'rxjs';
import { posix } from 'path';

export type BodyMap<Body> = Map<NodeId, Body>;

export type EngineInput<Body, Link> = { bodies: BodyMap<Body>; links: Link[] };

export type EngineOutputState = { pos: Position; id: NodeId };
export function createOutputState(): EngineOutputState {
  return { pos: Point.zero.copy(), id: undefined };
}

export type EngineOutput = EngineOutputState[];

export type BodyCreator<Body> = (id: NodeId, pos: Point) => Body;
export type LinkCreator<Body, Link> = (b1: Body, b2: Body) => Link;

export interface GraphConverter {
  <Body, Link>(createBody: BodyCreator<Body>, createLink: LinkCreator<Body, Link>): Promise<EngineInput<Body, Link>>;
}

export abstract class ILayoutEngine {
  public abstract run(converter: GraphConverter): Observable<EngineOutput>;
}
