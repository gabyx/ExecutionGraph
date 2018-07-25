import { Component, OnInit } from '@angular/core';
import { Point } from '@eg/graph';
import { Node } from '../../model/Node';

@Component({
  selector: 'eg-workspace',
  templateUrl: './workspace.component.html',
  styleUrls: ['./workspace.component.scss']
})
export class WorkspaceComponent implements OnInit {
  
  public nodes: Node[] = [];
  
  constructor() {}

  ngOnInit() {
    this.nodes = [
      new Node("n1", "First Node", {x: 20, y: 100}),
      new Node("n2", "Second", { x: 300, y: 300 }),
      new Node("n3", "Third", { x: 400, y: 500 }),
    ]
  }

  public updateNodePosition(node: Node, position: Point) {
    console.log(`[WorkspaceComponent] Updating node position to ${position.x}:${position.y}`);
    node.uiProps.x = position.x;
    node.uiProps.y = position.y;
  }
}
