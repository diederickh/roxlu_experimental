#include <pango/TextSurface.h>
#include <iostream>
#include <stdio.h>

int main() {
  
  // Create surface with default arial font
  TextSurface ts;
  ts.setup("Arial 15", 500, 550);
  ts.fill(1.0, 0.3, 0.3, .1);

  // Set some text and render it onto the surfface
  ts.setText("Just testing some text rendering", 0.0, 0.0, 0.0);
  ts.render();

  // Move a bit downwards and render some other text, in another font
  ts.move(0,30);
  ts.setFont("Monaco 12");
 
  ts.setTextWidth(200);
  ts.wrapWord();
  ts.alignCenter();
  ts.alignRight(); 
  ts.setText("And some more text and\nanother line\nand even another line which is the longest", 1.0, 0.0, 0.0);
  ts.render();

  // Render with markup
  ts.alignLeft();
  ts.move(10, 100);
  ts.setMarkup("<span font='Courier 12' fgcolor='#000000'>And a bit in courier</span><span fgcolor='#00FFFF' underline='double'> and some more</span><span font='Helvetica Neue 15' bgcolor='#CC00CC' fgcolor='#FFFFFF'> and even more.</span>");
  ts.render();

  // Get text with in pixels
  PangoRectangle r = ts.getPixelExtents();
  printf("Dimensions of last render (x/y do not include move() calls)\nx: %d, y: %d, w: %d, h: %d\n", r.x, r.y, r.width, r.height);

  // And write to png
  ts.saveAsPNG("pango.png");

  return 0;
}
