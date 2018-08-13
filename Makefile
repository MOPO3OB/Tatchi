MODULE = tatchi
NAME = Tatchi
COMMENT = Control your touchscreen
PLUGIN_ID = 101

FILES_DIR = /etc/$(MODULE)/
LIB_DIR = /usr/lib/arm-linux-gnueabihf/
PLUGINS_DIR = xfce4/panel/plugins/
SO_DIR = $(LIB_DIR)$(PLUGINS_DIR)
DESKTOP_DIR = /usr/share/$(PLUGINS_DIR)
CFLAGS = `pkg-config --cflags --libs libxfce4panel-2.0` -shared -fPIC -Wall
all:
	gcc -o lib$(MODULE).so $(CFLAGS) $(MODULE).c
	echo "[Xfce Panel]\nType=X-XFCE-PanelPlugin\nEncoding=UTF-8\nName=$(NAME)\nComment=$(COMMENT)\nIcon=\nX-XFCE-Module=$(MODULE)\nX-XFCE-Internal=true\nX-XFCE-Unique=true\nX-XFCE-API=2.0" > $(MODULE).desktop
install:
	mkdir -p $(FILES_DIR)
	install single_touch.png $(FILES_DIR)single_touch.png
	install multi_touch.png $(FILES_DIR)multi_touch.png
	install lib$(MODULE).so $(SO_DIR)
	install $(MODULE).desktop $(DESKTOP_DIR)
add:
	COMMAND="xfconf-query --channel 'xfce4-panel' --property '/panels/panel-1/plugin-ids'" ;\
	TEST=$$(xfconf-query --channel 'xfce4-panel' --property '/panels/panel-1/plugin-ids') ;\
	for id in $${TEST#*:}; do COMMAND="$$COMMAND --type int --set $$id"; done ;\
	bash -c "$$COMMAND --type int --set $(PLUGIN_ID)"
	xfconf-query --channel 'xfce4-panel' --property '/plugins/plugin-$(PLUGIN_ID)' --create --type string --set '$(MODULE)'
	xfce4-panel --restart
uninstall: remove
	rm -r $(FILES_DIR)
	rm $(SO_DIR)lib$(MODULE).so $(DESKTOP_DIR)$(MODULE).desktop
remove:
	pkill touchegg || true
	xfconf-query --channel 'xfce4-panel' --property '/plugins/plugin-$(PLUGIN_ID)' --reset
	xfce4-panel --restart
clean:
	rm lib$(MODULE).so $(MODULE).desktop
