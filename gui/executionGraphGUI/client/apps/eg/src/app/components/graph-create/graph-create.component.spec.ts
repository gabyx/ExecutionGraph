import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { GraphCreateComponent } from './graph-create.component';

describe('GraphCreateComponent', () => {
  let component: GraphCreateComponent;
  let fixture: ComponentFixture<GraphCreateComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ GraphCreateComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(GraphCreateComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
