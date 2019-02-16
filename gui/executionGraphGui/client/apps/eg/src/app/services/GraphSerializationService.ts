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

import { GraphSerializationMessages } from '@eg/messages';
import { Graph } from '../model';
export import sz = GraphSerializationMessages;
import { GraphTypeId, GraphId } from '../model/Graph';

export abstract class GraphSerializationService {
  public abstract async saveGraph(graphId: GraphId, filePath: string, overwrite: boolean): Promise<void>;
  public abstract async loadGraph(filePath: string): Promise<Graph>;
}
