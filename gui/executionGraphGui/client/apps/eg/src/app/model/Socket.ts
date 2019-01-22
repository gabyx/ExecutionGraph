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

import * as Long from 'long';
import { Guid } from 'guid-typescript';
import { Node, NodeId } from './Node';
import { isDefined } from '@eg/common';
import { getParentInjectorIndex } from '@angular/core/src/render3/util';

export class UIProps {
  public name: string = '';
}

export type SocketType = 'input' | 'output';
export type SocketIndex = Long;
export type SocketTypeIndex = Long;
export type SocketId = string;

export interface Socket {
  readonly kind: SocketType;
  readonly type?: string;
  readonly typeIndex: SocketTypeIndex;

  readonly index: SocketIndex;
  readonly parentId: NodeId;
  readonly uiProps: UIProps;
  readonly id: SocketId;
}

export type OutputSocket = Socket;
export type InputSocket = Socket;

/** Type guard for InputSocket */
export function isInputSocket(socket: Socket): socket is InputSocket {
  return socket.kind === 'input';
}

/** Type guard for OutputSocket */
export function isOutputSocket(socket: Socket): socket is OutputSocket {
  return socket.kind === 'output';
}

export function createId(): SocketId {
  return Guid.create().toString();
}

/*
 * Creator function for input/output sockets.
 */
export function createSocket(
  kind: SocketType,
  typeIndex: SocketTypeIndex,
  index: SocketIndex,
  parentId: NodeId,
  type: string | undefined | null
): InputSocket | OutputSocket {
  let uiName: string;

  switch (kind) {
    case 'input': {
      uiName = `in-${index}`;
      break;
    }
    case 'output': {
      uiName = `in-${index}`;
      break;
    }
  }

  return {
    id: createId(),
    kind: kind,
    typeIndex: typeIndex,
    type: isDefined(type) ? type : undefined,
    index: index,
    parentId: parentId,
    uiProps: { name: uiName }
  };
}
