import { Injectable, Inject } from '@angular/core';
import { Graph, NodeId, Node as ModelNode } from '../model';
import {
  Point,
  ILayoutStrategy,
  ILayoutEngine,
  EngineInput,
  MassSpringLayoutStrategy,
  EngineOutput,
  NodeCreator,
  EdgeCreator,
  GraphConverter
} from '@eg/graph';
import { AppState } from '../+state/reducers/app.reducers';
import { Store } from '@ngrx/store';
import { LoggerFactory, ILogger } from '@eg/logger';
import { MoveNode, MoveNodes, Moves } from '../+state/actions';
import { map, catchError } from 'rxjs/operators';
import { throwError } from 'rxjs';

/**
 * This function represents the body-link-type agnostic converter
 * which is used in the underlying Engine to convert our graph.
 */
async function convertGraph<Node, Edge>(
  graph: Graph,
  positionMap: Map<NodeId, Point>,
  createNode: (id: NodeId, pos: Point, opaqueData: any) => Node,
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
  private readonly engineCreator: (config: ILayoutStrategy) => ILayoutEngine;

  constructor(private store: Store<AppState>, loggerFactory: LoggerFactory) {
    this.logger = loggerFactory.create('AutoLayoutService');
    this.engineCreator = (config: ILayoutStrategy) => config.createEngine(loggerFactory);
  }

  public async layoutGraph(graph: Graph, config?: ILayoutStrategy): Promise<void> {
    this.logger.debug('Layouting graph...');

    config = config ? config : new MassSpringLayoutStrategy();

    // A copy of all positions (dont change the state!)
    const positionMap = new Map<NodeId, Point>();
    Object.values(graph.nodes).map(node => {
      positionMap.set(node.id, node.uiProps.position.copy());
    });

    // Create the converter for the engine
    const converter: GraphConverter = <Node, Edge>(a: NodeCreator<Node>, b: EdgeCreator<Node, Edge>) =>
      convertGraph(graph, positionMap, a, b);

    // Create the engine (by the strategy) ->
    // run it and dispatch the results in the store.
    return this.engineCreator(config)
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
