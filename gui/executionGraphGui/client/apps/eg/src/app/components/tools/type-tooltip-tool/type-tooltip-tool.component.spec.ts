import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { TypeToolTipToolComponent } from './type-tooltip-tool.component';

describe('TypeToolTipToolComponent', () => {
  let component: TypeToolTipToolComponent;
  let fixture: ComponentFixture<TypeToolTipToolComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [TypeToolTipToolComponent]
    }).compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(TypeToolTipToolComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
