import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { BackendTestComponent } from './backend-test.component';

describe('BackendTestComponent', () => {
  let component: BackendTestComponent;
  let fixture: ComponentFixture<BackendTestComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ BackendTestComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(BackendTestComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
