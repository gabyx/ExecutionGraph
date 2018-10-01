// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sat Sep 15 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

/**
 * UI Properties for the Inspector
 *
 * @export
 * @class UIPropertiesInspector
 */
export class UIPropertiesInspector {
  public isVisible: boolean = true;
}

/**
 * UI Properties for the Application
 *
 * @export
 * @class UIProperties
 */
export class UIProperties {
  public inspector?: UIPropertiesInspector;
}

export function createDefaultUIProperties() {
  return { inspector: new UIPropertiesInspector() };
}
