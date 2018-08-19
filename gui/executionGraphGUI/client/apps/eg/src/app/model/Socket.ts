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

import { SocketIndex } from './SocketIndex'
import { NodeId } from './NodeId'

/**
 * Modelclass for a Socket on a node.
 *
 * @export
 * @class Socket
 */
export class Socket {
  constructor(
    public readonly nodeId: NodeId,
    public readonly type: string,
    public readonly name: string,
    public readonly index: SocketIndex) { }
}

export class InputSocket extends Socket { }
export class OutputSocket extends Socket { }
