// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Mon Sep 10 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

/**
 * JSON stringify function which ignores circular reference.
 *
 * @export
 * @param {*} v
 * @returns JSON string of object `v`.
 */
export function stringify(v: any, space?: number) {
  const cache = new Map();
  return JSON.stringify(
    v,
    (key, value) => {
      if (typeof value === 'object' && value !== null) {
        if (cache.has(value)) {
          // Circular reference found, discard key
          return;
        }
        // Store value in our map
        cache.set(value, true);
      }
      return value;
    },
    space
  );
}
