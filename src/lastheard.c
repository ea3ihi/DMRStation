

#include "main.h"


#define MAX_LH_ROWS 24

enum
{
   LHCALLSIGN_COLUMN,
   LHNAME_COLUMN,
   LH_TG_COLUMN,
   LHN_COLUMNS
};

GtkTreeIter   lhiter;
GtkTreeStore *lhstore;

extern GtkTreeView *treeLH;

GtkCellRenderer *lhrenderer;
GtkTreeViewColumn *lhcolumnCall;
GtkTreeViewColumn *lhcolumnName;
GtkTreeViewColumn *lhcolumnTG;

void lastheard_init(void)
{
	lhstore = gtk_tree_store_new(LHN_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_UINT);


	gtk_tree_view_set_model (treeLH, GTK_TREE_MODEL(lhstore));

	lhrenderer = gtk_cell_renderer_text_new();
	lhcolumnCall = gtk_tree_view_column_new_with_attributes ("Call",
	                                                   lhrenderer,
	                                                   "text", LHCALLSIGN_COLUMN,
	                                                   NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeLH), lhcolumnCall);

	lhrenderer = gtk_cell_renderer_text_new();

	 GValue val = G_VALUE_INIT;
	 g_value_init(&val, G_TYPE_INT);
	 g_value_set_int (&val, 160);

	g_object_set_property ( (GObject *) lhrenderer,
						   "wrap-width",
	                       &val);

	g_value_set_int (&val, PANGO_WRAP_WORD_CHAR);
	g_object_set_property ( (GObject *) lhrenderer,
							   "wrap-mode",
							   &val);

	lhcolumnName = gtk_tree_view_column_new_with_attributes ("Name",
		                                                   lhrenderer,
		                                                   "text", LHNAME_COLUMN,
														   NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeLH), lhcolumnName);

	lhrenderer = gtk_cell_renderer_text_new();
	lhcolumnTG = gtk_tree_view_column_new_with_attributes ("TG",
														   lhrenderer,
														   "text", LH_TG_COLUMN,
														   NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeLH), lhcolumnTG);


	//test
	/*
	lastHeardData_t lh = {
			.tg=1234,
			.call="EA3ASD",
			.name="NAME NAME 1"
	};
	lastHeardAdd(&lh);
	*/

}


void lastHeardAdd(lastHeardData_t *data)
{
	int number_of_rows = gtk_tree_model_iter_n_children(GTK_TREE_MODEL(lhstore), NULL);
	if (number_of_rows >= MAX_LH_ROWS)
	{
		if (gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(lhstore), &lhiter, NULL, number_of_rows-1))
		{
		   gtk_tree_store_remove(lhstore, &lhiter);
		}
	}


	gtk_tree_store_insert ((GtkTreeStore *) lhstore, &lhiter, NULL, 0);  /* Acquire an iterator */

	gtk_tree_store_set ((GtkTreeStore *) lhstore, &lhiter,
				LHCALLSIGN_COLUMN, data->call,
				LHNAME_COLUMN, data->name,
				LH_TG_COLUMN, data->tg,
		        -1);

	/* gtk_tree_view_scroll_to_point (GTK_TREE_VIEW (treeLH),
	                               0,
	                               0);*/

	GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(lhstore), &lhiter);
	gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (treeLH), path, NULL, false, 0.0, 0.0);

	gtk_tree_path_free(path);
}

/**
 * Add entry to LH by id
 */
void lastHeardAddById(uint32_t id)
{
	gpointer data = dmrids_lookup(id);
	lastHeardData_t lh;

	if (data)
	{
		char * call = strtok(data, " ");
		g_snprintf((char *) lh.call, LHCALL_SIZE, call);
		char * name = strtok(call+strlen(call)+1, "\0");
		if (name)
		{
			g_snprintf((char *) lh.name, LHNAME_SIZE, name);
		}
		else
		{
			g_snprintf((char *) lh.name, LHNAME_SIZE, " ");
		}

	}
	else
	{
		lastHeardData_t lh;
		g_snprintf((gchar *) lh.call, sizeof(lh.call), "%u", id);
		g_snprintf((gchar *) lh.name, sizeof(lh.name), " - - ");
		lastHeardAdd(&lh);
	}

	lastHeardAdd(&lh);
}

/**
 * Add entry to LH by src and dst
 */
void lastHeardAddByIdAndTG(uint32_t id, uint32_t dst)
{
	gpointer data = dmrids_lookup(id);
	lastHeardData_t lh;
	lh.tg = dst;
	if (data)
	{
		char * call = strtok(data, " ");
		g_snprintf((char *) lh.call, LHCALL_SIZE, call);
		char * name = strtok(call+strlen(call)+1, "\0");
		if (name)
		{
			g_snprintf((char *) lh.name, LHNAME_SIZE, name);
		}
		else
		{
			g_snprintf((char *) lh.name, LHNAME_SIZE, " ");
		}

	}
	else
	{
		g_snprintf((gchar *) lh.call, sizeof(lh.call), "%d", id);
		g_snprintf((gchar *) lh.name, sizeof(lh.name), " - - ");
		lastHeardAdd(&lh);
	}

	lastHeardAdd(&lh);
}


