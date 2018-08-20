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

import { async, ComponentFixture, TestBed } from "@angular/core/testing";

import { ConnectionStyleOptionsComponent } from "./connection-style-options.component";

describe("ConnectionStyleOptionsComponent", () => {
  let component: ConnectionStyleOptionsComponent;
  let fixture: ComponentFixture<ConnectionStyleOptionsComponent>;

  beforeEach(
    async(() => {
      TestBed.configureTestingModule({
        declarations: [ConnectionStyleOptionsComponent]
      }).compileComponents();
    })
  );

  beforeEach(() => {
    fixture = TestBed.createComponent(ConnectionStyleOptionsComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it("should create", () => {
    expect(component).toBeTruthy();
  });
});
