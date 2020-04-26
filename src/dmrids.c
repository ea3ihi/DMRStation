


#include "main.h"

GHashTable *dmridstable;

/**
 * Read and parse DMR ids file
 */
void dmrids_init(void)
{
	FILE *fp;
	char buf[1024];

	dmridstable = g_hash_table_new (g_int_hash, g_int_equal);

	fp = fopen ("DMRIds.dat", "r");
	/* If file does not exist, exit. */
	if (!fp)
	{
		return;
	}

	int key;
	char *keyStr;
	char *value;

	while (fgets (buf, sizeof (buf), fp))
	    {
			/* Get the first and the second field. */
	        keyStr = strtok (buf, " ");
	        if (!keyStr) continue;

	        key = atoi(keyStr);
	        value = strtok (NULL, "\n");
	        if (!value) continue;

	        /* Look up the table for an existing key.
	         * If it exists replace it by inserting the new ke-value pair and
	         * freeing the old key-value pair */
	        int *old_key;
	        char *old_value;
	        int *knew = g_new0 (gint, 1);
	        *knew = key;

	        /* Try looking up this key. */
	        if (g_hash_table_lookup_extended (dmridstable, knew, &old_key, &old_value))
	        {
	            /* Insert the new value */
	            g_hash_table_insert (dmridstable, knew, g_strdup (value));
	            /* Just free the key and value */
	            g_free (old_key);
	            g_free (old_value);
	        }
	        else
	        {
	            /* Insert into our hash table it is not a duplicate. */
	            g_hash_table_insert (dmridstable, knew, g_strdup (value));
	        }
	    }
	    /* Close the file when done. */
	    fclose (fp);
}

void dmrids_deinit(void)
{
	g_hash_table_destroy (dmridstable);
}

/**
 * Get user info from DMR id
 */
gpointer dmrids_lookup(uint32_t key)
{
	return g_hash_table_lookup (dmridstable,
	                              &key
	                              );
}
