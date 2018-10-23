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

/**
 * A class for defining a unique id for a `Connection`.
 *
 * @export
 * @class ConnectionId
 */
export class ConnectionId {
  public static create(outSocketId: SocketId, inSocketId: SocketId): ConnectionId {
    return new ConnectionId(`${outSocketId.string}-to-${inSocketId.string}`);
  }

  constructor(public readonly idString) {}

  public equal(id: ConnectionId) {
    return this.idString === id.idString;
  }
}

enum Validity {
  Valid = 0,
  Invalid = 1,
  InOutMismatch = 1 << 1,
  OneLengthCycle = 1 << 2,
  TypeMismatch = 1 << 3
}

export class UIProps {
}

/**
 * Modelclass for a connection
 * @todo see -> SocketLink.fbs
 * @export
 * @class Connection
 */
export class Connection {
  public static readonly Validity = Validity;
  public readonly id: ConnectionId;
  public uiProps: UIProps = new UIProps();

  constructor(
    public outputSocket: OutputSocket,
    public inputSocket: InputSocket,
    public isWriteLink: boolean = true,
    validate: boolean = true
  ) {
    const invalid = validate ? this.isInvalid() : Connection.Validity.Valid /*just do it!*/;
    if (invalid) {
      throw new Error(`Cannot construct an invalid connection: ${Connection.getValidationError(invalid)}`);
    }
    this.id = ConnectionId.create(outputSocket.id, inputSocket.id);
  }

  /**
   * Gets the string identifier for this connection.
   *
   * @readonly
   * @memberof Connection
   */
  public get idString(): string {
    return this.id.idString;
  }

  public isInvalid(): Validity {
    return Connection.isInvalid(this.outputSocket, this.inputSocket);
  }

  /**
   * Check if a connection is valid.
   *
   * @static
   * @param {Socket} source
   * @param {Socket} target
   * @returns {Validity}
   * @memberof Connection
   */
  public static isInvalid(source: Socket, target: Socket): Validity {
    let result = Validity.Valid;
    /** input,output or vice versa */
    result |= source.kind !== target.kind ? Validity.Valid : Validity.Invalid | Validity.InOutMismatch;
    /** no 1-length-cycles, more elaborate cycle-detection in the backend */
    result |= source.parent !== target.parent ? Validity.Valid : Validity.Invalid | Validity.OneLengthCycle;
    /** correct type */
    result |= source.type.equals(target.type) ? Validity.Valid : Validity.Invalid | Validity.TypeMismatch;
    return result;
  }

  /**
   * Get the error description for the invalidity of the connection (if any).
   *
   * @static
   * @param {Validity} error
   * @returns {string}
   * @memberof Connection
   */
  public static getValidationError(error: Validity): string {
    if (error & Validity.Invalid) {
      let err = `${Validity[Validity.Invalid]}: `;
      err += error & Validity.OneLengthCycle ? `-${Validity[Validity.OneLengthCycle]} ` : '';
      err += error & Validity.InOutMismatch ? `-${Validity[Validity.InOutMismatch]} ` : '';
      err += error & Validity.TypeMismatch ? `-${Validity[Validity.TypeMismatch]} ` : '';
      return err;
    }
    return Validity[Validity.Valid];
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
}
