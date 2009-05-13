/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2009 Red Hat, Inc.
 *
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author: David Zeuthen <davidz@redhat.com>
 */

#include "config.h"

#include <polkitbackend/polkitbackend.h>

#include <glib/gi18n-lib.h>

#define DEVKIT_DISKS_TYPE_ACTION_LOOKUP          (devkit_disks_action_lookup_get_type())
#define DEVKIT_DISKS_ACTION_LOOKUP(o)            (G_TYPE_CHECK_INSTANCE_CAST ((o), DEVKIT_DISKS_TYPE_ACTION_LOOKUP, DevkitDisksActionLookup))
#define DEVKIT_DISKS_ACTION_LOOKUP_CLASS(k)      (G_TYPE_CHECK_CLASS_CAST((k), DEVKIT_DISKS_TYPE_ACTION_LOOKUP, DevkitDisksActionLookupClass))
#define DEVKIT_DISKS_ACTION_LOOKUP_GET_CLASS(o)  (G_TYPE_INSTANCE_GET_CLASS ((o), DEVKIT_DISKS_TYPE_ACTION_LOOKUP, DevkitDisksActionLookupClass))
#define DEVKIT_DISKS_IS_ACTION_LOOKUP(o)         (G_TYPE_CHECK_INSTANCE_TYPE ((o), DEVKIT_DISKS_TYPE_ACTION_LOOKUP))
#define DEVKIT_DISKS_IS_ACTION_LOOKUP_CLASS(k)   (G_TYPE_CHECK_CLASS_TYPE ((k), DEVKIT_DISKS_TYPE_ACTION_LOOKUP))

typedef struct _DevkitDisksActionLookup DevkitDisksActionLookup;
typedef struct _DevkitDisksActionLookupClass DevkitDisksActionLookupClass;

struct _DevkitDisksActionLookup
{
        GObject parent;
};

struct _DevkitDisksActionLookupClass
{
        GObjectClass parent_class;
};

GType           devkit_disks_action_lookup_get_type     (void) G_GNUC_CONST;

static void polkit_backend_action_lookup_iface_init (PolkitBackendActionLookupIface *iface);

#define _G_IMPLEMENT_INTERFACE_DYNAMIC(TYPE_IFACE, iface_init)                                                \
{                                                                                                             \
        const GInterfaceInfo g_implement_interface_info = {                                                   \
                (GInterfaceInitFunc) iface_init, NULL, NULL                                                   \
        };                                                                                                    \
        g_type_module_add_interface (type_module, g_define_type_id, TYPE_IFACE, &g_implement_interface_info); \
}

G_DEFINE_DYNAMIC_TYPE_EXTENDED (DevkitDisksActionLookup,
                                devkit_disks_action_lookup,
                                G_TYPE_OBJECT,
                                0,
                                _G_IMPLEMENT_INTERFACE_DYNAMIC (POLKIT_BACKEND_TYPE_ACTION_LOOKUP,
                                                                polkit_backend_action_lookup_iface_init))

static void
devkit_disks_action_lookup_init (DevkitDisksActionLookup *lookup)
{
}

static void
devkit_disks_action_lookup_class_finalize (DevkitDisksActionLookupClass *klass)
{
}

static void
devkit_disks_action_lookup_class_init (DevkitDisksActionLookupClass *klass)
{
}

/* ---------------------------------------------------------------------------------------------------- */

typedef struct
{
        const gchar *name;
        const gchar *message;
} Map;

static const Map map[] = {
        {
                "PartitionDelete",
                N_("Authentication is required to delete a partition"),
        },
        {
                "FilesystemCreate",
                N_("Authentication is required to create a filesystem")
        },
        {
                "PartitionCreate",
                N_("Authentication is required to create a partition")
        },
        {
                "PartitionModify",
                N_("Authentication is required to modify a partition")
        },
        {
                "PartitionTableCreate",
                N_("Authentication is required to create a partition table")
        },
        {
                "FilesystemSetLabel",
                N_("Authentication is required to set the file system label")
        },
        {
                "LinuxMdStop",
                N_("Authentication is required to stop a Software RAID device")
        },
        {
                "LinuxMdAddComponent",
                N_("Authentication is required to add a new component to a Software RAID device")
        },
        {
                "LinuxMdRemoveComponent",
                N_("Authentication is required to remove a component from a Software RAID device")
        },
        {
                "LinuxMdStart",
                N_("Authentication is required to start a Software RAID device")
        },
        {
                "DriveInhibitPolling",
                N_("Authentication is required to inhibit polling on a drive")
        },
        {
                "DrivePollMedia",
                N_("Authentication is required to poll for media")
        },
        {
                "InhibitAllPolling",
                N_("Authentication is required to inhibit all drive polling")
        },
        {
                NULL,
                NULL,
        }
};


static gchar *
devkit_disks_action_lookup_get_message   (PolkitBackendActionLookup *lookup,
                                          const gchar               *action_id,
                                          GHashTable                *details,
                                          PolkitActionDescription   *action_description)
{
        const gchar *operation;
        gchar *ret;
        guint n;

        ret = NULL;

        if (!g_str_has_prefix (action_id, "org.freedesktop.devicekit.disks."))
                goto out;

        operation = g_hash_table_lookup (details, "operation");
        if (operation == NULL)
                goto out;

        for (n = 0; map[n].name != NULL; n++) {
                if (g_strcmp0 (map[n].name, operation) != 0)
                        continue;

                ret = g_strdup (g_dgettext (GETTEXT_PACKAGE, map[n].message));
                break;
        }

 out:
        return ret;
}

static gchar *
devkit_disks_action_lookup_get_icon_name (PolkitBackendActionLookup *lookup,
                                          const gchar               *action_id,
                                          GHashTable                *details,
                                          PolkitActionDescription   *action_description)
{
        gchar *ret;

        ret = NULL;

        if (!g_str_has_prefix (action_id, "org.freedesktop.devicekit.disks."))
                goto out;

        /* explicitly left blank for now */

 out:
        return ret;
}

static GHashTable *
devkit_disks_action_lookup_get_details   (PolkitBackendActionLookup *lookup,
                                          const gchar               *action_id,
                                          GHashTable                *details,
                                          PolkitActionDescription   *action_description)
{
        const gchar *s;
        const gchar *s2;
        const gchar *s3;
        GString *str;
        GHashTable *ret;

        if (!g_str_has_prefix (action_id, "org.freedesktop.devicekit.disks."))
                return NULL;

        ret = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, g_free);

        /* see devkit_disks_daemon_local_check_auth() in devkit-disks-daemon.c
         * for where these keys are set
         */

        s = g_hash_table_lookup (details, "unix-device");
        if (s != NULL)
                g_hash_table_insert (ret, _("Device"), g_strdup (s));

        s = g_hash_table_lookup (details, "drive-vendor");
        s2 = g_hash_table_lookup (details, "drive-model");
        s3 = g_hash_table_lookup (details, "drive-revision");
        str = g_string_new (NULL);
        if (s != NULL) {
                g_string_append (str, s);
        }
        if (s2 != NULL) {
                if (str->len > 0)
                        g_string_append_c (str, ' ');
                g_string_append (str, s2);
        }
        if (s3 != NULL) {
                if (str->len > 0)
                        g_string_append_c (str, ' ');
                g_string_append_printf (str, "(%s)", s3);
        }

        if (str->len > 0) {
                g_hash_table_insert (ret, _("Drive"), g_strdup (str->str));
        }
        g_string_free (str, TRUE);

        return ret;
}

static void
polkit_backend_action_lookup_iface_init (PolkitBackendActionLookupIface *iface)
{
        iface->get_message   = devkit_disks_action_lookup_get_message;
        iface->get_icon_name = devkit_disks_action_lookup_get_icon_name;
        iface->get_details   = devkit_disks_action_lookup_get_details;
}

/* ---------------------------------------------------------------------------------------------------- */

void
g_io_module_load (GIOModule *module)
{
        bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
        bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");

        devkit_disks_action_lookup_register_type (G_TYPE_MODULE (module));

        g_io_extension_point_implement (POLKIT_BACKEND_ACTION_LOOKUP_EXTENSION_POINT_NAME,
                                        DEVKIT_DISKS_TYPE_ACTION_LOOKUP,
                                        "DeviceKit-disks action lookup extension " PACKAGE_VERSION,
                                        0);
}

void
g_io_module_unload (GIOModule *module)
{
}