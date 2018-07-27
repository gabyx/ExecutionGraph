import { async, ComponentFixture, TestBed, getTestBed } from '@angular/core/testing';
import { Component } from '@angular/core';
import { By } from '@angular/platform-browser';

import { GraphComponent } from '@eg/graph/src/components/graph/graph.component';
import { PortComponent } from '@eg/graph/src/components/port/port.component';
import { ConnectionComponent } from '@eg/graph/src/components/connection/connection.component';

@Component({
  selector: 'test',
  template: `
<ngcs-graph>
  <ngcs-port id="s"></ngcs-port>
  <ngcs-port id="t"></ngcs-port>
  <ngcs-connection from="s" to="t"></ngcs-connection>
</ngcs-graph>
`})
export class TestComponent {}


describe('GraphComponent', () => {
  let component: TestComponent;
  let fixture: ComponentFixture<TestComponent>;
  beforeEach(
    async(() => {
      TestBed.configureTestingModule({
        declarations: [TestComponent, GraphComponent, PortComponent, ConnectionComponent]
      }).compileComponents();
    })
  );

  beforeEach(() => {
    fixture = TestBed.createComponent(TestComponent);
    component = fixture.componentInstance;
    
    fixture.detectChanges();
  });

  it('should draw a line when adding a connection', () => {
    
    let pathElement = fixture.debugElement.query(By.css('path'));
    expect(pathElement).toBeDefined();
    expect(pathElement.nativeElement.attributes.d.value).toBe("M0,0 C0,0 0,0 0,0");
  })
});
