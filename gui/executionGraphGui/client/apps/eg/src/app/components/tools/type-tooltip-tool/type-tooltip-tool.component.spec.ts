import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { SocketTypeTooltipComponent } from './type-tooltip.component';

describe('SocketTypeTooltipComponent', () => {
  let component: SocketTypeTooltipComponent;
  let fixture: ComponentFixture<SocketTypeTooltipComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [SocketTypeTooltipComponent]
    }).compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(SocketTypeTooltipComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
