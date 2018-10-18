import { Component, OnInit, Input } from '@angular/core';
import { GraphComponent } from '@eg/graph/src/lib/components/graph/graph.component';

@Component({
  selector: 'ngcs-svg-layer',
  templateUrl: './svg-layer.component.html',
  styleUrls: ['./svg-layer.component.css']
})
export class SvgLayerComponent implements OnInit {

  constructor(private graph: GraphComponent) {
    console.log(`Attaching to parent graph `, graph);
  }

  get transformSvg() {
    return this.graph.transformSvg;
  }

  ngOnInit() {
  }

}
