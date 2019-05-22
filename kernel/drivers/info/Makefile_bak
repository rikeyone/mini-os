#Delete obj file first

#OBJFILE=$(objtree)/fs/proc/info/config_info.o
#$(shell rm $(OBJFILE) -f)


#FORCE will make sure this obj be compiled every time
obj-y += config_info.o
$(obj)/config_info.o: FORCE fs/proc/info/config_info.h

fs/proc/info/config_info.h: FORCE
	$(Q)$(CONFIG_SHELL) $(srctree)/fs/proc/info/mkconfiginfo $(objtree) $@

