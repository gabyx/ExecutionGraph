import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { HtmlLayerComponent } from './html-layer.component';

describe('HtmlLayerComponent', () => {
  let component: HtmlLayerComponent;
  let fixture: ComponentFixture<HtmlLayerComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ HtmlLayerComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(HtmlLayerComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
