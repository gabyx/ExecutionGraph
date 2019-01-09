import { Point, diff, norm2, normalize, norm } from "../model/Point";

export interface Node {
    readonly position: Point;
}

export interface Edge {
    readonly from: Node;
    readonly to: Node;
}

export interface Graph {
    readonly nodes: Node[];
    readonly edges: Edge[];
}

export class ForceDirectedGraphLayout {

    private readonly k: number;
    private readonly k2: number;

    private edgeMap: Map<Node, Set<Node>> = new Map();
    private noEdgeMap: Map<Node, Set<Node>> = new Map();

    constructor(
        private graph: Graph,
        private optimalDistance: number = 200) {


    }

    private initEdgeMap() {
        for(const node of this.graph.nodes) {
            const noEdges = new Set([...this.graph.nodes]);
            noEdges.delete(node);
            this.noEdgeMap.set(node, noEdges);
        }
        for(const edge of this.graph.edges) {
            if(this.edgeMap.has(edge.from)) {
                this.edgeMap.get(edge.from).add(edge.to);
            } else {
                this.edgeMap.set(edge.from, new Set([edge.to]));
            }
            this.noEdgeMap.get(edge.from).delete(edge.to);

            if(this.edgeMap.has(edge.to)) {
                this.edgeMap.get(edge.to).add(edge.from);
            } else {
                this.edgeMap.set(edge.to, new Set([edge.from]));
            }
            this.noEdgeMap.get(edge.to).delete(edge.from);
        }
    }

    public execute() {
        this.initEdgeMap();
        this.calculateSpringModel();
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
            for(const node of this.graph.nodes) {
                const nodeForce = {x: 0, y: 0};
                if(this.edgeMap.has(node)) {
                    for(const connectedNode of Array.from(this.edgeMap.get(node))) {
                        const fEdge = this.calculateSpringForce(node.position, connectedNode.position);
                        nodeForce.x += fEdge.x;
                        nodeForce.y += fEdge.y;
                    }
                }
                if (this.noEdgeMap.has(node)) {
                    for (const disconnectedNode of Array.from(this.noEdgeMap.get(node))) {
                        const fEdge = this.calculateSpringForce(node.position, disconnectedNode.position);
                        nodeForce.x += fEdge.x;
                        nodeForce.y += fEdge.y;
                    }
                }

                const f2 = norm(nodeForce);
                const f = Math.sqrt(f2);

                const adjustementX = step * nodeForce.x / f;
                const adjustementY = step * nodeForce.y / f;
                positionAdjustments += norm({x: adjustementX, y: adjustementY });
                node.position.x -= adjustementX;
                node.position.y -= adjustementY;
                energy += f2;
            }

            step *= 0.9;
        }
    }

    private calculateSpringForce(a: Point, b: Point): Point {
        const d = diff(a, b);
        const length = norm(d);

        const f = length - this.optimalDistance;
        return {
            x: f * d.x / length,
            y: f * d.y / length
        };
    }

    private calculateSpringElectriclForce(node: Node): Point {
        const force = { x: 0, y: 0 };

        for (const noEdge of this.noEdgeMap.get(node)) {
            const fR = this.calculateRepulsiveForce(node.position, noEdge.position);
            force.x += fR.x;
            force.y += fR.y;
        }
        for (const edge of this.edgeMap.get(node)) {
            const fA = this.calculateAttractiveForce(node.position, edge.position);
            force.x += fA.x;
            force.y += fA.y;
        }

        return force;
    }

    private calculateAttractiveForce(a: Point, b: Point): Point {
        const d: Point = {
            x: a.x - b.x,
            y: a.y - b.y
        };
        const d2 = d.x * d.x + d.y * d.y;

        const factor = -(d2 / this.k);

        return {
            x: d.x * factor,
            y: d.y * factor
        };
    }

    private calculateRepulsiveForce(a: Point, b: Point): Point {
        const d: Point = {
            x: a.x - b.x,
            y: a.y - b.y
        };
        const d2 = d.x * d.x + d.y * d.y;
        const d1 = Math.sqrt(d2);

        const factor = (this.k2 / d1);

        return {
            x: d.x * factor,
            y: d.y * factor
        };
    }

}