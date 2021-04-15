#include <printf.h>
#include <stdio.h>
#include <glib.h>

G_LOCK_DEFINE_STATIC (unique_pointer);
static GHashTable *unique_pointer_ht = NULL;
static gsize unique_pointer_id = 0;

static gconstpointer
uniquify_pointer (gconstpointer ptr)
{
  gpointer res;

  if (ptr == NULL)
    return NULL;

  G_LOCK (unique_pointer);
  if (unique_pointer_ht == NULL)
    unique_pointer_ht = g_hash_table_new (g_direct_hash, g_direct_equal);

  res = g_hash_table_lookup (unique_pointer_ht, ptr);
  if (res == NULL)
    {
      res = (gpointer) ++unique_pointer_id;
      g_hash_table_insert (unique_pointer_ht, (gpointer)ptr, res);
    }

  G_UNLOCK (unique_pointer);

  return res;
}

static char *
append_hex_u64 (char *p, guint64 val)
{
  const char hexdigits[] = "0123456789abcdef";
  gboolean leading_digit = TRUE;

  for (guint i = 0; i < 16; i++)
    {
      char c = hexdigits[(val >> (60 - 4*i)) & 0xf];
      if (!leading_digit || c != '0')
        {
          *p++ = c;
          leading_digit = FALSE;
        }
    }

  return p;
}

static int
unique_pointer_format (FILE *stream,
                       const struct printf_info *info,
                       const void *const *args)
{
  const char *ptr = *(char **)args[0];
  size_t written = 0;
  char buf[2 + 16]; /* Space for 0u + 64bit hex number  */
  char *to_write;
  size_t to_write_len;
  ssize_t padding;

  if (ptr != NULL)
    {
      guint64 value = (guint64)uniquify_pointer (ptr);
      char *p = buf;

      *p++ = '0';
      *p++ = 'u';

      p = append_hex_u64 (p, value);

      to_write = buf;
      to_write_len = p-buf;
    }
  else
    {
      to_write = "(nil)";
      to_write_len = strlen(to_write);
    }

  padding = info->width - to_write_len;
  if (padding > 0 && !info->left)
    {
      for (int i = 0; i < padding; i++)
        written += fwrite (" ", 1, 1, stream);
    }

  written += fwrite (to_write, 1, to_write_len, stream);

  if (padding > 0 && info->left)
    {
      for (int i = 0; i < padding; i++)
        written += fwrite (" ", 1, 1, stream);
    }

  return (int)written;
}

static int
unique_pointer_arginfo (const struct printf_info *info,
                        size_t n, int *argtypes,
                        int *size)
{
  if (n >= 1)
    {
      argtypes[0] = PA_POINTER;
      size[0] = -1; /* No custom size */
    }
  return 1;
}

__attribute__((constructor))
static void
unique_pointer_printf_init (void)
{
  register_printf_specifier ('p', unique_pointer_format,  unique_pointer_arginfo);
}
