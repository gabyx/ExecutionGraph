import { Id } from '@eg/common';
import { Connection, ConnectionId } from './Connection';
import { Node, NodeId } from './Node';

export class Graph {
  private nodesMap = new Map<NodeId, Node>();
  private connectionsMap = new Map<ConnectionId, Connection>();
  constructor(
    private readonly _id: Id,
    private readonly _type: Id,
    nodes: Node[] = [],
    connections: Connection[] = []
  ) {
    nodes.forEach((n: Node) => {
      this.addNode(n);
    });
    connections.forEach((c: Connection) => {
      this.addConnection(c);
    });
  }

  public get id(): Id {
    return this._id;
  }
  public get type(): Id {
    return this._type;
  }
  public get nodes() {
    return this.nodesMap.values();
  }
  public get connections() {
    return this.connectionsMap.values();
  }

  // @todo cmonspqr: These methods should be removed. This is the responsibility of the reducers

  public addNode(n: Node) {
    if (this.nodesMap.has(n.id)) {
      throw Error(`Node '${n.id}' already in graph!`);
    }
    this.nodesMap.set(n.id, n);
  }

  public removeNode(id: NodeId) {
    if (!this.nodesMap.delete(id)) {
      throw Error(`Node '${id}' is not in graph!`);
    }
  }

  public addConnection(c: Connection) {
    if (this.connectionsMap.has(c.id)) {
      throw new Error(`Node '${c.id.toString()}' already in graph!`);
    }
    this.connectionsMap.set(c.id, c);
  }

  public removeConnection(id: ConnectionId) {
    if (!this.connectionsMap.has(id)) {
      throw new Error(`Connection '${id}' not in graph!`);
    }
    this.connectionsMap.delete(id);
  }

  public node(id: NodeId) {
    return this.nodesMap.get(id);
  }

  public connection(id: ConnectionId) {
    return this.connectionsMap.get(id);
  }
}
