import { Point } from "../model/Point";

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

    private readonly k2: number;

    constructor(
        private graph: Graph,
        private k: number = 1.0) {
            this.k2 = k * k;
    }

    private calculateAttractiveForce(a: Point, b: Point): Point {
        const d: Point = {
            x: a.x - b.x,
            y: a.y - b.y
        };
        const d2 = d.x * d.x + d.y * d.y;
        const d1 = Math.sqrt(d2);

        const factor = -(d2 / this.k) / d1;

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

        const factor = (this.k2 / d2) / d1;

        return {
            x: d.x * factor,
            y: d.y * factor
        };
    }
}