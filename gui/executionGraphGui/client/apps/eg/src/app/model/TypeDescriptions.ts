// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sun Aug 19 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

import { Id } from '@eg/common';

export class SocketTypeDescription {
  public readonly type: string;
  public readonly name: string;
  public readonly description?: string;
}

export class NodeTypeDescription {
  public readonly type: string;
  public readonly name: string;

  public readonly inSocketNames: string[];
  public readonly outSocketNames: string[];
  public readonly description?: string;
}

export class GraphTypeDescription {
  public readonly id: Id;
  public readonly name: string;
  public readonly socketTypeDescriptions: SocketTypeDescription[];
  public readonly nodeTypeDescriptions: NodeTypeDescription[];
  public readonly description: string;
}
