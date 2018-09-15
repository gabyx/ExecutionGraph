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
   * @returns {(Graph | null)}
   * @memberof AppState
   */
  public getGraph(id: Id): Graph | null {
    let s = id.toString();
    return (s in this.graphs) ? this.graphs[s] : null
  }

  /**
   * Get the graph with the id `id` if it exists.
   *
   * @param {Id} id
   * @returns {(Graph | null)}
   * @memberof AppState
   */
  public getGraphDescription(id: Id): GraphTypeDescription | null {
    let s = id.toString();
    return (s in this.graphDescriptions) ? this.graphDescriptions[s] : null
  }

  public getSelectedGraph()
  {
    return this.getGraph(this.selectedGraphId);
  }
  public getSelectedGraphDescription()
  {
    return this.getGraphDescription(this.selectedGraphId);
  }

}
