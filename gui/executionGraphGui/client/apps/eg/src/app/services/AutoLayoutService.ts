import { Injectable, Inject } from '@angular/core';
import { Graph, NodeId, Node } from '../model';
import {
  Point,
  ILayoutStrategy,
  ILayoutEngine,
  EngineInput,
  MassSpringLayoutStrategy,
  EngineOutput,
  BodyCreator,
  LinkCreator,
  GraphConverter
} from '@eg/graph';
import { AppState } from '../+state/reducers/app.reducers';
import { Store } from '@ngrx/store';
import { LoggerFactory, ILogger } from '@eg/logger/src';
import { MoveNode, MoveNodes, Moves } from '../+state/actions';
import { map, catchError } from 'rxjs/operators';
import { throwError } from 'rxjs';

/**
 * This function represents the body-link-type agnostic converter
 * which is used in the underlying Engine to convert our graph.
 */
async function convertGraph<Body, Link>(
  graph: Graph,
  positionMap: Map<NodeId, Point>,
  createBody: (id: NodeId, pos: Point, opaqueData: any) => Body,
  createLink: (b1: Body, b2: Body) => Link
): Promise<EngineInput<Body, Link>> {
  const bodies: Body[] = [];

  const bodyMap = new Map<NodeId, Body>();

  // create bodies
  positionMap.forEach((pos, id) => {
    const b = createBody(id, pos, graph.nodes[id.toString()]);
    bodyMap.set(id, b);
    bodies.push(b);
  });

  // create links
  const links: Link[] = Object.values(graph.connections).map(connection =>
    createLink(bodyMap.get(connection.inputSocket.parent.id), bodyMap.get(connection.outputSocket.parent.id))
  );

  return { bodies: bodies, links: links };
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
    const converter: GraphConverter = <Body, Link>(a: BodyCreator<Body>, b: LinkCreator<Body, Link>) =>
      convertGraph(graph, positionMap, a, b);

    // Create the engine (by the strategy) ->
    // run it and dispatch the results in the store.
    return this.engineCreator(config)
      .run(converter)
      .pipe(
        map((out: EngineOutput) => {
          const s: Moves = [];
          out.forEach(res => {
            s.push({ pos: res.pos, node: <Node>res.opaqueData });
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
