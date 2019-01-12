import { Id } from '@eg/common';
import { Connection, ConnectionId } from './Connection';
import { Node, NodeId } from './Node';

export interface NodeMap {
  [id: string]: Node;
}

export interface ConnectionMap {
  [id: string]: Connection;
}

export interface Graph {
  readonly id: Id;
  readonly name: string;
  readonly typeId: Id;
  readonly connections: ConnectionMap;
  readonly nodes: NodeMap;
}
