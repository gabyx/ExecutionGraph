// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sun Jan 06 2019
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

import { NgModule } from '@angular/core';
import { CommonModule as ngCommonModule } from '@angular/common';
import { FileInputComponent } from './components/file-input/file-input.component';

@NgModule({
  imports: [ngCommonModule],
  declarations: [FileInputComponent],
  exports: [FileInputComponent],
  providers: []
})
export class CommonModule {}
