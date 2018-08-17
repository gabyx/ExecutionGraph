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

import { NodeId } from '../model/NodeId';
import { GraphManipulationMessages } from '../../../../../../messages/schemas/ts';

export import sz = GraphManipulationMessages;

export abstract class GraphManipulationService {
  public abstract async addNode(graphId: string, type: string, name: string): Promise<sz.AddNodeResponse>;
  public abstract async removeNode(graphId: string, nodeId: NodeId): Promise<void>;
}
