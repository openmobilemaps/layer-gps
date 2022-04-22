/*
 * Copyright (c) 2021 Ubique Innovation AG <https://www.ubique.ch>
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *  SPDX-License-Identifier: MPL-2.0
 */

import Foundation

class UBLocationManagerDelegateWrapper {
    let usage: UBLocationManager.LocationMonitoringUsage

    weak var delegate: UBLocationManagerDelegate?

    init(_ delegate: UBLocationManagerDelegate, usage: UBLocationManager.LocationMonitoringUsage) {
        self.delegate = delegate
        self.usage = usage
    }
}
