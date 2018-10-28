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

import { DraggableDirective } from './draggable.directive';
import { ElementRef } from '@angular/core';

describe('DraggableDirective', () => {
  it('should create an instance', () => {
    const directive = new DraggableDirective(new ElementRef(null));
    expect(directive).toBeTruthy();
  });
});
