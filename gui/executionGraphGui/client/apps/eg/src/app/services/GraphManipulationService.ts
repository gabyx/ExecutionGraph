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

import { NodeId, Node, Connection, Socket } from '../model';
import { GraphManipulationMessages } from '@eg/messages';

export import sz = GraphManipulationMessages;
import { GraphId } from '../model/Graph';

export abstract class GraphManipulationService {
  public abstract async addNode(graphId: GraphId, type: string, name: string): Promise<Node>;
  public abstract async removeNode(graphId: GraphId, nodeId: NodeId): Promise<void>;

  public abstract async addConnection(
    graphId: GraphId,
    source: Socket,
    target: Socket,
    cycleDetection: boolean
  ): Promise<Connection>;

  public abstract async removeConnection(graphId: GraphId, source: Socket, target: Socket): Promise<void>;
}
