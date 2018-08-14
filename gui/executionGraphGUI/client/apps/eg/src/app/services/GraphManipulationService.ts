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

import { IIdentifier } from "@eg/comon/Identifier"
import * as D from "@eg/comon/DataTypes";
import { GraphManipulationMessages } from '@messages/index';

export import sz = GraphManipulationMessages;

export abstract class GraphManipulationService extends IIdentifier {
  public abstract async addNode(graphId: string, type: string, name: string): Promise<sz.AddNodeResponse>;
  public abstract async removeNode(graphId: string, nodeId: D.NodeId): Promise<void>;
}


