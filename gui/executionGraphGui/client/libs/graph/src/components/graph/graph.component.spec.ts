import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { GraphComponent } from './graph.component';

describe('GraphComponent', () => {
  let component: GraphComponent;
  let fixture: ComponentFixture<GraphComponent>;

  beforeEach(
    async(() => {
      TestBed.configureTestingModule({
        declarations: [GraphComponent]
      }).compileComponents();
    })
  );

  beforeEach(() => {
    fixture = TestBed.createComponent(GraphComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });

  it('should throw when adding a connection to an inexistant port', () => {
    expect(() => component.registerConnection('foo', 'bar')).toThrow();
  })
});
