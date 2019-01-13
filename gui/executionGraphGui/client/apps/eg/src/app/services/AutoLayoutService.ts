import { Injectable, Inject } from '@angular/core';
import { Graph } from '../model';
import {
  LayoutStrategys,
  ILayoutStrategy,
  Graph as LayoutGraph,
  Node as LayoutNode,
  Edge as LayoutEdge,
  ForceDirectedGraphLayoutStrategy,
  ILayoutEngine
} from '@eg/graph';
import { AppState } from '../+state/reducers/app.reducers';
import { Store } from '@ngrx/store';
import { MoveNode } from '../+state/actions';
import { LoggerFactory, ILogger } from '@eg/logger/src';

@Injectable()
export class AutoLayoutService {
  private readonly logger: ILogger;
  private readonly engineCreator: (config: ILayoutStrategy) => ILayoutEngine;

  constructor(private store: Store<AppState>, loggerFactory: LoggerFactory) {
    this.logger = loggerFactory.create('AutoLayoutService');
    this.engineCreator = (config: ILayoutStrategy) => config.createEngine(loggerFactory);
  }

  public async layoutGraph(
    graph: Graph,
    config: ILayoutStrategy = new ForceDirectedGraphLayoutStrategy()
  ): Promise<void> {
    this.logger.debug('Layouting graph...');

    const conv = await this.convertGraph(graph);

    this.engineCreator(config)
      .dispatchRun(config, conv.layoutGraph)
      .catch((e: any) => {
        this.logger.error(`Error: '${e}'`);
        throw e;
      })
      .then(() => {
        for (const nodeId of Object.keys(conv.nodeMap)) {
          this.store.dispatch(new MoveNode(graph.nodes[nodeId], { ...conv.nodeMap[nodeId].position }));
        }
      });
  }

  private async convertGraph(graph: Graph): Promise<{ layoutGraph: LayoutGraph; nodeMap: any }> {
    // Convert graph to a layout graph
    const nodeMap = {};
    for (const nodeId of Object.keys(graph.nodes)) {
      nodeMap[graph.nodes[nodeId].id.toString()] = {
        position: { ...graph.nodes[nodeId].uiProps.position }
      };
    }
    const nodes: LayoutNode[] = Object.keys(nodeMap).map(id => nodeMap[id]);

    const edges: LayoutEdge[] = Object.keys(graph.connections)
      .map(id => graph.connections[id])
      .map(connection => ({
        from: nodeMap[connection.inputSocket.parent.id.toString()],
        to: nodeMap[connection.outputSocket.parent.id.toString()]
      }));

    return { layoutGraph: { nodes, edges }, nodeMap: nodeMap };
  }
}
