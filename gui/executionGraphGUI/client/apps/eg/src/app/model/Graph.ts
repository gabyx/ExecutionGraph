import { Id } from "@eg/common";
import { Connection } from "./Connection";
import { Node } from "./Node";

export class Graph {
    constructor(
        public readonly id: Id,
        public readonly nodes: Node[],
        public readonly connections: Connection[]
    ) {}

}