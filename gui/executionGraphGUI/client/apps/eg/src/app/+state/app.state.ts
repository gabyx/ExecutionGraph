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
  public graphs: Map<Id, Graph> = new Map<Id, Graph>();

  /** Map of graph types descriptions:
   * type id -> graph description
   */
  public graphDescriptions: Map<Id, GraphTypeDescription> = new Map<Id, GraphTypeDescription>();

  public selectedGraphId?: Id; // Which Graph has been selected
  public loaded: boolean = false; // Has the AppState been loaded
  public error?: any; // Last none error (if any)

  public addGraph(graph: Graph) {
    if (this.graphs.has(graph.id)) {
      throw `Graph '${graph.id.toString()}' already added!`;
    }
    this.graphs.set(graph.id, graph);
  }

  public removeGraph(id: Id) {
    if (!this.graphs.delete(id))
    {
      throw `Graph '${id.toString()}' not existing!`;
    }
  }

  public addGraphDescription(graphDesc: GraphTypeDescription) {
    if (this.graphDescriptions.has(graphDesc.id)) {
      throw `GraphDescription '${graphDesc.id.toString()}' already added!`;
    }
    this.graphDescriptions.set(graphDesc.id, graphDesc);
  }

  /**
   * Get the graph with the id `id` if it exists.
   *
   * @param {Id} id
   * @returns {(Graph | undefined)}
   * @memberof AppState
   */
  public graph(id: Id): Graph | undefined {
    return this.graphs.get(id);
  }

  /**
   * Get the graph with the id `id` if it exists.
   *
   * @param {Id} id
   * @returns {(Graph | undefined)}
   * @memberof AppState
   */
  public graphDescription(id: Id): GraphTypeDescription | undefined {
    return this.graphDescriptions.get(id);
  }

  public get selectedGraph(): Graph | undefined {
    return isDefined(this.selectedGraphId) ? this.graph(this.selectedGraphId) : undefined;
  }
  public get selectedGraphDescription(): GraphTypeDescription | undefined {
    return isDefined(this.selectedGraphId) ? this.graphDescription(this.selectedGraphId) : undefined;
  }

}
