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
import { Observable } from 'rxjs';
import { Vector2 } from '@eg/common';

export interface InputState {
  pos: Vector2;
  opaqueData: any;
}
export type OutputState = InputState;
export type EngineOutput = OutputState[];

export type NodeCreator<Node> = (state: InputState) => Node;
export type EdgeCreator<Node, Edge> = (b1: Node, b2: Node) => Edge;

export type GraphConverter = <Node, Edge>(
  createNode: NodeCreator<Node>,
  createEdge: EdgeCreator<Node, Edge>
) => Promise<void>;

export abstract class ILayoutEngine {
  public abstract run(converter: GraphConverter): Observable<EngineOutput>;
}
