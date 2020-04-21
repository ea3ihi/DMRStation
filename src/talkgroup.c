

#include "main.h"


#define MAX_TG_ROWS 150

enum
{
   TGID_COLUMN,
   TGNAME_COLUMN,
   TGN_COLUMNS
};

GtkTreeIter   tgiter;
GtkTreeStore *tgstore;

extern GtkTreeView *treeTG;

GtkCellRenderer *tgrenderer;
GtkTreeViewColumn *tgcolumnId;
GtkTreeViewColumn *tgcolumnName;
GtkTreeSelection *tgselect;

extern AppSettingsStruct_t settings;

static void
tg_selection_changed_cb (GtkTreeSelection *selection, gpointer data)
{
        GtkTreeIter iter;
        GtkTreeModel *model;
        uint32_t tg;

        if (gtk_tree_selection_get_selected (selection, &model, &iter))
        {
                gtk_tree_model_get (model, &iter, TGID_COLUMN, &tg, -1);

                if (tg != settings.currentTG)
                {
                	g_print ("Selected tg %d\n", tg);
                	activateTG(settings.dmrId, tg);
                }

                //g_free (tg);
        }
}

void talkgroup_init(void)
{
	tgstore = gtk_tree_store_new  (TGN_COLUMNS, G_TYPE_UINT, G_TYPE_STRING);

	gtk_tree_view_set_model (treeTG, GTK_TREE_MODEL(tgstore));

	tgrenderer = gtk_cell_renderer_text_new ();


	/*tgcolumnId = gtk_tree_view_column_new_with_attributes ("TG",
	                                                   tgrenderer,
	                                                   "text", TGID_COLUMN,
	                                                   NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeTG), tgcolumnId);
	*/

	tgcolumnName = gtk_tree_view_column_new_with_attributes ("Name",
		                                                   tgrenderer,
		                                                   "text", TGNAME_COLUMN,
		                                                   NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeTG), tgcolumnName);

	//test data
	talkgroupData_t tg;

	tg.id=91;
	g_snprintf((gchar *) &tg.name, TGNAME_SIZE, "World");
	talkgroupAdd(&tg);

	tg.id=214;
	g_snprintf((gchar *) &tg.name, TGNAME_SIZE, "Spain");
	talkgroupAdd(&tg);

	tg.id=21412;
	g_snprintf((gchar *)&tg.name, TGNAME_SIZE, "Galicia");
	talkgroupAdd(&tg);

	tg.id=21471;
	g_snprintf((gchar *)&tg.name, TGNAME_SIZE, "Free 21471");
	talkgroupAdd(&tg);

	tg.id=21460;
	g_snprintf((gchar *)&tg.name, TGNAME_SIZE, "Conf Catalana");
	talkgroupAdd(&tg);

	tg.id=2143;
	g_snprintf((gchar *)&tg.name, TGNAME_SIZE, "Catalunya");
	talkgroupAdd(&tg);

	tg.id=21438;
	g_snprintf((gchar *)&tg.name, TGNAME_SIZE, "Barcelona");
	talkgroupAdd(&tg);

	tg.id=21463;
	g_snprintf((gchar *)&tg.name, TGNAME_SIZE, "Free 21463");
	talkgroupAdd(&tg);

	tg.id=21462;
	g_snprintf((gchar *)&tg.name, TGNAME_SIZE, "Free 21462");
	talkgroupAdd(&tg);


	//selection and signals
	tgselect = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeTG));
	gtk_tree_selection_set_mode (tgselect, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (tgselect), "changed",
					  G_CALLBACK (tg_selection_changed_cb),
		                  NULL);

	GtkTreePath *path = gtk_tree_path_new_from_indices(4, -1);
	gtk_tree_selection_select_path(tgselect, path);
	gtk_tree_path_free(path);
}


void talkgroupAdd(talkgroupData_t *data)
{
	gchar buf[20];

	int number_of_rows = gtk_tree_model_iter_n_children(GTK_TREE_MODEL(tgstore), NULL);
	if (number_of_rows >= MAX_TG_ROWS)
	{
		if (gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(tgstore), &tgiter, NULL, number_of_rows-1))
		{
		   gtk_tree_store_remove(tgstore, &tgiter);
		}
	}

	g_snprintf(buf, 20, "%d %s", data->id, (gchar *)data->name);
	gtk_tree_store_append ((GtkTreeStore *) tgstore, &tgiter, NULL);

	gtk_tree_store_set ((GtkTreeStore *) tgstore, &tgiter,
				TGID_COLUMN, data->id,
				TGNAME_COLUMN, buf,
		        -1);

}
