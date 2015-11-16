
FLTK_SRC += fltk/gui.cxx

if USE_FLTK_TAB
FLTK_SRC += fltk/gui_fltk_tab.cxx
else
FLTK_SRC += fltk/gui_fltk_default.cxx
endif # USE_FLTK_TAB
