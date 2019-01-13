import { Point, diff, norm2, normalize, norm } from '../../model/Point';
import { ILayoutEngine, Graph, Node } from './ILayoutEngine';
import { LayoutStrategys, ILayoutStrategy } from './ILayoutStrategy';
import { LoggerFactory, ILogger } from '@eg/logger/src';

export class ForceDirectedGraphLayoutStrategy extends ILayoutStrategy {
  public static readonly strategy_ = LayoutStrategys.MassSpringSystemSimon;
  public readonly strategy = ForceDirectedGraphLayoutStrategy.strategy_;

  public optimalDistance: number = 300;

  public createEngine(loggerFactory: LoggerFactory): ILayoutEngine {
    return new ForceDirectedGraphLayout(loggerFactory);
  }
}

export class ForceDirectedGraphLayout extends ILayoutEngine {
  private readonly logger: ILogger;
  private config: ForceDirectedGraphLayoutStrategy;
  private graph: Graph;

  private readonly k: number;
  private readonly k2: number;

  private edgeMap: Map<Node, Set<Node>> = new Map();
  private noEdgeMap: Map<Node, Set<Node>> = new Map();

  constructor(loggerFactory: LoggerFactory) {
    super();
    this.logger = loggerFactory.create('ForceDirectedGraphLayout');
  }

  public isStrategyValid(s: LayoutStrategys) {
    return s === ForceDirectedGraphLayoutStrategy.strategy_;
  }

  public async run(config: ForceDirectedGraphLayoutStrategy, graph: Graph): Promise<void> {
    this.config = config;
    this.graph = graph;

    this.initEdgeMap();
    this.calculateSpringModel();
  }

  private static calculateSpringForce(a: Point, b: Point, distance: number): Point {
    const d = diff(a, b);
    const length = norm(d);

    const f = length - distance;
    return {
      x: (f * d.x) / length,
      y: (f * d.y) / length
    };
  }

  private static calculateAttractiveForce(a: Point, b: Point, k: number): Point {
    const d: Point = {
      x: a.x - b.x,
      y: a.y - b.y
    };
    const d2 = d.x * d.x + d.y * d.y;

    const factor = -(d2 / k);

    return {
      x: d.x * factor,
      y: d.y * factor
    };
  }

  private static calculateRepulsiveForce(a: Point, b: Point, k2: number): Point {
    const d: Point = {
      x: a.x - b.x,
      y: a.y - b.y
    };
    const d2 = d.x * d.x + d.y * d.y;
    const d1 = Math.sqrt(d2);

    const factor = k2 / d1;

    return {
      x: d.x * factor,
      y: d.y * factor
    };
  }

  private initEdgeMap() {
    for (const node of this.graph.nodes) {
      const noEdges = new Set([...this.graph.nodes]);
      noEdges.delete(node);
      this.noEdgeMap.set(node, noEdges);
    }
    for (const edge of this.graph.edges) {
      if (this.edgeMap.has(edge.from)) {
        this.edgeMap.get(edge.from).add(edge.to);
      } else {
        this.edgeMap.set(edge.from, new Set([edge.to]));
      }
      this.noEdgeMap.get(edge.from).delete(edge.to);

      if (this.edgeMap.has(edge.to)) {
        this.edgeMap.get(edge.to).add(edge.from);
      } else {
        this.edgeMap.set(edge.to, new Set([edge.from]));
      }
      this.noEdgeMap.get(edge.to).delete(edge.from);
    }
  }

  private calculateSpringModel() {
    const MIN_ADJUSTMENT = 5;

    let step = 10;
    let energy = Number.POSITIVE_INFINITY;
    let positionAdjustments = Number.POSITIVE_INFINITY;

    while (positionAdjustments >= MIN_ADJUSTMENT) {
      positionAdjustments = 0;
      const energy0 = energy;
      energy = 0;
      for (const node of this.graph.nodes) {
        const nodeForce = { x: 0, y: 0 };
        if (this.edgeMap.has(node)) {
          for (const connectedNode of Array.from(this.edgeMap.get(node))) {
            const fEdge = ForceDirectedGraphLayout.calculateSpringForce(
              node.position,
              connectedNode.position,
              this.config.optimalDistance
            );
            nodeForce.x += fEdge.x;
            nodeForce.y += fEdge.y;
          }
        }
        if (this.noEdgeMap.has(node)) {
          for (const disconnectedNode of Array.from(this.noEdgeMap.get(node))) {
            const fEdge = ForceDirectedGraphLayout.calculateSpringForce(
              node.position,
              disconnectedNode.position,
              2 * this.config.optimalDistance
            );
            nodeForce.x += fEdge.x;
            nodeForce.y += fEdge.y;
          }
        }
        // const nodeForce = this.calculateSpringElectricalForce(node);

        const f2 = norm(nodeForce);
        const f = Math.sqrt(f2);

        const adjustementX = (step * nodeForce.x) / f;
        const adjustementY = (step * nodeForce.y) / f;
        positionAdjustments += norm({ x: adjustementX, y: adjustementY });
        node.position.x -= adjustementX;
        node.position.y -= adjustementY;
        energy += f2;
      }

      step *= 0.9;
    }
  }

  private calculateSpringElectricalForce(node: Node): Point {
    const force = { x: 0, y: 0 };

    if (this.noEdgeMap.has(node)) {
      for (const noEdge of Array.from(this.noEdgeMap.get(node))) {
        const fR = ForceDirectedGraphLayout.calculateRepulsiveForce(node.position, noEdge.position, 1.2 ** 2);
        force.x += fR.x;
        force.y += fR.y;
      }
    }
    if (this.edgeMap.has(node)) {
      for (const edge of Array.from(this.edgeMap.get(node))) {
        const fA = ForceDirectedGraphLayout.calculateAttractiveForce(node.position, edge.position, 1.2);
        force.x += fA.x;
        force.y += fA.y;
      }
    }

    return force;
  }
}
