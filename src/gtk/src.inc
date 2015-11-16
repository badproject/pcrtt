GTK_GLADE := $(top_srcdir)/src/gtk/gtk_tab.glade
GTK_GEN_HEADER := $(top_builddir)/src/gtk/gtk_glade.h
GTK_SRC += gtk/gui.cxx

-include generator-gtk
.PHONY: generator-gtk

generator-gtk: $(GTK_GEN_HEADER)

$(GTK_GEN_HEADER):
	@echo -e "static const gsize gtk_glade_size=$$(wc -c <$(GTK_GLADE));\n" \
	 >> $@
	@echo "static const gchar gtk_glade_src[] = " >> $@
	@sed -e 's/["\]/\\&/g' -e 's/^/    "/' -e 's/$$/\\n"/' -e '$$s/$$/;/' \
	 $(GTK_GLADE) >> $@

AM_CXXFLAGS += $(GTK_CFLAGS) -I$(top_builddir)/src/gtk
AM_LDFLAGS += $(GTK_LIBS)

EXTRA_DIST += gtk/gtk_tab.glade	\
				gtk/gui.h		\
				gtk/src.inc
