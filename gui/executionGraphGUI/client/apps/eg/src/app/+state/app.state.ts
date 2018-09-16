// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sat Sep 15 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

import { Id } from "@eg/common";
import { Graph, GraphTypeDescription } from '../model';
import { isDefined } from '@eg/common'

export class AppState {
  /** Map of graphs:
   * id -> graph
   * @todo can we make this Map()...?
  */
  public graphs: { [id: string]: Graph; } = {};

  /** Map of graph types descriptions:
   * type id -> graph description
   */
  public graphDescriptions: { [type: string]: GraphTypeDescription } = {};

  public selectedGraphId?: Id; // Which Graph has been selected
  public loaded: boolean = false; // Has the AppState been loaded
  public error?: any; // Last none error (if any)

  /**
   * Get the graph with the id `id` if it exists.
   *
   * @param {Id} id
   * @returns {(Graph | undefined)}
   * @memberof AppState
   */
  public getGraph(id: Id): Graph | undefined {
    let s = id.toString();
    return (s in this.graphs) ? this.graphs[s] : undefined
  }

  /**
   * Get the graph with the id `id` if it exists.
   *
   * @param {Id} id
   * @returns {(Graph | undefined)}
   * @memberof AppState
   */
  public getGraphDescription(id: Id): GraphTypeDescription | undefined {
    let s = id.toString();
    return (s in this.graphDescriptions) ? this.graphDescriptions[s] : undefined
  }

  public getSelectedGraph() {
    return isDefined(this.selectedGraphId) ? this.getGraph(this.selectedGraphId) : undefined;
  }
  public getSelectedGraphDescription() {
    return isDefined(this.selectedGraphId) ? this.getGraphDescription(this.selectedGraphId) : undefined;
  }

}
