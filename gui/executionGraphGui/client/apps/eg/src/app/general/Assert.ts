// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Wed Jan 23 2019
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================
import { environment } from '../../environments/environment';
import { assert } from '@eg/common/src';

/*
 * Assert funtion which is neglected depending on env flags.
 */
export function assertThat(condition: boolean, message?: string) {
  if (environment.asserts) {
    assert(condition, message);
  }
}
