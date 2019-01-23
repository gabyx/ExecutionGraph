import { async, ComponentFixture, TestBed } from '@angular/core/testing';
import { GraphModule } from '@eg/graph';

import { ConnectionLayerComponent } from './connection-layer.component';
import { Component, ViewChild } from '@angular/core';
import { StoreModule } from '@ngrx/store';
import { reducers } from '../../+state/reducers/app.reducers';
@Component({
  selector: 'eg-test',
  template: `
    <ngcs-graph> <eg-connection-layer #testComponent></eg-connection-layer> </ngcs-graph>
  `
})
export class TestComponent {
  @ViewChild('testComponent') connectionLayer: ConnectionLayerComponent;
}

describe('ConnectionLayerComponent', () => {
  let component: ConnectionLayerComponent;
  let fixture: ComponentFixture<TestComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      imports: [GraphModule, StoreModule.forRoot(reducers, { initialState: {} })],
      declarations: [TestComponent, ConnectionLayerComponent]
    }).compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(TestComponent);
    component = fixture.componentInstance.connectionLayer;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
