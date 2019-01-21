import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { GraphOpenComponent } from './graph-open.component';

describe('GraphOpenComponent', () => {
  let component: GraphOpenComponent;
  let fixture: ComponentFixture<GraphOpenComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [GraphOpenComponent]
    }).compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(GraphOpenComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
