import { Component, OnInit } from '@angular/core';
import { GraphComponent } from '../graph/graph.component';
import { LoggerFactory, ILogger } from '@eg/logger/src';

@Component({
  selector: 'ngcs-html-layer',
  templateUrl: './html-layer.component.html',
  styleUrls: ['./html-layer.component.scss']
})
export class HtmlLayerComponent implements OnInit {
  private readonly logger: ILogger;

  constructor(private graph: GraphComponent, loggerFactory: LoggerFactory) {
    this.logger = loggerFactory.create('HtmlLayerComponent');
    this.logger.debug(`Attaching to parent graph `, graph);
  }

  public get transformCss() {
    return this.graph.transformCss;
  }

  ngOnInit() {
  }

}
