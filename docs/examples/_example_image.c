handle = gui_button_create(0, 10, 10, 300, 50, 0, 0, 0, 0);
gui_widget_settext(handle, _GT("Button"), 0);

handle = gui_image_create(0, 2, 2, bmimage_brand.xSize, bmimage_brand.ySize, 0, 0, 0, 0);
gui_image_setsource(handle, &bmimage_brand, 0);

handle = gui_image_create(0, 200, 40, bmimage_voyo.xSize, bmimage_voyo.ySize, 0, 0, 0, 0);
gui_image_setsource(handle, &bmimage_voyo, 0);

handle = gui_image_create(0, 350, 10, bmimage_voyo565.xSize, bmimage_voyo565.ySize, 0, 0, 0, 0);
gui_image_setsource(handle, &bmimage_voyo565, 0);