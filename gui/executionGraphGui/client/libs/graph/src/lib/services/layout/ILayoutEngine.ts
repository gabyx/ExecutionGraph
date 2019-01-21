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
import { Observable } from 'rxjs';
import { Vector2 } from '@eg/common';

export type NodeMap<Node> = Map<NodeId, Node>;

export interface EngineInput<Node, Edge> {
  nodes: Node[];
  edges: Edge[];
}

export interface EngineOutputState {
  pos: Vector2;
  id: NodeId;
  opaqueData: any;
}
export type EngineOutput = EngineOutputState[];

export type NodeCreator<Node> = (id: NodeId, pos: Vector2, opaqueData: any) => Node;
export type EdgeCreator<Node, Edge> = (b1: Node, b2: Node) => Edge;

export type GraphConverter = <Node, Edge>(
  createNode: NodeCreator<Node>,
  createEdge: EdgeCreator<Node, Edge>
) => Promise<EngineInput<Node, Edge>>;

export abstract class ILayoutEngine {
  public abstract run(converter: GraphConverter): Observable<EngineOutput>;
}
