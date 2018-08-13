/*

github.com/MOPO3OB/Tatchi
version: 0.0.1-dev

*/

#include <libxfce4panel/xfce-panel-plugin.h>

static void constructor(XfcePanelPlugin *plugin);
XFCE_PANEL_PLUGIN_REGISTER_INTERNAL(constructor);

char get_mode() {
	FILE *file;
	char mode;
	file = fopen("/etc/tatchi/mode", "r");
	if (file) {
		mode = getc(file);
		fclose(file);
	}
	if (mode == 's' || mode == 'm')
		return mode;
	else
		return 'f';
}

void set_icon(gint size, void *data) {
	char *file;
	char mode = get_mode();
        switch (mode) {
                case 'm':
                        file = "/etc/tatchi/multi_touch.png";
                        break;
                case 's':      
                case 'f':
                default:
                        file = "/etc/tatchi/single_touch.png";
        }

        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(file, NULL);
        pixbuf = gdk_pixbuf_scale_simple(GDK_PIXBUF(pixbuf), size, size, GDK_INTERP_BILINEAR);
        gtk_image_set_from_pixbuf(GTK_IMAGE(data), pixbuf);
}

static gboolean plugin_size_changed(XfcePanelPlugin *plugin, gint size, void *data) {
	size = xfce_panel_plugin_get_size(plugin) - 8;

	set_icon(size, data);
	return TRUE;
}	

void set_mode(char mode) {
	FILE *file;
	file = fopen("/etc/tatchi/mode", "w");
	if (file) {
		fprintf(file, "%c", mode);
		fclose(file);
	}
}


static void button_press(GtkWidget *widget, void *data) {
	char mode = get_mode();
        GdkPixbuf *pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(data));
	gint size = gdk_pixbuf_get_width(pixbuf);
	switch (mode) {
		case 's':
			set_mode('m');
			system("touchegg &");
			break;
		case 'm':
		case 'f':
		default:
			set_mode('s');
			system("pkill touchegg");
	}
	set_icon(size, data);
}

void init() {
	system("pkill touchegg");
	char mode = get_mode();
	if (mode == 'm')
		system("touchegg &");
	else
		set_mode('s');
}

static void
constructor(XfcePanelPlugin *plugin) {
	init();
        GtkWidget *icon = gtk_image_new();
        GtkWidget *button = gtk_button_new();
        gtk_button_set_image(GTK_BUTTON(button), icon);
        gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
	gtk_container_add(GTK_CONTAINER(plugin), button);
	g_signal_connect(button, "clicked", G_CALLBACK(button_press), icon);
	g_signal_connect(G_OBJECT(plugin), "size-changed", G_CALLBACK(plugin_size_changed), icon);
	gtk_widget_show_all(button);
}
