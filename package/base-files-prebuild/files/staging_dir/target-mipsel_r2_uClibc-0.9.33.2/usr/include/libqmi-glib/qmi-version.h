/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 * Copyright (C) 2013 Lanedo GmbH
 */

#ifndef _QMI_VERSION_H_
#define _QMI_VERSION_H_

/**
 * SECTION:qmi-version
 * @short_description: Version information in the API.
 *
 * This section defines types that are used to identify the libqmi-glib version.
 **/

/**
 * QMI_MAJOR_VERSION:
 *
 * Evaluates to the major version number of libqmi-glib which this source
 * is compiled against.
 */
#define QMI_MAJOR_VERSION (1)

/**
 * QMI_MINOR_VERSION:
 *
 * Evaluates to the minor version number of libqmi-glib which this source
 * is compiled against.
 */
#define QMI_MINOR_VERSION (8)

/**
 * QMI_MICRO_VERSION:
 *
 * Evaluates to the micro version number of libqmi-glib which this source
 * compiled against.
 */
#define QMI_MICRO_VERSION (0)

/**
 * QMI_CHECK_VERSION:
 * @major: major version (e.g. 1 for version 1.2.5)
 * @minor: minor version (e.g. 2 for version 1.2.5)
 * @micro: micro version (e.g. 5 for version 1.2.5)
 *
 * Returns: %TRUE if the version of the libqmi-glib header files
 * is the same as or newer than the passed-in version.
 */
#define QMI_CHECK_VERSION(major,minor,micro)                         \
    (QMI_MAJOR_VERSION > (major) ||                                  \
     (QMI_MAJOR_VERSION == (major) && QMI_MINOR_VERSION > (minor)) || \
     (QMI_MAJOR_VERSION == (major) && QMI_MINOR_VERSION == (minor) && QMI_MICRO_VERSION >= (micro)))

#endif /* _QMI_VERSION_H_ */
