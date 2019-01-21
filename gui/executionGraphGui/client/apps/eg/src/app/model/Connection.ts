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

import { OutputSocket, InputSocket, SocketId, Socket } from './Socket';

//tslint:disable:no-bitwise

export type ConnectionId = string;
/**
 * A class for defining a unique id for a `Connection`.
 *
 * @export
 */
export function createConnectionId(outSocketId: string, inSocketId: string): ConnectionId {
  return `n${outSocketId}-to-n${inSocketId}`;
}

enum Invalidity {
  Valid = 0,
  Invalid = 1,
  InOutMismatch = 1 << 1,
  OneLengthCycle = 1 << 2,
  TypeMismatch = 1 << 3
}

export class UIProps {}

/**
 * Modelclass for a connection
 * @export
 * @class Connection
 */
export class Connection {
  public static readonly Invalidity = Invalidity;
  public readonly _id: ConnectionId;
  public uiProps: UIProps = new UIProps();

  /**
   * Check if a connection is valid.
   *
   * @static
   * @param {Socket} source
   * @param {Socket} target
   * @returns {Invalidity}
   * @memberof Connection
   */
  public static isInvalid(source: Socket, target: Socket): Invalidity {
    let result = Invalidity.Valid;
    /** input,output or vice versa */
    result |= source.kind !== target.kind ? Invalidity.Valid : Invalidity.Invalid | Invalidity.InOutMismatch;
    /** no 1-length-cycles, more elaborate cycle-detection in the backend */
    result |= source.parent !== target.parent ? Invalidity.Valid : Invalidity.Invalid | Invalidity.OneLengthCycle;
    /** correct type */
    result |= source.typeIndex.equals(target.typeIndex)
      ? Invalidity.Valid
      : Invalidity.Invalid | Invalidity.TypeMismatch;
    return result;
  }

  /**
   * Create a connection.
   *
   * @static
   * @param {Socket} source
   * @param {Socket} target
   * @returns
   * @memberof Connection
   */
  public static create(source: Socket, target: Socket, validate: boolean = true) {
    if (Socket.isOutputSocket(source) && Socket.isInputSocket(target)) {
      // Make a Write-Link
      return new Connection(source, target, true, validate);
    } else if (Socket.isInputSocket(source) && Socket.isOutputSocket(target)) {
      // Make a Get-Link
      return new Connection(target, source, false, validate);
    } else {
      throw new Error(`Programming error: Connection cannot be made! ${source} <-> ${target}`);
    }
  }

  /**
   * Get the error description for the invalidity of the connection (if any).
   *
   * @static
   * @param {Invalidity} error
   * @returns {string}
   * @memberof Connection
   */
  public static getValidationErrors(error: Invalidity): string[] {
    const errorMessages = [];
    for (const validityFlag of [Invalidity.OneLengthCycle, Invalidity.InOutMismatch, Invalidity.TypeMismatch]) {
      if (error & validityFlag) {
        errorMessages.push(`${Invalidity[validityFlag]}`);
      }
    }
    return errorMessages;
  }

  constructor(
    public outputSocket: OutputSocket,
    public inputSocket: InputSocket,
    public isWriteLink: boolean = true,
    validate: boolean = true
  ) {
    const invalid = validate ? this.isInvalid() : Connection.Invalidity.Valid /*just do it!*/;
    if (invalid) {
      throw new Error(`Cannot construct an invalid connection: ${Connection.getValidationErrors(invalid).join(',')}`);
    }
    this._id = createConnectionId(outputSocket.id, inputSocket.id);
  }

  /**
   * Gets the string identifier for this connection.
   *
   * @readonly
   * @memberof Connection
   */
  public get id(): string {
    return this._id;
  }

  public isInvalid(): Invalidity {
    return Connection.isInvalid(this.outputSocket, this.inputSocket);
  }
}

//tslint:enable:no-bitwise
