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

import { InputSocket, OutputSocket, Socket } from './Socket';
import * as fromSocket from './Socket';
import { create } from 'domain';

//tslint:disable:no-bitwise

/**
 * A class for defining a unique id for a `Connection`.
 */
export type ConnectionId = string;
export function createId(outSocketId: string, inSocketId: string): ConnectionId {
  return `n${outSocketId}-to-n${inSocketId}`;
}

export enum Invalidity {
  Valid = 0,
  Invalid = 1,
  InOutMismatch = 1 << 1,
  OneLengthCycle = 1 << 2,
  TypeMismatch = 1 << 3
}

export class UIProps {}

/**
 * Modelclass for a connection
 */
export interface Connection {
  readonly id: ConnectionId;
  readonly outputSocket: OutputSocket;
  readonly inputSocket: InputSocket;
  readonly isWriteLink: boolean;
  readonly uiProps: UIProps;
}

/**
 * Check if a connection is valid.
 */
export function isInvalid(source: Socket, target: Socket): Invalidity {
  let result = Invalidity.Valid;
  /** input,output or vice versa */
  result |= source.kind !== target.kind ? Invalidity.Valid : Invalidity.Invalid | Invalidity.InOutMismatch;
  /** no 1-length-cycles, more elaborate cycle-detection in the backend */
  result |= source.parentId !== target.parentId ? Invalidity.Valid : Invalidity.Invalid | Invalidity.OneLengthCycle;
  /** correct type */
  result |= source.typeIndex === target.typeIndex ? Invalidity.Valid : Invalidity.Invalid | Invalidity.TypeMismatch;
  return result;
}

/**
 * Create a valid connection.
 */
export function createValidConnection(source: Socket, target: Socket): Connection {
  const invalid = isInvalid(source, target);
  if (invalid) {
    throw new Error(`Cannot construct an invalid connection: ${getValidationErrors(invalid).join(',')}`);
  }
  return createConnection(source, target);
}

/**
 * Create a connection (unchecked: especially for dummy connections in the gui).
 */
export function createConnection(source: Socket, target: Socket): Connection {
  let isWriteLink = true;
  let inS: InputSocket;
  let outS: OutputSocket;
  if (fromSocket.isOutputSocket(source) && fromSocket.isInputSocket(target)) {
    outS = source;
    inS = target;
  } else if (fromSocket.isInputSocket(source) && fromSocket.isOutputSocket(target)) {
    outS = target;
    inS = source;
    // Make a Get-Link
    isWriteLink = false;
  } else {
    throw new Error(`Programming error: Connection cannot be made! ${source} <-> ${target}`);
  }

  const id = createId(source.id, target.id);
  return { id: id, outputSocket: outS, inputSocket: inS, isWriteLink: isWriteLink, uiProps: {} };
}

/**
 * Get the error description for the invalidity of the connection (if any).
 */
export function getValidationErrors(error: Invalidity): string[] {
  const errorMessages = [];
  for (const validityFlag of [Invalidity.OneLengthCycle, Invalidity.InOutMismatch, Invalidity.TypeMismatch]) {
    if (error & validityFlag) {
      errorMessages.push(`${Invalidity[validityFlag]}`);
    }
  }
  return errorMessages;
}

//tslint:enable:no-bitwise
