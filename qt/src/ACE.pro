
lessThan(QT_MAJOR_VERSION,5): error("Requires Qt 5")

TEMPLATE = subdirs

SUBDIRS += \
   core \
   gui \
   example

gui.depends = core
example.depends = core gui

# make OpenCLEvent NOTHROW exception class lolz(public EOpenCL)

# redo metadata model code to work with new metadata (do NOT do any new functions yet!)
