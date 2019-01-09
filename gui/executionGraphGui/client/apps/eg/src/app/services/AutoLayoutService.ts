import { Injectable } from "@angular/core";
import { Graph } from "../model";
import { ForceDirectedGraphLayout, Graph as LayoutGraph, Node as LayoutNode, Edge as LayoutEdge } from "@eg/graph";
import { AppState } from "../+state/reducers/app.reducers";
import { Store } from "@ngrx/store";
import { MoveNode } from "../+state/actions";

@Injectable()
export class AutoLayoutService {

    constructor(private store: Store<AppState>) {
    }

    public layoutGraph(graph: Graph, nodeDistance: number) {
        // Convert graph to a layout graph
        const nodeMap = {};
        for(const nodeId of Object.keys(graph.nodes)) {
            nodeMap[graph.nodes[nodeId].id.toString()] = {
                position: {...graph.nodes[nodeId].uiProps.position}
            };
        }
        const nodes: LayoutNode[] = Object.keys(nodeMap).map(id => nodeMap[id]);
        const edges: LayoutEdge[] = Object.keys(graph.connections)
            .map(id => graph.connections[id])
            .map(connection => ({
                from: nodeMap[connection.inputSocket.parent.id.toString()],
                to: nodeMap[connection.outputSocket.parent.id.toString()]
            }));

        new ForceDirectedGraphLayout({ nodes, edges }, nodeDistance).execute();

        for(const nodeId of Object.keys(nodeMap)) {
            this.store.dispatch(new MoveNode(graph.nodes[nodeId], {...nodeMap[nodeId].position}));
        }
    }
}