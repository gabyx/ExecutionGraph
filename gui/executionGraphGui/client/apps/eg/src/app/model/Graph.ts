import { Connection } from './Connection';
import { Node } from './Node';

export interface NodeMap {
  [id: string]: Node;
}

export interface ConnectionMap {
  [id: string]: Connection;
}

export type GraphId = string;
export type GraphTypeId = string;

export interface Graph {
  readonly id: GraphId;
  readonly name: string;
  readonly typeId: GraphTypeId;
  readonly connections: ConnectionMap;
  readonly nodes: NodeMap;
}
