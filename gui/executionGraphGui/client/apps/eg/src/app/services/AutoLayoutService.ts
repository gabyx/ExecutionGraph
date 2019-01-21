import { Injectable } from '@angular/core';
import { Graph, NodeId, Node as ModelNode } from '../model';
import {
  ILayoutEngine,
  EngineInput,
  MassSpringLayoutConfig,
  EngineOutput,
  NodeCreator,
  EdgeCreator,
  GraphConverter,
  MassSpringLayoutEngine
} from '@eg/graph';
import { AppState } from '../+state/reducers/app.reducers';
import { Store } from '@ngrx/store';
import { LoggerFactory, ILogger } from '@eg/logger';
import { MoveNodes, Moves } from '../+state/actions';
import { map, catchError } from 'rxjs/operators';
import { throwError } from 'rxjs';
import { Vector2 } from '@eg/common';

/**
 * This function represents the body-link-type agnostic converter
 * which is used in the underlying Engine to convert our graph.
 */
async function convertGraph<Node, Edge>(
  graph: Graph,
  positionMap: Map<NodeId, Vector2>,
  createNode: (id: NodeId, pos: Vector2, opaqueData: any) => Node,
  createEdge: (b1: Node, b2: Node) => Edge
): Promise<EngineInput<Node, Edge>> {
  const nodes: Node[] = [];

  const nodeMap = new Map<NodeId, Node>();

  // create bodies
  positionMap.forEach((pos, id) => {
    const b = createNode(id, pos, graph.nodes[id.toString()]);
    nodeMap.set(id, b);
    nodes.push(b);
  });

  // create links
  const edges: Edge[] = Object.values(graph.connections).map(connection =>
    createEdge(nodeMap.get(connection.inputSocket.parent.id), nodeMap.get(connection.outputSocket.parent.id))
  );

  return { nodes: nodes, edges: edges };
}

@Injectable()
export class AutoLayoutService {
  private readonly logger: ILogger;

  constructor(private store: Store<AppState>, private loggerFactory: LoggerFactory) {
    this.logger = loggerFactory.create('AutoLayoutService');
  }

  public async layoutGraphSpringSystem(graph: Graph, config?: MassSpringLayoutConfig): Promise<void> {
    if (!config) {
      config = new MassSpringLayoutConfig();
    }
    const engine = new MassSpringLayoutEngine(config, this.loggerFactory);
    return this.layoutGraph(graph, engine);
  }

  private async layoutGraph(graph: Graph, engine: ILayoutEngine): Promise<void> {
    this.logger.debug('Layouting graph...');

    // A copy of all positions (dont change the state!)
    const positionMap = new Map<NodeId, Vector2>();
    Object.values(graph.nodes).map(node => {
      positionMap.set(node.id, node.uiProps.position.copy());
    });

    // Create the converter for the engine
    const converter: GraphConverter = <Node, Edge>(a: NodeCreator<Node>, b: EdgeCreator<Node, Edge>) =>
      convertGraph(graph, positionMap, a, b);

    // Create the engine (by the strategy) ->
    // run it and dispatch the results in the store.
    return engine
      .run(converter)
      .pipe(
        map((out: EngineOutput) => {
          const s: Moves = [];
          out.forEach(res => {
            s.push({ pos: res.pos, node: <ModelNode>res.opaqueData });
          });
          this.store.dispatch(new MoveNodes(s));
        }),
        map(() => undefined),
        catchError((e, catched) => {
          this.logger.error(`Error: ${e}`);
          return throwError(e);
        })
      )
      .toPromise();
  }
}
