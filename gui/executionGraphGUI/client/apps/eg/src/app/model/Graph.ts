import { Id } from "@eg/common";
import { Connection } from "./Connection";
import { Node } from "./Node";

export class Graph {
  constructor(
    private readonly _id: Id,
    private readonly _type: Id,
    private _nodes: Node[] = [],
    private _connections: Connection[] = [],
  ) { }

  public get id(): Id { return this._id; }
  public get type(): Id { return this._type; }
  public get nodes(): Node[] { return this._nodes; }
  public get connections(): Connection[] { return this._connections; }
}
