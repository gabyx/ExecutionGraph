import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { ConnectionStyleOptionsComponent } from './connection-style-options.component';

describe('ConnectionStyleOptionsComponent', () => {
  let component: ConnectionStyleOptionsComponent;
  let fixture: ComponentFixture<ConnectionStyleOptionsComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ ConnectionStyleOptionsComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(ConnectionStyleOptionsComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
