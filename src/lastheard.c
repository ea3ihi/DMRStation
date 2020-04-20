

#include "main.h"


#define MAX_LH_ROWS 24

enum
{
   CALLSIGN_COLUMN,
   NAME_COLUMN,
   N_COLUMNS
};

GtkTreeIter   lhiter;
GtkTreeStore *lhstore;

extern GtkTreeView *treeLH;

GtkCellRenderer *lhrenderer;
GtkTreeViewColumn *lhcolumnCall;
GtkTreeViewColumn *lhcolumnName;

void lastheard_init(void)
{
	lhstore = gtk_tree_store_new  (N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING);


	gtk_tree_view_set_model (treeLH, GTK_TREE_MODEL(lhstore));

	lhrenderer = gtk_cell_renderer_text_new ();
	lhcolumnCall = gtk_tree_view_column_new_with_attributes ("Call",
	                                                   lhrenderer,
	                                                   "text", CALLSIGN_COLUMN,
	                                                   NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeLH), lhcolumnCall);

	lhcolumnName = gtk_tree_view_column_new_with_attributes ("Name",
		                                                   lhrenderer,
		                                                   "text", NAME_COLUMN,
		                                                   NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeLH), lhcolumnName);

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
				CALLSIGN_COLUMN, data->call,
				NAME_COLUMN, data->name,
		        -1);

}
