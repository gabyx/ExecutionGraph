import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { GraphSaveComponent } from './graph-save.component';

describe('GraphSaveComponent', () => {
  let component: GraphSaveComponent;
  let fixture: ComponentFixture<GraphSaveComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [GraphSaveComponent]
    }).compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(GraphSaveComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
