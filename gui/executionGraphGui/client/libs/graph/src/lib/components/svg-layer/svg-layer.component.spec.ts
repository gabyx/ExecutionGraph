import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { SvgLayerComponent } from './svg-layer.component';

describe('SvgLayerComponent', () => {
  let component: SvgLayerComponent;
  let fixture: ComponentFixture<SvgLayerComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ SvgLayerComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(SvgLayerComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
