import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { ConnectionLayerComponent } from './connection-layer.component';

describe('ConnectionLayerComponent', () => {
  let component: ConnectionLayerComponent;
  let fixture: ComponentFixture<ConnectionLayerComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ ConnectionLayerComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(ConnectionLayerComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
