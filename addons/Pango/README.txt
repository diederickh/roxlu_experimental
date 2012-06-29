Pango
=====
This addon enables you to use advanced text rendering using Pango and Cairo. 


Installation openFrameworks 0071
--------------------------------
1)	You only need to do this once for your openFrameworks download. Pango
	needs a slightly different version of cairo which is part of OF. Therefore
	you need to replace the libs/cairo with the files you can find in this addon.

		- Replace the "cairo" libs dir from your openframeworks/libs/cairo install with 
		install/replace_openframeworks_libs/cairo

		
2) 	Add the addon to your XCode project (tested with XCode 4.x) by dragging the 
	following directories into your project:
	
		- src
		- libs/32/ (for 32bit version, 64bit is on my todo list)

3)	Add a header search path to libs/32/include/, toggle the recursive flag.
	(You can easily add a search path by dragging the directory from Finder into
	the xCode settings).


	
Example:
--------
void testApp::draw(){
	ruFont font("Arial 20");

	ruPango pango;
	if(!pango.create(500,700)) {
		return;
	}
	
	pango.setColor(0.0, 0.0, 0.0, 1.0);
	pango.fill(1,1,1,0.4);
	pango.setMarkup("Nice font rendering with <b>Pango</b>  <span font_family=\"monospace\">PANGO</span>");\
	pango.setFont(font);	
	pango.alignCenter();
	pango.update();
	
	int w = pango.getPixelsWidth();
	int h = pango.getPixelsHeight();

	// We load pixel data using GL_BGRA because this is cairos internal format.
	ofImage img;
	img.allocate(w, h, OF_IMAGE_COLOR_ALPHA);
	img.getTextureReference().loadData(pango.getPixels(),w,h, GL_BGRA);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	img.draw(10,10);
}

