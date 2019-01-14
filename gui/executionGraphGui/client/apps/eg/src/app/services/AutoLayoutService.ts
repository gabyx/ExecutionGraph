import { Injectable, Inject } from '@angular/core';
import { Graph, NodeId } from '../model';
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
import { MoveNode } from '../+state/actions';

/**
 * This function represents the body-link-type agnostic converter
 * which is used in the underlying Engine to convert our graph.
 */
async function convertGraph<Body, Link>(
  graph: Graph,
  positionMap: Map<NodeId, Point>,
  createBody: (id: NodeId, pos: Point) => Body,
  createLink: (b1: Body, b2: Body) => Link
): Promise<EngineInput<Body, Link>> {
  const bodies = new Map<NodeId, Body>();
  positionMap.forEach((pos, id) => bodies.set(id, createBody(id, pos)));

  const links: Link[] = Object.values(graph.connections).map(connection =>
    createLink(bodies.get(connection.inputSocket.parent.id), bodies.get(connection.outputSocket.parent.id))
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

  public async layoutGraph(graph: Graph, config: ILayoutStrategy = new MassSpringLayoutStrategy()): Promise<void> {
    this.logger.debug('Layouting graph...');

    // A copy of all positions
    const positionMap = new Map<NodeId, Point>();
    Object.values(graph.nodes).map(node => {
      positionMap.set(node.id, node.uiProps.position.copy());
    });

    // Create the converter for the engine
    const converter: GraphConverter = <Body, Link>(a: BodyCreator<Body>, b: LinkCreator<Body, Link>) =>
      convertGraph(graph, positionMap, a, b);

    // Create the engine (by the strategy) ->
    // run it and dispatch the results in the store.
    this.engineCreator(config)
      .dispatchRun(converter)
      .catch((e: any) => {
        this.logger.error(`Error: '${e}'`);
        throw e;
      })
      .then((out: EngineOutput) => {
        out.forEach(res => this.store.dispatch(new MoveNode(graph.nodes[res.id.toString()], res.pos)));
      });
  }
}
