/*
 *  Multi2Sim Tools
 *  Copyright (C) 2011  Rafael Ubal (ubal@ece.neu.edu)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <visual-common.h>
#include <visual-memory.h>
#include <visual-evergreen.h>



/*
 * Main Program
 */

struct vi_cycle_bar_t *visual_cycle_bar;
struct vi_mem_panel_t *mem_panel;
struct vi_evg_panel_t *evg_panel;


static void visual_destroy_event(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();
}


static void visual_cycle_bar_refresh(void *user_data, long long cycle)
{
	vi_mem_panel_refresh(mem_panel);
	vi_evg_panel_refresh(evg_panel);
}


void visual_run(char *file_name)
{
	char *m2s_images_path = "images";

	/* Initialization */
	m2s_dist_file("close.png", m2s_images_path, m2s_images_path,
		vi_list_image_close_path, sizeof vi_list_image_close_path);
	m2s_dist_file("close-sel.png", m2s_images_path, m2s_images_path,
		vi_list_image_close_sel_path, sizeof vi_list_image_close_sel_path);

	/* Initialize visual state */
	vi_state_init(file_name);

	/* Initialize visual systems */
	vi_mem_system_init();
	vi_evg_gpu_init();

	/* Create checkpoints */
	vi_state_create_checkpoints();

	/* Initialize GTK */
	gtk_init(NULL, NULL);

	/* Create main window */
	GtkWidget *window;
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(visual_destroy_event), NULL);

	/* Vertical box */
	GtkWidget *vbox;
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	/* Cycle bar */
	vi_cycle_bar_init(visual_cycle_bar_refresh, NULL);
	gtk_box_pack_start(GTK_BOX(vbox), vi_cycle_bar_get_widget(), FALSE, FALSE, 0);

	/* Evergreen panel */
	evg_panel = vi_evg_panel_create();
	gtk_box_pack_start(GTK_BOX(vbox), vi_evg_panel_get_widget(evg_panel),
		FALSE, FALSE, 0);

	/* Memory system panel */
	mem_panel = vi_mem_panel_create();
	gtk_box_pack_start(GTK_BOX(vbox), vi_mem_panel_get_widget(mem_panel),
		TRUE, TRUE, 0);

	/* Show */
	gtk_widget_show_all(window);

	/* Run GTK */
	gtk_main();

	/* Free */
	vi_evg_gpu_done();
	vi_mem_system_done();
	vi_cycle_bar_done();
	vi_state_done();
}
