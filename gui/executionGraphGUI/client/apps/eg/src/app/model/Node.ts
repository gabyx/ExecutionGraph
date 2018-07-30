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

import { Socket } from "./Socket";

export type UIProps = {
  x: number,
  y: number
}

export class Node {
  constructor(
    public readonly id: string,
    public readonly name: string,
    public readonly inputs: Socket[],
    public readonly outputs: Socket[],
    public readonly uiProps: UIProps = {x: 0, y: 0})
  {}
}
