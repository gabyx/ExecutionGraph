// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sun Jul 29 2018
//  @author Simon Spoerri, simon (døt) spoerri (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

import { async, ComponentFixture, TestBed, getTestBed } from '@angular/core/testing';
import { Component } from '@angular/core';
import { By } from '@angular/platform-browser';

import { GraphComponent } from './graph.component';
import { PortComponent } from '../port/port.component';

@Component({
  selector: 'ngcs-graph>',
  template: `
    <ngcs-graph>
      <ngcs-port id="s"></ngcs-port>
      <ngcs-port id="t"></ngcs-port>
      <ngcs-connection from="s" to="t"></ngcs-connection>
    </ngcs-graph>
  `
})
export class TestComponent {}

describe('GraphComponent', () => {
  let component: TestComponent;
  let fixture: ComponentFixture<TestComponent>;
  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [TestComponent, GraphComponent, PortComponent]
    }).compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(TestComponent);
    component = fixture.componentInstance;

    fixture.detectChanges();
  });

  it('should draw a line when adding a connection', () => {
    const pathElement = fixture.debugElement.query(By.css('path'));
    expect(pathElement).toBeDefined();
    expect(pathElement.nativeElement.attributes.d.value).toBe('M0,0 C0,0 0,0 0,0');
  });
});
