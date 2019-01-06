// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sun Dec 30 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

/**
 * In this way isDefined() will respect variable's type and
 * scoped code after this call would know that its not undefined or null.
 *
 * @template T
 * @param {(T | T[] | null)} value
 * @returns {value is T[]}
 */
export function isArray<T>(value: T | T[] | null): value is T[] {
  return Array.isArray(value);
}
