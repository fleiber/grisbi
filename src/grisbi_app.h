#ifndef __GRISBI_APP_H__
#define __GRISBI_APP_H__

#include <gtk/gtk.h>

/*START_INCLUDE*/
#include "grisbi_window.h"
/*END_INCLUDE*/

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GRISBI_TYPE_APP              (grisbi_app_get_type())
#define GRISBI_APP(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), GRISBI_TYPE_APP, GrisbiApp))
#define GRISBI_APP_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), GRISBI_TYPE_APP, GrisbiAppClass))
#define GRISBI_IS_APP(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), GRISBI_TYPE_APP))
#define GRISBI_IS_APP_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GRISBI_TYPE_APP))
#define GRISBI_APP_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), GRISBI_TYPE_APP, GrisbitAppClass))

/* Private structure type */
typedef struct _GrisbiAppPrivate GrisbiAppPrivate;


/* Main object structure */
typedef struct _GrisbiApp GrisbiApp;

struct _GrisbiApp
{
    GObject object;

    /*< private > */
    GrisbiAppPrivate *priv;
};

/* Class definition */
typedef struct _GrisbiAppClass GrisbiAppClass;

struct _GrisbiAppClass
{
    GObjectClass parent_class;
};


/* Public methods */

/* construction */
GType grisbi_app_get_type ( void ) G_GNUC_CONST;
GrisbiApp *grisbi_app_get_default ( void );
gboolean grisbi_app_get_first_use ( GrisbiApp *app );
GrisbiWindow *grisbi_app_create_window ( GrisbiApp *app,
                        GdkScreen *screen );
GrisbiWindow *grisbi_app_get_active_window ( GrisbiApp *app );
const GList	*grisbi_app_get_windows ( GrisbiApp *app );

G_END_DECLS

#endif  /* __GRISBI_APP_H__ */